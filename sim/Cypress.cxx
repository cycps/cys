#include <stdexcept>
#include <fstream>
#include <yaml-cpp/yaml.h>

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
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::microseconds;
using std::to_string;

string Sim::header()
{
  string s;
  for(string &l: labels)
  {
    s += l + ",";
  }
  for(size_t i=0; i<labels.size()-1; ++i)
  {
    s += labels[i] + "',";
  }
  s += labels[labels.size()-1] + "'";
  return s;
}

string Sim::datastring()
{
  string s;
  for(unsigned long i=0; i<yx; ++i)
  {
    s += to_string(y[i]) + ",";
  }
  for(unsigned long i=0; i<yx-1; ++i)
  {
    s += to_string(dy[i]) + ",";
  }
  s += to_string(dy[yx-1]);
  return s;
}

Object::Object(ulong n, string name)
  : n{n}, idx{Sim::get().nextResidIndex(n)}, name{name}
{
  Sim::get().objects.push_back(this);
}

void Object::label(Var &v, string s)
{
  Sim::get().labels[v.idx] = name+"."+s;
}
  
void Object::label(std::vector<std::pair<Var, std::string>> labels)
{
  for(auto &p : labels) label(p.first, p.second);
}

void Sim::step()
{
  //lock_guard<mutex> lk{mtx};
  sensorManager.step(t);
  for(Object *o : objects) o->resid();
}

ActuationServer::ActuationServer() 
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
    lg << log("socket() failed") << endl;
    exit(1);
  }
  bzero(&addr, sizeof(addr)); 
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port);

  auto *saddr = reinterpret_cast<const struct sockaddr*>(&addr);
  int err = bind(sockfd, saddr, sizeof(addr));
  if(err < 0)
  {
    lg << log("bind() failed "+to_string(err)) << endl;
    exit(1);
  }

  lg << log("bound to port "+to_string(port)) << endl;
}

void ActuationServer::listen()
{
  socklen_t len;
  constexpr size_t sz{sizeof(CPacket)};
  char msg[sz];
  sockaddr_in client_addr;
  auto *caddr = reinterpret_cast<sockaddr*>(&client_addr);

  lg << log("listening") << endl;

  for(;;)
  {
    int err = recvfrom(sockfd, msg, sz, 0, caddr, &len);
    if(err < 0)
    {
      lg << log("recvfrom() failed: "+to_string(err)) << endl;
      exit(1);
    }

    CPacket pkt;
    try { pkt = CPacket::fromBytes(msg); }
    catch(runtime_error &ex)
    {
      lg << log("packet read error: " + string(ex.what())) << endl;
      continue;
    }

    lock_guard<mutex> lk(rx_mtx);
    try { actuators.at(pkt.id_tag)->actuate(pkt.value); }
    catch(std::out_of_range)
    {
      lg << log("unknown id_tag "+to_string(pkt.id_tag)) << endl;
      continue;
    }
  }
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

  //cout << "[" << sim->t << "]" << "[sm] t=" << t << endl;
  //if there are no sensors the line below results in sadness
  //cout << "[" << sim->t << "]" << "[sm] q=" << Q.top()->nxt << endl;

  
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
  inet_ntop(AF_INET, &(s.out_addr.sin_addr), sadr, 128);

  /*
  cout << "[" << sim->t  << "] "
    << sadr << ":" << ntohs(s.out_addr.sin_port) << "  " <<  v
    << endl;
    */
}

bool SensorCmp::operator()(const Sensor *a, const Sensor *b)
{
  return a->nxt > b->nxt;
}

// SingleDirect ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

SingleDirect::SingleDirect(Sim &sim)
  : sim{&sim}
{  

}

int SingleDirect::run(realtype begin, realtype end, realtype step)
{
  lg << log("running simulation") << endl;
  lg << log("N=" + to_string(sim->yx)) << endl;
  initState();
  sim->initObjects();
  initIda(begin);
  
  bool ok = sim->conditionsCheck();
  if(!ok)
  {
    lg << log("conditions check failed, "
              "see the simulator log file for details") << endl;
    exit(1);
  }

  realtype tret{0};
 
  size_t result_size{static_cast<size_t>(std::ceil((end-begin)/step))};

  realtype *results = new realtype[result_size*(sim->yx*2+1)];
  size_t period = std::ceil(step / 1.0e-6);
  //size_t period = step;

  lg << log("begin crunch") << endl;

  size_t i=0;
  for(realtype t=begin+step; t<end; t += step, ++i)
  {
    sim->t = t;
    auto t0 = high_resolution_clock::now();
    int retval = IDASolve(mem, t, &tret, yv, dyv, IDA_NORMAL);
    if(retval != IDA_SUCCESS)
    {
      lg << log("["+to_string(t)+"] IDASolve failed: " + to_string(retval)) 
         << endl;
      
      lg << log("running conditions check to help isolate") << endl;
      sim->conditionsCheck();
      results[i*(sim->yx*2+1)] = tret;
      memcpy(&results[i*(sim->yx*2+1)+1], sim->y, sizeof(realtype)*sim->yx);
      memcpy(&results[i*(sim->yx*2+1)+1+sim->yx], sim->dy, sizeof(realtype)*sim->yx);
      writeResults(results, i+1);
      delete[] results;

      exit(1);
    }
    sim->t = t;
    sim->step();
    
    sim->slk.unlock();
    results[i*(sim->yx*2+1)] = t;
    memcpy(&results[i*(sim->yx*2+1)+1], sim->y, sizeof(realtype)*sim->yx);
    memcpy(&results[i*(sim->yx*2+1)+1+sim->yx], sim->dy, sizeof(realtype)*sim->yx);
    auto t1 = high_resolution_clock::now();
    size_t elapsed = duration_cast<microseconds>(t1 - t0).count();


    if(elapsed < period)
    {
      usleep(period - elapsed);
    }
    sim->slk.lock();
  }

  std::cout << "simulation finished writing results" << std::endl;
    
  writeResults(results, i);

  delete[] results;


  return 0;
}

