#include <stdexcept>
#include <fstream>

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

Object::Object(unsigned long n): idx{Sim::get().nextResidIndex(n)} 
{
  Sim::get().objects.push_back(this);
}

void Sim::step()
{
  sensorManager.step(t);
  for(Object *o : objects) o->Resid();
}

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
  int err = bind(sockfd, saddr, sizeof(addr));
  if(err < 0)
  {
    cerr << "[ActuationServer] bind() failed " << err << endl;
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
    cout << "[ActuationServer] !!!" << endl;
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
  //if there are no sensors the line below results in sadness
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
  inet_ntop(AF_INET, &(s.out_addr.sin_addr), sadr, 128);

  cout << "################>>>[" << sim->t  << "] "
    << sadr << ":" << ntohs(s.out_addr.sin_port) << "  " <<  v
    << endl;
}

bool SensorCmp::operator()(const Sensor *a, const Sensor *b)
{
  return a->nxt > b->nxt;
}

// SingleDirect ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

SingleDirect::SingleDirect(Sim &sim)
  : sim{&sim}
{  }

int SingleDirect::run(realtype begin, realtype end, realtype step)
{
  cout << "running simulation" << endl
       << "N=" << sim->yx <<endl
       << endl;
  initState();
  initIda(begin);

  realtype tret{0};
 
  size_t result_size{static_cast<size_t>(std::ceil((end-begin)/step))};

  realtype *results = new realtype[result_size*sim->yx];
  size_t period = step / 1.0e-6;

  dumpState(cout);

  size_t i=0;
  for(realtype t=begin+step; t<end; t += step, ++i)
  {
    sim->t = t;
    auto t0 = high_resolution_clock::now();
    int retval = IDASolve(mem, t, &tret, yv, dyv, IDA_NORMAL);
    if(retval != IDA_SUCCESS)
    {
      cerr << "[" << t << "]" << "IDASolve failed: " << retval << endl;
      exit(1);
    }
    //sim->t = tret;
    sim->step();
    memcpy(&results[i*sim->yx], sim->y, sizeof(realtype)*sim->yx);
    auto t1 = high_resolution_clock::now();
    size_t elapsed = duration_cast<microseconds>(t1 - t0).count();
    cout << "~~~~~~~~~~~~~~~~t=" << t << endl;

    if(elapsed < period)
      usleep(period - elapsed);
  }

  cout << "Simulation completed, saving results" << endl;
  std::ofstream r_out{"results.csv", std::ios_base::out};
  for(int j=0; j<i; ++j)
  {
    for(int k=0; k<sim->yx; ++k)
    {
      r_out << results[i*sim->yx + k] << ",";
    }
    r_out << endl;
  }

  return 0;
}

void SingleDirect::dumpState(ostream &out)
{
  out << "y" << endl;
  for(int k=0; k<sim->yx; ++k)
  {
    out << sim->y[k] << ",";
  }
  out << endl;
  
  out << "dy" << endl;
  for(int k=0; k<sim->yx; ++k)
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

  for(int i=0; i<sim->yx; ++i)
  {
    sim->y[i] = sim->dy[i] = 0;
  }

  cout << "[SingleDirect] init state ok" << endl;
}

int F_Single(realtype t, N_Vector y, N_Vector dy, N_Vector r, void* /*udata*/)
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
    cerr << "IDACreate() failed" << endl;
    exit(1);
  }

  int retval = IDAInit(mem, F_Single, begin, yv, dyv);
  if(retval != IDA_SUCCESS)
  {
    cerr << "IDAInit() failed" << endl;
    exit(1);
  }

  retval = IDASetUserData(mem, this);
  if(retval != IDA_SUCCESS)
  {
    cerr << "IDASetUserData() failed" << endl;
    exit(1);
  }

  retval = IDASStolerances(mem, rtl, atl);
  if(retval != IDA_SUCCESS)
  {
    cerr << "IDASStolerances() failed" << endl;
    exit(1);
  }

  if(sim->rx != sim->yx)
  {
    cerr << "Number of equations must equal number of variables" << endl;
    cerr << "NEQ=" << sim->rx << "NV=" << sim->yx << endl;
    exit(1);
  }

  retval = IDADense(mem, sim->rx);
  if(retval != IDA_SUCCESS)
  {
    cerr << "IDADesnse() failed" << endl;
    exit(1);
  }

  cout << "IDAInit ok" << endl;
}

