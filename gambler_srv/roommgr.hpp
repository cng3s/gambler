#ifndef __ROOMMGR_HPP__
#define __ROOMMGR_HPP__

#include <vector>
#include <map>
#include <memory>
#include <mutex>

#include "types.hpp"
#include "utils.hpp"

class room {
private:
	game_t game_type_;
	roomid_t roomid_;
	std::vector<userid_t> users_;

public:
	explicit room(game_t game_type, userid_t creator)
		: game_type_(game_type), roomid_(invalid_roomid), users_{ creator }
	{}

	void set_roomid(roomid_t roomid)
	{ roomid_ = roomid; }
};

class roommgr {
private:
	std::map<roomid_t, std::unique_ptr<room>> rooms_;
	std::mutex mtx_;

public:
	void create_room(game_t type, userid_t creator)
	{
		std::unique_ptr<room> roomptr = std::make_unique<room>(type, creator);
		roomid_t roomid = invalid_roomid;
		{
			std::lock_guard<std::mutex> lg(mtx_);
			do {
				roomid = gen_roomid();
			} while (!is_roomid_available_locked(roomid));
			roomptr->set_roomid(roomid);
			rooms_.emplace(roomid, std::move(roomptr));
		}
	}

private:
	bool is_roomid_available_locked(roomid_t roomid)
	{ return is_roomid_valid_locked(roomid) && !is_roomid_exist_locked(roomid); }

	bool is_roomid_valid_locked(roomid_t roomid)
	{ return roomid != invalid_roomid; }

	bool is_roomid_exist_locked(roomid_t roomid)
	{ return rooms_.find(roomid) != rooms_.end(); }
};

#endif
