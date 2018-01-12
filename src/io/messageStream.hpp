#ifndef _UTK_MESSAGE_STREAM_HPP_
#define _UTK_MESSAGE_STREAM_HPP_

#include <iostream>
#include <string>
	
inline void WARNING(const std::string &s)
{
    std::string color = "\033[1;33m";
    std::string default_console = "\033[0m";
    std::cerr<<color<<"[WARN] " <<s<<default_console<<std::endl;
}
inline void ERROR(const std::string &s)
{
    std::string color = "\033[1;31m";
    std::string default_console = "\033[0m";
    std::cerr<<color<<"[ERROR] "<<s<<default_console<<std::endl;
}
// inline void LOG(const std::string &s)
// {
//     std::string default_console = "\033[0m";
//     std::cerr<<default_console<<"[LOG] "<<s<<default_console<<std::endl;
// }

#endif
