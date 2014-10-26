#ifndef track_host_hpp
#define track_host_hpp

#include "track.hpp"

#include <map>
#include <queue>


struct host_record
{
	inline host_record(const address_t &address, time_t time) :
	record_address(address), record_time(time) {}

	address_t record_address;
	time_t    record_time;
};


typedef std::queue <host_record>  host_record_queue;
typedef std::map <address_t, int> host_counts;


class track_host : public track
{
public:
	track_host() : time_window(0), violation_limit(0) {}

	inline track_host(int limit, time_t window) { this->reset(limit, window); }

	void reset(int, time_t);
	void clear();

	bool add_violation(const address_t&, const label_t&, const label_t&, time_t);

private:
	time_t time_window;
	int    violation_limit;

	host_record_queue records;
	host_counts       hosts;
};

#endif //track_host_hpp
