#include "msg.hpp"
#include "types.hpp"
#include "usermgr.hpp"
#include "common.hpp"


msg::msg(boost::beast::flat_buffer &&fb)
	: buf(std::move(fb))
{}


void msg::handle()
{
	do_parse();
	make_response();
}


void msg::do_parse()
{
	json::stream_parser sp;
	json::error_code ec;
	char *const buf_data = static_cast<char *const>(buf.data().data());

	BOOST_LOG_TRIVIAL(info) << buf.size() << " bytes message:" << buf_data;

	sp.write(buf_data, buf.size(), ec);
	if (ec) {
		BOOST_LOG_TRIVIAL(error) << ec.message();
		return;
	}

	sp.finish(ec);
	if (ec) {
		BOOST_LOG_TRIVIAL(error) << ec.message();
		return;
	}

	json::value jv = sp.release();
	auto const &obj = jv.as_object();
	cmd_t cmd = static_cast<cmd_t>(obj.at("cmd").as_int64());
	do_dispatch(cmd, obj);
}


void msg::do_dispatch(cmd_t cmd, const json::object &obj)
{
	try {
		switch (cmd) {
			case cmd_t::login:
				return parse_req_login(obj);
			case cmd_t::create_room:
				return parse_req_create_room(obj);
			default:
				BOOST_LOG_TRIVIAL(error) << "unknow command";
		}
	} catch (const std::out_of_range &ex) {
		BOOST_LOG_TRIVIAL(error) << ex.what();
	}
}

void msg::parse_req_login(const json::object &obj) 
{
	auto const &data_obj = obj.at("data").as_object();
	userid_t user_id = static_cast<userid_t>(data_obj.at("uid").as_int64());
	g_usermgr->login(user_id);
}

void msg::parse_req_create_room(const json::object &obj)
{
}

void msg::make_response()
{
	const char *str = "here is server! good good flat buffer";
	json::object js {
		{"cmd", 1},
		{"msg", str}
	};
	std::string js_str = json::serialize(js);

	buf.clear();
	auto newbuf = buf.prepare(js_str.size());
	char *const p = static_cast<char *const>(newbuf.data());
	memcpy(p, js_str.c_str(), js_str.size()); 
	buf.commit(js_str.size());
	BOOST_LOG_TRIVIAL(info) << "js_str: " << js_str.c_str() << " buffer_.size(): " 
		<< buf.size() << ", js_str().size(): " << js_str.size(); 
}
