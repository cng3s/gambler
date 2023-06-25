#ifndef __MSG_HPP__
#define __MSG_HPP__

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <boost/json.hpp>
#include <memory>

#include "err.hpp"
namespace json = boost::json;
namespace beast = boost::beast;

struct msg
{
	beast::flat_buffer buf;

	enum class cmd_t {
		login,
		create_room,
		join_room,
		logout
	};

	explicit msg() = default;

	explicit msg(beast::flat_buffer &&fb)
		: buf(std::move(fb))
	{}
	
	void do_parse()
	{
		// do deserialize
		json::stream_parser sp;
		json::error_code jsonec;
		const auto &data = buf.data();
		sp.write(static_cast<char *const>(data.data()), buf.size(), jsonec);
		if (jsonec)
			return fail(jsonec, "json parse");
		sp.finish(jsonec);
		if (jsonec)
			return fail(jsonec, "json finish");

		json::value jv = sp.release();
		switch (jv.kind()) {
			case json::kind::object: {
				auto const &obj = jv.get_object();
				printf("this is a object\n");
				if (!obj.empty()) {
					for (auto it = obj.cbegin(); it != obj.cend(); ++it) 
						std::cout << "key:" << json::serialize(it->key()) 
							<< ",val:" << json::serialize(it->value()) 
							<< std::endl;
				}
				break;
			}
			case json::kind::string: {
				printf("this is a string\n");
				const json::string &jvstr = jv.get_string();
				printf("finished parse json: %d %s\n", jvstr.size(), json::serialize(jvstr));
				break;
			}
		}
	}

};

#endif
