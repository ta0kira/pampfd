#ifndef track_user_hpp
#define track_user_hpp

#include "track.hpp"

#include <map>
#include <queue>


struct user_record
{
	inline user_record(const address_t &address, const label_t &user, time_t time) :
	record_address(address), record_user(user), record_time(time) {}

	address_t record_address;
	label_t   record_user;
	time_t    record_time;
};


typedef std::queue <user_record>          user_record_queue;
typedef std::map <label_t, int>           user_counts;
typedef std::map <address_t, user_counts> host_user_counts;


class track_users : public track
{
public:
	track_users() : time_window(0), violation_limit(0) {}

	inline track_users(int limit, time_t window) { this->reset(limit, window); }

	void reset(int, time_t);
	void clear();

	bool add_violation(const address_t&, const label_t&, const label_t&, time_t);

private:
	time_t time_window;
	int    violation_limit;

	user_record_queue records;
	host_user_counts  hosts_users;
};

#endif //track_users_hpp
