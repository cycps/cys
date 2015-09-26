#ifndef CYS_PROTOCOL
#define CYS_PROTOCOL

#include <array>

namespace cys {

struct CPacket
{
  std::array<char,4> hdr{'c', 'y', 'p', 'r'};
  unsigned long id_tag, sec, usec;
  double value;

  CPacket() = default;
  CPacket(unsigned long id_tag,
          unsigned long sec, unsigned long usec,
          double value)
    : id_tag{id_tag}, sec{sec}, usec{usec}, value{value}
  {}

  static CPacket fromBytes(char *buf);
  void toBytes(char *buf);
};

std::ostream& operator<<(std::ostream &o, const CPacket &c);

}

#endif
