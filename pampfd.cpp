#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "ipc.hpp"
#include "whitelist.hpp"
#include "track-host.hpp"
#include "track-users.hpp"
#include "block.hpp"
#include "regex-check.hpp"


static track_host  hosts;
static track_users users;
static label_t     table;


static void register_handlers();

static bool violation_function(const label_t&, const address_t&, const label_t&, const label_t&, time_t);


int main(int argc, char *argv[])
{
#ifdef LOGNAME
	setenv("LOGNAME", LOGNAME, 1);
#endif

	if (argc < 5 || argc > 6)
	{
	fprintf(stderr, "%s [socket name] [table name] [whitelist] [host limit/time] (users limit/time)\n", argv[0]);
	return 1;
	}

	const char *socket_arg    = argv[1];
	const char *table_arg     = argv[2];
	const char *whitelist_arg = argv[3];
	const char *host_arg      = argv[4];
	const char *users_arg     = (argc > 5)? argv[5] : NULL;

	table = table_arg;

	int limit_val, time_val;
	char error;

	if (host_arg)
	{
	if (sscanf(host_arg, "%i/%i%c", &limit_val, &time_val, &error) != 2)
	 {
	fprintf(stderr, "%s: bad host limits '%s'\n", argv[0], host_arg);
	return 1;
	 }
	else hosts.reset(limit_val, time_val);
	}

	if (users_arg)
	{
	if (sscanf(users_arg, "%i/%i%c", &limit_val, &time_val, &error) != 2)
	 {
	fprintf(stderr, "%s: bad host limits '%s'\n", argv[0], users_arg);
	return 1;
	 }
	else users.reset(limit_val, time_val);
	}

	if (!create_socket(socket_arg))
	{
	fprintf(stderr, "%s: unable to create socket '%s': %s\n", argv[0], socket_arg, strerror(errno));
	return 1;
	}

	set_whitelist(whitelist_arg);
	register_handlers();

	while (process_connection(&violation_function) || !errno);

	destroy_socket();

	return 0;
}


static bool violation_function(const label_t &type, const address_t &address,
  const label_t &user, const label_t &service, time_t time)
{
	if (check_whitelisted(address)) return true;
	regex_check type_regex;
	if (!(type_regex = type.c_str())) return true;
	//NOTE: don't use '||', so that both will be called!
	bool host_violation = type_regex == "host" && hosts.add_violation(address, user, service, time);
	bool user_violation = type_regex == "user" && users.add_violation(address, user, service, time);
	if (host_violation || user_violation) block_address(address, table.c_str());
	return true;
}


static void exit_handler(int value)
{
	signal(value, SIG_DFL);
	destroy_socket();
	raise(value);
}


static void reload_whitelist(int value)
{
	reload_whitelist();
}


static void clear_violations(int value)
{
	hosts.clear();
	users.clear();
}


static void register_handlers()
{
    #ifdef SIGFPE
	signal(SIGFPE, &exit_handler);
    #endif

    #ifdef SIGILL
	signal(SIGILL, &exit_handler);
    #endif

    #ifdef SIGSEGV
	signal(SIGSEGV, &exit_handler);
    #endif

    #ifdef SIGBUS
	signal(SIGBUS, &exit_handler);
    #endif

    #ifdef SIGABRT
	signal(SIGABRT, &exit_handler);
    #endif

    #ifdef SIGIOT
	signal(SIGIOT, &exit_handler);
    #endif

    #ifdef SIGTRAP
	signal(SIGTRAP, &exit_handler);
    #endif

    #ifdef SIGEMT
	signal(SIGEMT, &exit_handler);
    #endif

    #ifdef SIGSYS
	signal(SIGSYS, &exit_handler);
    #endif

    #ifdef SIGPIPE
	signal(SIGPIPE, SIG_IGN);
    #endif

    #ifdef SIGLOST
	signal(SIGLOST, &exit_handler);
    #endif

    #ifdef SIGXCPU
	signal(SIGXCPU, &exit_handler);
    #endif

    #ifdef SIGXFSZ
	signal(SIGXFSZ, &exit_handler);
    #endif

    #ifdef SIGTERM
	signal(SIGTERM, &exit_handler);
    #endif

    #ifdef SIGINT
	signal(SIGINT, &exit_handler);
    #endif

    #ifdef SIGQUIT
	signal(SIGQUIT, &exit_handler);
    #endif

    #ifdef SIGHUP
	signal(SIGHUP, &exit_handler);
    #endif

    #ifdef SIGALRM
	signal(SIGALRM, &exit_handler);
    #endif

    #ifdef SIGVTALRM
	signal(SIGVTALRM, &exit_handler);
    #endif

    #ifdef SIGPROF
	signal(SIGPROF, &exit_handler);
    #endif

    #ifdef SIGTSTP
	signal(SIGTSTP, SIG_DFL);
    #endif

    #ifdef SIGTTIN
	signal(SIGTTIN, SIG_IGN);
    #endif

    #ifdef SIGTTOU
	signal(SIGTTOU, SIG_IGN);
    #endif

    #ifdef SIGUSR1
	signal(SIGUSR1, &reload_whitelist);
    #endif

    #ifdef SIGUSR2
	signal(SIGUSR2, &clear_violations);
    #endif

    #ifdef SIGPOLL
	signal(SIGPOLL, &exit_handler);
    #endif

    #ifdef SIGSTKFLT
	signal(SIGSTKFLT, &exit_handler);
    #endif

    #ifdef SIGIO
	signal(SIGIO, &exit_handler);
    #endif

    #ifdef SIGPWR
	signal(SIGPWR, &exit_handler);
    #endif

    #ifdef SIGINFO
	signal(SIGINFO, &exit_handler);
    #endif

    #ifdef SIGUNUSED
	signal(SIGUNUSED, &exit_handler);
    #endif

    #ifdef SIGSYS
	signal(SIGSYS, &exit_handler);
    #endif
}
