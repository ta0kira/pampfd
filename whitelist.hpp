#ifndef whitelist_hpp
#define whitelist_hpp

#include <string>


bool set_whitelist(const std::string&);
bool reload_whitelist();
bool check_whitelisted(const std::string&);

#endif //whitelist_hpp
