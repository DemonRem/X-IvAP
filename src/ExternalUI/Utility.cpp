/* 
 * File:   Utility.cpp
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

#include "Utility.h"

#include <sstream>

std::vector<std::string> Utility::str_explode(const std::string& s, char delim)
{
    std::vector<std::string> result;
    std::istringstream iss(s);

    for (std::string token; std::getline(iss, token, delim); )
    {
        result.push_back(std::move(token));
    }

    return result;
}

bool Utility::str_endswith(const std::string& fullString, const std::string& ending)
{
    if (fullString.length() >= ending.length()) 
    {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } 
    else 
    {
        return false;
    }
}
