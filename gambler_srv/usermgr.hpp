#include <set>
#include <mutex>
#include <cstdint>
#include "types.hpp"

class user {
private:
	userid_t uid;
};

class usermgr {
private:
	std::mutex mtx_;
	std::set<userid_t> online_;

public:
	bool is_online(userid_t uid) 
	{
		std::lock_guard<std::mutex> lg(mtx_);
		return online_.find(uid) != online_.end();
	}

	void login(userid_t uid)
	{
		std::lock_guard<std::mutex> lg(mtx_);
		online_.emplace(uid);
	}

	void logout(userid_t uid)
	{
		std::lock_guard<std::mutex> lg(mtx_);
		auto it = online_.find(uid);
		if (it != online_.end())
			online_.erase(it);
	}
};
