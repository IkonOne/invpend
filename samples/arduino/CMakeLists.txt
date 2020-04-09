cmake_minimum_required(VERSION 3.13)

MESSAGE_CONFIGURING()

set(ARDUINO_FQBN arduino:avr:uno)

option(IPLIB_SAMPLE_hello_serial "" True)
option(IPLIB_SAMPLE_quad_encoders "" True)

if(IPLIB_SAMPLE_hello_serial)
	add_subdirectory("hello_serial")
endif()

if(IPLIB_SAMPLE_quad_encoders)
	add_subdirectory("quad_encoders")
endif()

