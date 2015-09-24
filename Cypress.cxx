#include <stdexcept>

#include "Cypress.hxx"
#include "Actuator.hxx"
#include "Sensor.hxx"

using namespace cys;
using std::cerr;
using std::cout;
using std::endl;
using std::thread;
using std::string;
using std::ostream;
using std::runtime_error;
using std::lock_guard;
using std::mutex;

ActuationServer::ActuationServer(Sim &sim) 
  : sim{&sim} 
{
  initComms();
  listen_thread = new thread([this](){ listen(); });
  listen_thread->detach();
}

void ActuationServer::enlist(Actuator &a)
{
 actuators[a.id_tag] = &a;
}

void ActuationServer::initComms()
{
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if(sockfd < 0)
  {
    cerr << "[ActuationServer] socket() failed" << endl;
    exit(1);
  }
  bzero(&addr, sizeof(addr)); 
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);

  auto *saddr = reinterpret_cast<const struct sockaddr*>(&addr);
  int err = bind(sockfd, saddr, sizeof(saddr));
  if(err < 0)
  {
    cerr << "[ActuationServer] bind() failed" << endl;
    exit(1);
  }

  cout << "[ActuationServer] bound to port " << port << endl;
}

void ActuationServer::listen()
{
  socklen_t len;
  constexpr size_t sz{sizeof(CPacket)};
  char msg[sz];
  sockaddr_in client_addr;
  auto *caddr = reinterpret_cast<sockaddr*>(&client_addr);

  cout << "[ActuationServer] listening" << endl;

  for(;;)
  {
    int err = recvfrom(sockfd, msg, sz, 0, caddr, &len);
    if(err < 0)
    {
      cerr << "[ActuationServer] recvfrom() failed: " << err << endl;
      exit(1);
    }

    CPacket pkt;
    try { pkt = CPacket::fromBytes(msg); }
    catch(runtime_error &ex)
    {
      cerr << "[ActuationServer] packet read error: " << ex.what() << endl;
      continue;
    }

    lock_guard<mutex> lk(rx_mtx);
    try { actuators.at(pkt.id_tag)->actuate(pkt.value); }
    catch(std::out_of_range)
    {
      cout << "[ActuationServer] unknown id_tag " << pkt.id_tag << endl;
      continue;
    }
  }
}

//CPacket +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CPacket CPacket::fromBytes(char *buf)
{
  unsigned long id_tag, sec, usec;
  double value;

  char head[5];
  strncpy(head, buf, 4);
  head[4] = 0;

  if(strncmp("cypr", head, 4) != 0)
  {
    throw runtime_error("Bad packet header `" + string(head) + "`");
  }

  size_t at = 4;
  id_tag = be64toh(*reinterpret_cast<unsigned long*>(buf+at));
  at += sizeof(unsigned long);
  sec = be64toh(*reinterpret_cast<unsigned long*>(buf+at));
  at += sizeof(unsigned long);
  usec = be64toh(*reinterpret_cast<unsigned long*>(buf+at));
  at += sizeof(unsigned long);
  value = *reinterpret_cast<double*>(buf+at);

  return CPacket{id_tag, sec, usec, value};
}

void CPacket::toBytes(char *bytes)
{
  strncpy(bytes, hdr.data(), 4);

  size_t at = 4;
  *reinterpret_cast<unsigned long*>(bytes+at) = htobe64(id_tag);
  at += sizeof(unsigned long);
  *reinterpret_cast<unsigned long*>(bytes+at) = htobe64(sec);
  at += sizeof(unsigned long);
  *reinterpret_cast<unsigned long*>(bytes+at) = htobe64(usec);
  at += sizeof(unsigned long);
  *reinterpret_cast<double*>(bytes+at) = value;

}

ostream& cys::operator<<(ostream &o, const CPacket &c)
{
  o << "{" 
    << c.id_tag << ", " 
    << c.sec << ", " 
    << c.usec << ", " 
    << c.value 
    << "}";
  return o;
}

// SensorManager +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
SensorManager::SensorManager(Sim &sim)
  : sim{&sim}
{
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if(sockfd < 0)
  {
    cerr << "[" << t << "]" << "[SensorManager] socket() failure: " << sockfd << endl;
    exit(1);
  }
}

void SensorManager::add(Sensor &s)
{
  s.nxt = t + (1.0/s.rate);
  Q.push(&s);
}

void SensorManager::step(double t)
{
  this->t = t; 

  cout << "[" << sim->t << "]" << "[sm] t=" << t << endl;
  cout << "[" << sim->t << "]" << "[sm] q=" << Q.top()->nxt << endl;

  
  while(Q.top()->nxt <= (t-thresh))
  {
    Sensor *s = Q.top();
    tx(*s);
    s->nxt = t + (1.0/s->rate);
    Q.pop();
    Q.push(s);
  }
}

void SensorManager::tx(Sensor &s)
{
  realtype v = s.v;//sim->y[s.idx];
  unsigned long sec = std::floor(t);
  unsigned long usec = std::floor((t - sec)*1e6);

  CPacket cpk{s.id_tag, sec, usec, v};

  constexpr size_t sz{sizeof(CPacket)};
  char buf[sz];
  cpk.toBytes(buf);
  auto *addr = reinterpret_cast<sockaddr*>(&s.out_addr);
  sendto(sockfd, buf, sz, 0, addr, sizeof(s.out_addr));

  char sadr[128];
  inet_ntop(AF_INET, &(s.out_addr.sin_port), sadr, 128);

  cout << "[" << sim->t  << "]"
    << sadr << ":" << s.out_addr.sin_port << "  " <<  v
    << endl;
}

bool SensorCmp::operator()(const Sensor *a, const Sensor *b)
{
  return a->nxt > b->nxt;
}

SingleDirect::SingleDirect(Sim &sim)
  : sim{&sim}
{  }

int SingleDirect::run()
{
  return 0;
}
