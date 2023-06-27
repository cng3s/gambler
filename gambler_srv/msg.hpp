#ifndef __MSG_HPP__
#define __MSG_HPP__

#include <boost/beast/core.hpp>
#include <boost/json.hpp>

#include "types.hpp"

class msg
{
public:
	boost::beast::flat_buffer buf;

	explicit msg() = default;

	explicit msg(boost::beast::flat_buffer &&fb);

	void handle();
	
	void do_parse();
	
	void do_dispatch(cmd_t cmd, const boost::json::object &obj);

	void parse_req_login(const boost::json::object &obj);

	void parse_req_create_room(const boost::json::object &obj);

	void make_response();
};

#endif
