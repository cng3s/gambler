#ifndef __ERR_HPP__
#define __ERR_HPP__

#include <boost/beast/core.hpp>
#include <iostream>

namespace beast = boost::beast;
void fail(beast::error_code ec, char const* what)
{
	std::cerr << what << ": " << ec.message() << "\n";
}


#endif
