cmake_minimum_required(VERSION 3.8...3.20)
project (gambler_srv)

add_executable (gambler_srv
	srv.cpp
	common.cpp
	msg.cpp
	roommgr.cpp
)

add_definitions ("-DBOOST_LOG_DYN_LINK")

target_link_libraries (gambler_srv boost_json boost_log_setup boost_log boost_thread)

set_property(TARGET gambler_srv PROPERTY FOLDER "gambler_srv")
