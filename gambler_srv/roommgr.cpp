#include "roommgr.hpp"

#include "utils.hpp"
#include "usermgr.hpp"
#include "common.hpp"

room::room(game_t game_type, userid_t creator)
	: game_type_(game_type), roomid_(invalid_roomid), users_{ creator }
{}


void room::set_roomid(roomid_t roomid)
{ 
	std::lock_guard<std::mutex> lg(mtx_);
	roomid_ = roomid; 
}

bool room::do_join(userid_t userid)
{
	std::lock_guard<std::mutex> lg(mtx_);
	auto it = users_.find(userid);
	if (it != users_.end()) {
		BOOST_LOG_TRIVIAL(error) << static_cast<uni_id_t>(userid) 
			<< " is in the room #" << static_cast<uni_id_t>(roomid_);
		return false;
	}
	users_.emplace(userid);
	return true;
}

bool roommgr::create_room(game_t type, userid_t creator)
{
	if (!g_usermgr->is_online(creator)) {
		BOOST_LOG_TRIVIAL(error) << static_cast<uni_id_t>(creator) << " is not online";
		return false;
	}

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
	return true;
}

bool roommgr::join_room(roomid_t roomid, userid_t userid)
{
	if (!g_usermgr->is_online(userid))	{
		BOOST_LOG_TRIVIAL(error) << static_cast<uni_id_t>(userid) << " is not online";
		return false;
	}

	std::lock_guard<std::mutex> lg(mtx_);
	auto it = rooms_.find(roomid);
	if (it == rooms_.end()) {
		BOOST_LOG_TRIVIAL(error) << static_cast<uni_id_t>(roomid) << " is not exist";
		return false;
	}
	it->second->do_join(userid);
	return true;
}

bool roommgr::is_roomid_available_locked(roomid_t roomid)
{ return is_roomid_valid_locked(roomid) && !is_roomid_exist_locked(roomid); }

bool roommgr::is_roomid_valid_locked(roomid_t roomid)
{ return roomid != invalid_roomid; }

bool roommgr::is_roomid_exist_locked(roomid_t roomid)
{ return rooms_.find(roomid) != rooms_.end(); }
