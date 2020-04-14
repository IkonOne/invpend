cmake_minimum_required(VERSION 3.13)

MESSAGE_CONFIGURING()

option(IPLIB_SAMPLE_hello_serial "" True)
option(IPLIB_SAMPLE_hello_udp "" True)

if(IPLIB_SAMPLE_hello_serial)
	add_subdirectory("hello_serial")
endif()

if(IPLIB_SAMPLE_hello_udp)
	add_subdirectory("hello_udp")
endif()

if( INVPEND_ENABLE_ARDUINO )
	add_subdirectory(arduino)
endif()