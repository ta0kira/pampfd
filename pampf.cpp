#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "ipc.hpp"


int main(int argc, char *argv[])
{
	if (argc < 3 || argc > 6)
	{
	fprintf(stderr, "%s [type] [socket name] (host) (user) (service)\n", argv[0]);
	return 1;
	}

	const char *type_arg    = argv[1];
	const char *socket_arg  = argv[2];
	const char *host_arg    = (argc > 3)? argv[3] : getenv("PAM_RHOST");
	const char *user_arg    = (argc > 4)? argv[4] : getenv("PAM_USER");
	const char *service_arg = (argc > 5)? argv[5] : getenv("PAM_SERVICE");

	if (!send_violation(socket_arg, type_arg, host_arg? host_arg : "", user_arg? user_arg : "", service_arg? service_arg : ""))
	{
	fprintf(stderr, "%s: could not send violation: %s\n", argv[0], strerror(errno));
	return 1;
	}

	return 0;
}
