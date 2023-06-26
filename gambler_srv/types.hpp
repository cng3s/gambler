#ifndef __TYPES_HPP__
#define __TYPES_HPP__

#define invalid_userid userid_t(0)
#define invalid_roomid roomid_t(0)
#define invalid_user_state user_state_t(0)
#define uni_id_t uint32_t
#define uni_id_t_max UINT_MAX

enum class userid_t : uni_id_t;
enum class roomid_t : uni_id_t;

enum class game_t : uint8_t {
	unknown = 0,
	taxes_poker,
	avalon,
};

enum class user_state_t : uint8_t {
	unknown = 0,
	idle,
	ready_for_game,
	playing_the_game,
};

#endif
