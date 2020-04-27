#ifndef ADDRESS_H
#define ADDRESS_H

#include <ostream>

namespace iplib {
namespace net {

class Address {
  public:
	Address() = default;
	~Address() = default;
	Address(const Address &rhs) = default;
	Address &operator=(const Address &rhs) = default;

	/**
	 *	ctor.
	 *
	 *	Takes an IPv4 address of the form:
	 *		a.b.c.d:port
	 */
	Address(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short port);

	/**
	 *	ctor.
	 *
	 *	Takes in IPv4 address of the form:
	 *		address:port
	 *
	 * 	NO ERROR CHECKING.
	 */
	Address(unsigned int address, unsigned short port);

	unsigned int GetAddress() const;
	unsigned char GetA() const;
	unsigned char GetB() const;
	unsigned char GetC() const;
	unsigned char GetD() const;
	unsigned short GetPort() const;

  private:
	unsigned int _address;
	unsigned short _port;
};

std::ostream &operator<<(std::ostream &out, const Address &rhs);

} // net
} // iplib

#endif // ADDRESS_H