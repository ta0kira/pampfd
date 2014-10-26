#include "track-host.hpp"

#ifdef VERBOSE
#include <stdlib.h>
#endif


	void track_host::reset(int limit, time_t window)
	{
	time_window = window;
	violation_limit = limit;
	this->clear();
	}


	void track_host::clear()
	{
	while (records.size()) records.pop();
	hosts.clear();
	}


	bool track_host::add_violation(const address_t &address, const label_t &user, const label_t &service, time_t time)
	{
	if (!time_window || !violation_limit) return false;
	if (!address.size()) return false;

	while (records.size() && (signed) records.front().record_time < time - time_window)
	 {
	host_counts::iterator current = hosts.find(records.front().record_address);
	if (current != hosts.end() && --(current->second) <= 0) hosts.erase(current);
	records.pop();
	 }

	records.push(host_record(address, time));

#ifdef VERBOSE
	char logger_buffer[BUFFER_SIZE] = { 0x00 };
	snprintf(logger_buffer, sizeof logger_buffer, "/usr/bin/logger -p security.notice \"bad authentication for %s from %s: %i\"",
	  user.c_str(), address.c_str(), hosts[address] + 1);
	system(logger_buffer);
#endif

	return (++hosts[address] >= violation_limit);
	}
