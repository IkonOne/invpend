#include <iostream>

#include "Address.h"

using namespace std;

namespace iplib {
namespace net {

Address::Address(unsigned int address, unsigned short port) : _address(address), _port(port)
{ }

Address::Address(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short port)
	: Address((a << 24) | (b << 16) | (c << 8) | (d << 0), port)
{ }

unsigned int Address::GetAddress() const { return _address; }
unsigned char Address::GetA() const { return (_address & 0xFF000000) >> 24; }
unsigned char Address::GetB() const { return (_address & 0x00FF0000) >> 16; }
unsigned char Address::GetC() const { return (_address & 0x0000FF00) >> 8; }
unsigned char Address::GetD() const { return (_address & 0x000000FF) >> 0; }
unsigned short Address::GetPort() const { return _port; }

Address Address::fromString(string addr) {
	unsigned int arr[4] = {0};
	size_t pos = 0;
	string token;

	for (int i = 0; i < 4; ++i) {
		if (i < 3) pos = addr.find(".");
		else pos = addr.find(":");
		token = addr.substr(0, pos);
		arr[i] = atoi(token.c_str());
		addr.erase(0, pos + 1);
	}

	return Address(arr[0], arr[1], arr[2], arr[3], atoi(addr.c_str()));
}

ostream &operator<<(ostream &out, const Address &rhs) {
	out << static_cast<int>(rhs.GetA()) << ".";
	out << static_cast<int>(rhs.GetB()) << ".";
	out << static_cast<int>(rhs.GetC()) << ".";
	out << static_cast<int>(rhs.GetD()) << ":";
	out << rhs.GetPort();
	return out;
}

istream &operator>>(istream &in, Address &lhs) {
	string addr;
	in >> addr;
	lhs.fromString(addr);

	return in;
}

} // net
} // iplib