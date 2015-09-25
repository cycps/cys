#include "Protocol.hxx"

#if defined(__linux__)
#include <endian.h>
#elif defined(__APPLE__)
#include <libkern/OSByteOrder.h>
#define htobe64(x) OSSwapHostToBigInt64(x)
#define be64toh(x) OSSwapBigToHostInt64(x)
#endif

#include <iostream>
#include <stdexcept>

using namespace cys;
using std::ostream;
using std::string;
using std::runtime_error;

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
