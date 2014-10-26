#ifndef track_hpp
#define track_hpp

#include <string>

#include <time.h>


typedef std::string address_t;
typedef std::string label_t;


struct track
{
	//address, user, service, time
	virtual bool add_violation(const address_t&, const label_t&, const label_t&, time_t) = 0;
};

#endif //track_hpp
