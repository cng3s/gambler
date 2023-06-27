#ifndef __COMMON_HPP__
#define __COMMON_HPP__

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/json.hpp>

namespace json = boost::json;
namespace logging = boost::log;

class usermgr;
class roommgr;

extern usermgr *g_usermgr;
extern roommgr *g_roommgr;

void global_init();
void global_deinit();

#endif
