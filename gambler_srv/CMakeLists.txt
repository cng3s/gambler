cmake_minimum_required(VERSION 3.8...3.20)
project (gambler_srv)

add_executable (gambler_srv
	msg.hpp
	err.hpp
	srv.cpp
)

target_link_libraries (gambler_srv boost_json)

set_property(TARGET gambler_srv PROPERTY FOLDER "gambler_srv")