void SingleDirect::writeResults(realtype *results, size_t n)
{
  std::ofstream r_out{"results.csv", std::ios_base::out};
  lg << log("Simulation completed, saving results") << endl;
  r_out << "t," << sim->header() << endl;
  //r_out << sim->datastring() << endl;
  for(unsigned long j=0; j<n; ++j)
  {
    for(unsigned long k=0; k<(sim->yx*2+1)-1; ++k)
    {
      r_out << results[j*(sim->yx*2+1) + k] << ",";
    }
    r_out << results[j*(sim->yx*2+1)-1];
    r_out << endl;
  }
}

void SingleDirect::dumpState(ostream &out)
{
  out << "y" << endl;
  for(unsigned long k=0; k<sim->yx; ++k)
  {
    out << sim->y[k] << ",";
  }
  out << endl;
  
  out << "dy" << endl;
  for(unsigned long k=0; k<sim->yx; ++k)
  {
    out << sim->dy[k] << ",";
  }
  out << endl;
}

void SingleDirect::initState()
{
  yv = N_VNew_Serial(sim->yx); 
  dyv = N_VNew_Serial(sim->yx);
  rv = N_VNew_Serial(sim->rx);

  sim->y = NV_DATA_S(yv); 
  sim->dy = NV_DATA_S(dyv);
  sim->r = NV_DATA_S(rv);

  for(unsigned long i=0; i<sim->yx; ++i)
  {
    sim->y[i] = sim->dy[i] = 0;
  }

  lg << log("init state ok") << endl;
}

int F_Single(realtype /*t*/, N_Vector y, N_Vector dy, N_Vector r, void* /*udata*/)
{
  Sim::get().y = NV_DATA_S(y);
  Sim::get().dy = NV_DATA_S(dy);
  Sim::get().r = NV_DATA_S(r);
  Sim::get().step();

  return 0;
}

void SingleDirect::initIda(realtype begin)
{
  mem = IDACreate();
  if(mem == nullptr)
  {
    lg << log("IDACreate() failed") << endl;
    exit(1);
  }

  int retval = IDAInit(mem, F_Single, begin, yv, dyv);
  if(retval != IDA_SUCCESS)
  {
    lg << log("IDAInit() failed") << endl;
    exit(1);
  }

  retval = IDASetUserData(mem, this);
  if(retval != IDA_SUCCESS)
  {
    lg << log("IDASetUserData() failed") << endl;
    exit(1);
  }

  retval = IDASStolerances(mem, rtl, atl);
  if(retval != IDA_SUCCESS)
  {
    lg << log("IDASStolerances() failed") << endl;
    exit(1);
  }

  if(sim->rx != sim->yx)
  {
    lg << log("Number of equations must equal number of variables") << endl;
    lg << log("NEQ="+to_string(sim->rx)+" NV="+to_string(sim->yx)) << endl;
    exit(1);
  }

  retval = IDADense(mem, sim->rx);
  if(retval != IDA_SUCCESS)
  {
    lg << log("IDADesnse() failed") << endl;
    exit(1);
  }

  //IDACalcIC(mem, IDA_Y_INIT, 1e-3);
  retval = IDASetMaxNumSteps(mem, 5000);
  if(retval != IDA_SUCCESS)
  {
    lg << log("IDASetMaxNumSteps() failed") << endl;
    exit(1);
  }

  retval = IDASetMaxStep(mem, 1e-6);
  if(retval != IDA_SUCCESS)
  {
    lg << log("IDASetMaxStep() failed") << endl;
    exit(1);
  }

  lg << log("IDAInit ok") << endl;
}

void Sim::initObjects()
{
  for(Object *o : objects) o->init();
}

bool Sim::conditionsCheck()
{
  lg << log("Checking conditions") << endl;
  bool ok{true};
  for(Object *o : objects)
  {
    ok = ok && o->conditionsCheck();
  }
  if(ok) lg << log("Conditions look good!") << endl;
  else   lg << log("Conditions check failed") << endl;

  return ok;
}

bool Object::conditionsCheck()
{
  resid();
  bool ok{true};
  for(ulong i=0; i<n; ++i)
  {
    realtype ri = r(i);
    if(std::abs(ri) > 1e-9)
    {
      Sim::get().lg << name << ": r(" << i << ") = " << ri << endl;
      ok = false;
    }
  }
  return ok;
}
