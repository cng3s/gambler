#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <ctime>
#include <random>
#include <climits>

#include "types.hpp"

uni_id_t gen_uni_id()
{
	std::default_random_engine e{ std::random_device{}() };
	std::uniform_int_distribution<uni_id_t> dis(1, UINT_MAX);
	return dis(e);
}

roomid_t gen_roomid()
{
	return static_cast<roomid_t>(gen_uni_id());
}

#endif
