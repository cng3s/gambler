#ifndef __COMMON_HPP__
#define __COMMON_HPP__

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

#include "usermgr.hpp"
#include "roommgr.hpp"

usermgr *g_usermgr;
roommgr *g_roommgr;

namespace logging = boost::log;
void log_init()
{
	logging::core::get()->set_filter(logging::trivial::severity >= logging::trivial::info);
}

void global_init()
{
	log_init();
	g_usermgr = new usermgr();
	g_roommgr = new roommgr();
}

void global_deinit()
{
	delete g_usermgr;
	delete g_roommgr;
}

#endif
