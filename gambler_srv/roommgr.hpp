#ifndef __ROOMMGR_HPP__
#define __ROOMMGR_HPP__

#include <vector>
#include <map>
#include <set>
#include <memory>
#include <mutex>

#include "types.hpp"


class room {
private:
	std::mutex mtx_;
	game_t game_type_;
	roomid_t roomid_;
	std::set<userid_t> users_;

public:
	explicit room(game_t game_type, userid_t creator);

	void set_roomid(roomid_t roomid);

	bool do_join(userid_t userid);
};

class roommgr {
private:
	std::map<roomid_t, std::unique_ptr<room>> rooms_;
	std::mutex mtx_;

public:
	bool create_room(game_t type, userid_t creator);

	bool join_room(roomid_t roomid, userid_t userid);

private:
	bool is_roomid_available_locked(roomid_t roomid);

	bool is_roomid_valid_locked(roomid_t roomid);

	bool is_roomid_exist_locked(roomid_t roomid);
};

#endif
