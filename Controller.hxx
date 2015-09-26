#ifndef CYS_CONTROLLER
#define CYS_CONTROLLER

#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <set>
#include <sstream>
#include <memory>
#include <fstream>
#include <mutex>
#include <array>
#include <atomic>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>

#include "Cypress.hxx"

#if defined(__linux__)
#include <endian.h>
#elif defined(__APPLE__)
#include <libkern/OSByteOrder.h>
#define htobe64(x) OSSwapHostToBigInt64(x)
#define be64toh(x) OSSwapBigToHostInt64(x)
#endif

namespace cys {


struct CVal
{
  unsigned long sec, usec;
  double v;

  CVal() = default;
  CVal(unsigned long sec, unsigned long usec, double v) 
    : sec{sec}, usec{usec}, v{v} {};
};

struct ControlBuffer
{
  std::vector<std::vector<CVal>> buf;
  size_t size() { return buf.size(); }
  void clear() { for(auto &b: buf) b.clear(); }
};

//Control Coordinate
struct CCoord
{
  unsigned long who{}, what{};
  CCoord() = default;
  CCoord(unsigned long who, unsigned long what)
    : who{who}, what{what}
  {}
};

using FrameVarResolver = std::function<double(const std::vector<CVal>&, double last)>;

double UseLatestArrival(const std::vector<CVal> &, double last);

struct Controller
{
  size_t period{4}; //4 millisecond default period
  std::string name;
  size_t input_size{0};

  //maps id_tag to a local input index
  std::unordered_map<unsigned long, size_t> imap;

  std::vector<double> input_frame;

  //maps a local variable index to a resolver
  std::vector<FrameVarResolver> resolvers;
  //std::vector<InputSource> sources{};
  
  ControlBuffer a_, b_;
  ControlBuffer *a{&a_}, *b{&b_};

  std::mutex io_mtx;

  void run();

  CPacket cpk;
  //std::hash<std::string> hsh{};
  //void setTarget(std::string what);
  //size_t setInput(std::string what);
  void setTarget(unsigned long id_tag);
  size_t setInput(unsigned long id_tag);
  void setDestination(std::string addr);

  void rx(), tx(double v);
  void listen();
  void send(CPacket pkt);
  void io();
  void kernel();
  void swapBuffers();
  void computeFrame();

  std::ofstream k_lg, io_lg;

  //Comms stuff ---------------------------------------------------------------
  unsigned short port{7474};
  int sockfd;
  struct sockaddr_in servaddr, cliaddr, tgtaddr;
  std::atomic<bool> listener_bound{false};

  Controller(std::string name) 
    : name{name}, 
      k_lg{name+"k.log", std::ios_base::out | std::ios_base::app},
      io_lg{name+"io.log", std::ios_base::out | std::ios_base::app} 
  {}
  
  virtual void compute() = 0;
};

}

#endif
