#include "track-users.hpp"

#include <pwd.h>

#ifdef VERBOSE
#include <stdlib.h>
#endif


	void track_users::reset(int limit, time_t window)
	{
	time_window = window;
	violation_limit = limit;
	this->clear();
	}


	void track_users::clear()
	{
	while (records.size()) records.pop();
	hosts_users.clear();
	}


	bool track_users::add_violation(const address_t &address, const label_t &user, const label_t &service, time_t time)
	{
	if (!time_window || !violation_limit) return false;
	if (!address.size()) return false;

	while (records.size() && (signed) records.front().record_time < time - time_window)
	 {
	host_user_counts::iterator current_host = hosts_users.find(records.front().record_address);
	if (current_host != hosts_users.end())
	 {
	user_counts::iterator current_user = current_host->second.find(records.front().record_user);
	if (current_user != current_host->second.end() && --current_user->second <= 0)
	current_host->second.erase(current_user);
	if (!current_host->second.size()) hosts_users.erase(current_host);
	 }
	records.pop();
	 }

	struct passwd *user_entry = getpwnam(user.c_str());
	endpwent();
	if (user_entry && user_entry->pw_uid >= MIN_UID) return false;

	records.push(user_record(address, user, time));

#ifdef VERBOSE
	if (hosts_users[address].find(user) == hosts_users[address].end())
	 {
	char logger_buffer[BUFFER_SIZE] = { 0x00 };
	snprintf(logger_buffer, sizeof logger_buffer, "/usr/bin/logger -p security.notice \"bad user %s from %s: %i\"",
	  user.c_str(), address.c_str(), (signed) hosts_users[address].size() + 1);
	system(logger_buffer);
	 }
#endif

	++hosts_users[address][user];
	return ((signed) hosts_users[address].size() >= violation_limit);
	}
