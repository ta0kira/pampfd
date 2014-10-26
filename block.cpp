#include "block.hpp"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

//TODO: at some point, get rid of 'system' and switch to 'ioctl'; maybe just pull out the code from 'pfctl'
//TODO: at some point, use the syslog API
//TODO: add DNS lookup?

// #include <sys/ioctl.h>
// #include <sys/types.h>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <arpa/inet.h>
//
// #include <net/pfvar.h>
//
// #define PFDEV "/dev/pf"


bool block_address(const address_t &address, const char *table)
{
	char pf_buffer[BUFFER_SIZE] = { 0x00 };
	snprintf(pf_buffer, sizeof pf_buffer, "/sbin/pfctl -t %s -T add %s", table, address.c_str());
	char logger_buffer[BUFFER_SIZE] = { 0x00 };
	snprintf(logger_buffer, sizeof logger_buffer, "/usr/bin/logger -p security.notice \"%s added to '%s' pf table\"",
	  address.c_str(), table);
	return system(pf_buffer) == 0 && system(logger_buffer) == 0;
// 	int pf = open(PFDEV, O_RDWR);
// 	if (pf < 0) return false;
//
// 	struct pfioc_table table;
// 	           struct pfioc_table {
//                      struct pfr_table         pfrio_table;
//                      void                    *pfrio_buffer;
//                      int                      pfrio_esize;
//                      int                      pfrio_size;
//                      int                      pfrio_size2;
//                      int                      pfrio_nadd;
//                      int                      pfrio_ndel;
//                      int                      pfrio_nchange;
//                      int                      pfrio_flags;
//                      u_int32_t                pfrio_ticket;
//              };
//
// 	      struct pfr_table {
//                      char            pfrt_anchor[MAXPATHLEN];
//                      char            pfrt_name[PF_TABLE_NAME_SIZE];
//                      u_int32_t       pfrt_flags;
//                      u_int8_t        pfrt_fback;
//              };
//
//
// 	struct pfr_addr add_address;
//
// 	if (!inet_aton(address, &add_address.pfra_u._pfra_ip4addr)) return false;
//
// 	{
//                      union {
//                              struct in_addr   _pfra_ip4addr;
//                              struct in6_addr  _pfra_ip6addr;
//                      }                pfra_u;
//                      u_int8_t         pfra_af;
//                      u_int8_t         pfra_net;
//                      u_int8_t         pfra_not;
//                      u_int8_t         pfra_fback;
//              };
//
//
// 	if (ioctl(pf, DIOCRADDADDRS, ) != 0) return false;
//
// fprintf(stderr, "adding %s to %s\n", address.c_str(), table);
// return true;
}
