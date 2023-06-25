#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/asio/strand.hpp>
#include <boost/json.hpp>
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "msg.hpp"
#include "err.hpp"

namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace json = boost::json;
using tcp = boost::asio::ip::tcp;

class session : public std::enable_shared_from_this<session>
{
	websocket::stream<beast::tcp_stream> ws_;
	msg msg_;

public:
	explicit session(tcp::socket&& socket)
		: ws_(std::move(socket)) 
	{}

	// Get on the correct executor
	void run()
	{
		// We need to be executing within a strand to perform async operations
		// on the I/O objects in this session. Although not strictly necessary
		// for single-threaded contexts, this example code is written to be
		// thread-safe by default.
		net::dispatch(
			ws_.get_executor(),
			beast::bind_front_handler(&session::on_run, shared_from_this()));
	}

	// Start the asynchronous operation
	void on_run()
	{
		// Set suggested timeout settings for the websocket
		ws_.set_option(websocket::stream_base::timeout::suggested(beast::role_type::server));

		// Set a decorator to change the Server of the handshake
		ws_.set_option(websocket::stream_base::decorator(
			[](websocket::response_type& res) {
				res.set(http::field::server,
					std::string(BOOST_BEAST_VERSION_STRING) +
					" websocket-server-async");
			}));

		// Accept the websocket handshake
		ws_.async_accept(beast::bind_front_handler(&session::on_accept, shared_from_this()));
	}

	void do_read()
	{
		ws_.async_read(
			msg_.buf,
			beast::bind_front_handler(&session::on_read, shared_from_this()));
	}

	void on_read(beast::error_code ec, std::size_t bytes_transferred)
	{
		boost::ignore_unused(bytes_transferred);

		// This indicates that the session was closed
		if(ec == websocket::error::closed)
			return;

		if(ec)
			return fail(ec, "read");

		// Echo the message
		ws_.text(ws_.got_text());
		printf("recevie %d bytes message: %s\n", msg_.buf.size(), msg_.buf.data());
		
		
		msg_.do_parse();

		const char *str = "here is server! good good flat buffer";
		json::object js {
			{"cmd", 1},
			{"msg", str}
		};
		std::string js_str = json::serialize(js);

		msg_.buf.clear();
		auto buf = msg_.buf.prepare(js_str.size());
		char *const p = static_cast<char *const>(buf.data());
		std::cout << "js_str:" << js_str << std::endl;
		memcpy(p, js_str.c_str(), js_str.size()); 
		msg_.buf.commit(js_str.size());
		printf("js_str: %s, buffer_.size(): %d, js_str().size(): %d\n", 
			js_str.c_str(), msg_.buf.size() , js_str.size());

		ws_.async_write(
			msg_.buf.data(),
			beast::bind_front_handler(&session::on_write, shared_from_this()));
	}

	void on_write(beast::error_code ec, std::size_t bytes_transferred)
	{
		boost::ignore_unused(bytes_transferred);

		if(ec)
			return fail(ec, "write");

		// Clear the buffer
		msg_.buf.consume(msg_.buf.size());

		// Do another read
		do_read();
	}

	void on_accept(beast::error_code ec)
	{
		if(ec)
			return fail(ec, "accept");

		do_read();
	}
};

// Accepts incoming connections and launches the sessions
class listener : public std::enable_shared_from_this<listener>
{
	net::io_context& ioc_;
	tcp::acceptor acceptor_;

public:
	listener(net::io_context& ioc, tcp::endpoint endpoint)
		: ioc_(ioc), acceptor_(ioc)
	{
		beast::error_code ec;

		acceptor_.open(endpoint.protocol(), ec);
		if(ec) {
			fail(ec, "open");
			return;
		}

		acceptor_.set_option(net::socket_base::reuse_address(true), ec);
		if(ec) {
			fail(ec, "set_option");
			return;
		}

		acceptor_.bind(endpoint, ec);
		if(ec) {
			fail(ec, "bind");
			return;
		}

		acceptor_.listen(net::socket_base::max_listen_connections, ec);
		if(ec) {
			fail(ec, "listen");
			return;
		}
	}

	void run()
	{
		do_accept();
	}

private:
	void do_accept()
	{
		// The new connection gets its own strand
		acceptor_.async_accept(
			net::make_strand(ioc_),
			beast::bind_front_handler(&listener::on_accept, shared_from_this()));
	}

	void on_accept(beast::error_code ec, tcp::socket socket)
	{
		if(ec)
			fail(ec, "accept");
		else
			std::make_shared<session>(std::move(socket))->run();

		do_accept();
	}

};

int main(int argc, char* argv[])
{
	auto const address = net::ip::make_address("192.168.85.130");
	auto const port = static_cast<unsigned short>(3000);
	auto const threads = std::max<int>(1, 1);

	// The io_context is required for all I/O
	net::io_context ioc{threads};

	// Create and launch a listening port
	std::make_shared<listener>(ioc, tcp::endpoint{address, port})->run();

	// Run the I/O service on the requested number of threads
	std::vector<std::thread> v;
	v.reserve(threads - 1);
	for(auto i = threads - 1; i > 0; --i)
		v.emplace_back([&ioc] { ioc.run(); });
	ioc.run();

	return EXIT_SUCCESS;
}
