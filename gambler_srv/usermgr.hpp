#ifndef __USERMGR_HPP__
#define __USERMGR_HPP__

#include <set>
#include <mutex>
#include <cstdint>
#include <memory>

#include "types.hpp"
#include "roommgr.hpp"


class user {
private:
	userid_t uid_;
	user_state_t state_;
	roomid_t roomid_;

public:
	explicit user(userid_t uid)
		: uid_(uid), state_(invalid_user_state), roomid_(invalid_roomid)
	{}

	userid_t get_uid() const 
	{ return uid_; }

	void set_state(user_state_t state) 
	{ state_ = state; }

	user_state_t get_state() const
	{ return state_; }
};

class usermgr {
private:
	std::mutex mtx_;
	std::map<userid_t, std::unique_ptr<user>> online_;

public:
	bool is_online(userid_t uid) 
	{
		std::lock_guard<std::mutex> lg(mtx_);
		return online_.find(uid) != online_.end();
	}

	void login(userid_t uid)
	{
		std::lock_guard<std::mutex> lg(mtx_);
		online_.emplace(uid, std::make_unique<user>(uid));
	}

	void logout(userid_t uid)
	{
		std::lock_guard<std::mutex> lg(mtx_);
		auto it = online_.find(uid);
		if (it != online_.end())
			online_.erase(it);
	}

	bool is_user_online(userid_t userid)
	{
		std::lock_guard lg(mtx_);
		return online_.find(userid) != online_.end();
	}
};

#endif
