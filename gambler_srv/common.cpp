#include "common.hpp"
#include "usermgr.hpp"
#include "roommgr.hpp"

usermgr *g_usermgr;
roommgr *g_roommgr;

static void log_init()
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
