/* 
 * File:   Utility.h
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

#ifndef UTILITY_H
#define UTILITY_H

#include <string>
#include <vector>

class Utility
{
public:
    static std::vector<std::string> str_explode(std::string const & s, char delim);
    static bool str_endswith(std::string const &fullString, std::string const &ending);
    
    // Prints the specified snprintf-formatted string to the X-Plane Log.txt
    static void XLog(const char* format, ...);
};

#endif /* UTILITY_H */

