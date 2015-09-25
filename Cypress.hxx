#ifndef CYS_OBJECT
#define CYS_OBJECT

#if defined(__linux__)
#include <endian.h>
#elif defined(__APPLE__)
#include <libkern/OSByteOrder.h>
#define htobe64(x) OSSwapHostToBigInt64(x)
#define be64toh(x) OSSwapBigToHostInt64(x)
#endif

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sundials/sundials_types.h>
#include <nvector/nvector_serial.h>
#include <ida/ida.h>
#include <ida/ida_dense.h>

#include <thread>
#include <iostream>
#include <array>
#include <string>
#include <unordered_map>
#include <queue>
#include <chrono>

#include "Protocol.hxx"

namespace cys {

struct Sim;
struct Actuator;
struct Sensor;
struct Object;

struct ActuationServer
{
  explicit ActuationServer(Sim &sim);

  void enlist(Actuator &a);

  private:
    Sim *sim;
    std::thread *listen_thread{nullptr};
    int sockfd;
    sockaddr_in addr;
    unsigned int port{4747};
    std::mutex rx_mtx;
    std::unordered_map<unsigned int, Actuator*> actuators;

    void initComms();
    void listen();
};

struct SensorCmp
{
  bool operator()(const Sensor *a, const Sensor *b);
};


struct SensorManager
{
  Sim *sim;
  double t{0}, thresh{0.74e-6};

  explicit SensorManager(Sim &s);

  void add(Sensor&);
  void step(double t);
  void tx(Sensor&);

  //Comms stuff
  int sockfd;
  struct sockaddr_in servaddr;

  private:
    std::priority_queue<Sensor*, std::vector<Sensor*>, SensorCmp> Q;
};

struct SingleDirect
{
  Sim *sim;
  SingleDirect(Sim &sim);

  N_Vector yv, dyv, rv;

  realtype rtl = 1e-3, atl = 1e-3;

  void *mem{nullptr};

  void initState();
  void initIda(realtype begin);
  //void writeDataHeader();
  //void compute();
  void dumpState(std::ostream &out);

  int run(realtype begin, realtype end, realtype step);
};

struct Sim
{
  realtype *y{nullptr}, *dy{nullptr}, *r{nullptr}, t;
  unsigned long yx{0}, rx{0};
  
  ActuationServer actuationServer;
  SensorManager sensorManager;

  std::vector<Object*> objects;

  static Sim& get()
  {
    static Sim instance;
    return instance;
  }
  Sim(const Sim &) = delete;
  void operator =(const Sim &) = delete;

  unsigned long nextVarIndex() { return yx++; }
  unsigned long nextResidIndex(unsigned long n) 
  { 
    unsigned long _rx = rx;
    rx += n;
    return _rx;
  }

  template<class Solver=SingleDirect>
  int run(realtype begin, realtype end, realtype step)
  {
    Solver solver{*this};
    return solver.run(begin, end, step);
  }

  void step();

  private:
    Sim()
      : actuationServer{*this},
        sensorManager{*this}
    {  }

};

struct Object 
{
  explicit Object(unsigned long n);

  virtual void Resid() = 0;
  realtype& r(unsigned int offset)        { return Sim::get().r[idx + offset]; }
  realtype  cr(unsigned int offset) const { return Sim::get().r[idx + offset]; }

  private:
    unsigned long idx;
};

struct Var
{
  unsigned long idx;
  inline operator realtype&()      { return Sim::get().y[idx]; }
  inline operator realtype() const { return Sim::get().y[idx]; }
  inline realtype& d()             { return Sim::get().dy[idx]; }
  inline realtype  d()       const { return Sim::get().dy[idx]; }

  Var()
  {
    idx = Sim::get().nextVarIndex();
  }

  Var(const Var &v): idx{v.idx} {}
  void operator =(const Var &v) { idx = v.idx; }
};

inline realtype& d(Var &v) { return  v.d(); }
inline realtype d(const Var &v)  { return  v.d(); }


}

#endif
