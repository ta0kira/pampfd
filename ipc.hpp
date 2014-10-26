#ifndef ipc_hpp
#define ipc_hpp

#include <stdio.h>

#include "track.hpp"


typedef bool(*violation_func)(const label_t&, const address_t&, const label_t&, const label_t&, time_t);

bool create_socket(const char*);
bool process_connection(violation_func);
bool finish_connection();
void destroy_socket();

bool send_violation(const char*, const label_t&, const address_t&, const label_t&, const label_t&);

#endif //ipc_hpp
