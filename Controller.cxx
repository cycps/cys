#include "Controller.hxx"

#include <thread>
#include <chrono>
#include <csignal>
#include <stdexcept>
#include <string>
#include <iostream>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>

using std::string;
using namespace cys;
using std::endl;
using std::this_thread::sleep_for;
using std::chrono::milliseconds;
using std::lock_guard;
using std::mutex;
using std::thread;
using std::runtime_error;
using std::to_string;
using std::ostream;
using std::make_shared;
using namespace std::chrono;
using std::vector;


// InputSource -----------------------------------------------------------------
in6_addr InputSource::address()
{
  in6_addr va, sa;

  addrinfo hints, *vai{nullptr}, *sai{nullptr};
  bzero(&hints, sizeof(addrinfo));
  hints.ai_family = AF_INET6;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_protocol = IPPROTO_UDP;

  int ret = getaddrinfo(variable.c_str(), nullptr, &hints, &vai);

  if(ret != 0) {
    std::cerr << "failed to get ipv6 udp address for '" << variable << "'" 
              << std::endl;
    exit(1);
  }
  
  va = ((sockaddr_in6*)(vai->ai_addr))->sin6_addr;
  char va_s[INET6_ADDRSTRLEN];
  bzero(va_s, INET6_ADDRSTRLEN);
  inet_ntop(AF_INET6, &va, va_s, INET6_ADDRSTRLEN);

  std::cout << "[" << variable << "] --> [" << va_s << std::endl;


  //the magic of dns takes place

  return va;
}

// Controller ------------------------------------------------------------------
void Controller::listen()
{

  for(auto &is : sources) {
    in6_addr addr = is.address();
  }
  
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  int opt=1;
  int err = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  err = setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));

  if(sockfd < 0)
    throw runtime_error{"socket() failed: " + to_string(sockfd)};

  io_lg << log("got socket " + to_string(sockfd)) << endl;

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(port);

  const struct sockaddr *addr = reinterpret_cast<const struct sockaddr*>(&servaddr);
  err = bind(sockfd, addr, sizeof(servaddr));
  if(err < 0)
  {
    io_lg << ts() << "bind error: " << errno << endl;
    close(sockfd);
    //shutdown(sockfd, SHUT_RDWR);
    throw runtime_error{"bind() failed: " + to_string(err)};
  }

  io_lg << log("bound to port " + to_string(port)) << endl;

  io_lg << log("Listening") << endl;

  listener_bound = true;

  io();
}

void Controller::swapBuffers()
{
  lock_guard<mutex> lk(io_mtx);

  std::swap(a,b);
  a->buf = b->buf;
  b->clear();
}

void Controller::computeFrame()
{
  for(size_t i=0; i<a->size(); ++i)
  {
    auto cv = resolvers[i](a->buf[i], input_frame[i]);
    input_frame[i] = cv;
    //TODO should be able to specify what is done when after a frame is
    //computed
    /*
    if(!a->buf[i].empty())
    {
      CVal last = a->buf[i].back();
      a->buf[i].clear();
      last.v = cv;
      a->buf[i].push_back(last);

    }
    */
  }
}

void Controller::tx(double v)
{
  auto *addr = reinterpret_cast<struct sockaddr*>(&tgtaddr);
  
  auto tp = high_resolution_clock::now();
  auto dur = tp.time_since_epoch();
  
  size_t sec = duration_cast<seconds>(dur).count();
  dur -= seconds(sec);
  size_t usec = duration_cast<microseconds>(dur).count();

  cpk.sec = sec;
  cpk.usec = usec;
  cpk.value = v;

  char buf[sizeof(cpk)];
  cpk.toBytes(buf);

  sendto(sockfd, buf, sizeof(cpk), 0, addr, sizeof(tgtaddr));
}

void Controller::setDestination(string addr)
{
  bzero(&tgtaddr, sizeof(tgtaddr));
  tgtaddr.sin_family = AF_INET;
  tgtaddr.sin_port = htons(4747);
  int err = inet_pton(AF_INET, addr.c_str(), &tgtaddr.sin_addr);
  if(err < 0)
  {
    io_lg << ts() << "Bad destination address: " << addr << endl;
    throw runtime_error{"io failure"};
  }
}

void Controller::setTarget(unsigned long id_tag)
{
  cpk.id_tag = id_tag;
}

size_t Controller::setInput(unsigned long id_tag)
{
  imap[id_tag] = input_size;
  input_frame.push_back(0);
  resolvers.push_back(UseLatestArrival);
  a_.buf.push_back({});
  b_.buf.push_back({});
  return input_size++;
}


void Controller::kernel()
{
  k_lg << log("Kernel started") << endl;
  while(true)
  {
    swapBuffers();
    computeFrame();
    compute();
    sleep_for(milliseconds(period));
  }
}

void Controller::io()
{
  socklen_t len;
  constexpr size_t sz{sizeof(CPacket)};
  char msg[sz];
  auto *addr = reinterpret_cast<struct sockaddr*>(&cliaddr);

  io_lg << log("entering io loop") << endl;
  while(true)
  {
    //BLOCKING!!
    int err = recvfrom(sockfd, msg, sz, 0, addr, &len);
    if(err < 0)
      throw runtime_error{"recvfrom() failed: " + to_string(err)};

    CPacket pkt;
    try
    {
      pkt = CPacket::fromBytes(msg);
    }
    catch(runtime_error &ex)
    {
      io_lg << ts() << "packet read error: " << ex.what() << endl;
      continue;
    }

    if(imap.find(pkt.id_tag) == imap.end())
    {
      io_lg << ts() << "dropped: " << pkt << endl;
      continue;
    }

    io_lg << ts() << "accepted: " << pkt << endl;

    
    lock_guard<mutex> lk(io_mtx);
    a->buf[imap[pkt.id_tag]].push_back({pkt.sec, pkt.usec, pkt.value});  
  }
}

void Controller::run()
{
  k_lg << log("up") << endl;

  thread t_io([this](){listen();});
  while(!listener_bound){ usleep(10); }
  thread t_k([this](){kernel();});

  t_io.join();
  t_k.join();

  k_lg << log("down") << endl;

}

//Resolvers -------------------------------------------------------------------

double cys::UseLatestArrival(const std::vector<CVal> &v, double last)
{
  if(v.empty()) return last;

  return v.back().v;
}
