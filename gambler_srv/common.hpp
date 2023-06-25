#ifndef __COMMON_HPP__
#define __COMMON_HPP__

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

#include "usermgr.hpp"

usermgr *g_usermgr;

namespace logging = boost::log;
void log_init()
{
	logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::info);
}

void global_init()
{
	log_init();
	g_usermgr = new usermgr();
}

void global_deinit()
{
	delete g_usermgr;
}

#endif
