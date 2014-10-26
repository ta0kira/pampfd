#include "ipc.hpp"

#include <sstream>

#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>


static const char *socket_name = NULL;
static int socket_fd = -1;

bool create_socket(const char *name)
{
	if (socket_name || socket_fd >= 0) destroy_socket();

	int errno_temp = 0;


	struct sockaddr_un new_address;
	size_t new_length = 0;

	int new_socket = socket(PF_LOCAL, SOCK_STREAM, 0);
	if (new_socket < 0) return false;


	new_address.sun_family = AF_LOCAL;
	strncpy(new_address.sun_path, name, sizeof new_address.sun_path);

	new_length = (offsetof(struct sockaddr_un, sun_path) + SUN_LEN(&new_address) + 1);

	if (bind(new_socket, (struct sockaddr*) &new_address, new_length) < 0) return false;

	socket_name = name;
	socket_fd   = new_socket;

	if (listen(new_socket, LISTEN_LIMIT) < 0)
	{
	errno_temp = errno;
	destroy_socket();
	errno = errno_temp;
	return false;
	}


	if (((SOCKET_USER && getuid() != SOCKET_USER) || (SOCKET_GROUP && getgid() != SOCKET_GROUP)) &&
	    chown(name, SOCKET_USER, SOCKET_GROUP) < 0)
	{
	errno_temp = errno;
	destroy_socket();
	errno = errno_temp;
	return false;
	}


	if (chmod(name, SOCKET_MODE) < 0)
	{
	errno_temp = errno;
	destroy_socket();
	errno = errno_temp;
	return false;
	}

	return true;
}


static int get_connection()
{
	struct stat socket_stat;
	errno = 0;
	if (!socket_name || stat(socket_name, &socket_stat) < 0 || !S_ISSOCK(socket_stat.st_mode)) return -1;
	if (socket_fd < 0) return -1;
	struct sockaddr new_address;
	socklen_t new_length = sizeof new_address;
	return accept(socket_fd, (struct sockaddr*) &new_address, &new_length);
}


bool process_connection(violation_func function)
{
	if (!function) return false;

	label_t   type;
	address_t address;
	label_t   user;
	label_t   service;
	time_t    time;

	int errno_temp = 0;

	struct timespec new_time = { 0, 0 };
	clock_gettime(CLOCK_MONOTONIC, &new_time);

	int new_connection = get_connection();
	if (new_connection < 0) return false;

	FILE *input = fdopen(new_connection, "a+");
	if (!input)
	{
	errno_temp = errno;
	close(new_connection);
	errno = errno_temp;
	return false;
	}

	char buffer[BUFFER_SIZE] = { 0x00 };
	int index = 0;

	while (fgets(buffer, sizeof buffer, input))
	{
	int size = strlen(buffer);
	if (size) buffer[size - 1] = 0x00;
	switch (index++)
	 {
	case 0: type    = buffer; break;
	case 1: address = buffer; break;
	case 2: user    = buffer; break;
	case 3: service = buffer; break;
	default: fclose(input);
	 }
	}

	fclose(input);
	time = new_time.tv_sec;

	errno = 0;
	return index == 4 && (*function)(type, address, user, service, time);
}


void destroy_socket()
{
	if (socket_fd >= 0) close(socket_fd);
	socket_fd = -1;
	if (socket_name) unlink(socket_name);
	socket_name = NULL;
}


bool send_violation(const char *name, const label_t &type, const address_t &address, const label_t &user, const label_t &service)
{
	int errno_temp = 0;


	struct sockaddr_un new_address;
	size_t new_length = 0;

	int new_socket = socket(PF_LOCAL, SOCK_STREAM, 0);
	if (new_socket < 0) return false;


	new_address.sun_family = AF_LOCAL;
	strncpy(new_address.sun_path, name, sizeof new_address.sun_path);

	new_length = (offsetof(struct sockaddr_un, sun_path) + SUN_LEN(&new_address) + 1);

	if (connect(new_socket, (struct sockaddr*) &new_address, new_length) != 0) return false;

	FILE *output = fdopen(new_socket, "a+");
	if (!output)
	{
	errno_temp = errno;
	close(new_socket);
	errno = errno_temp;
	return false;
	}

	fprintf(output, "%s\n", type.c_str());
	fprintf(output, "%s\n", address.c_str());
	fprintf(output, "%s\n", user.c_str());
	fprintf(output, "%s\n", service.c_str());

	fclose(output);
	return true;
}
