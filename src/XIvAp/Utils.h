#ifndef _UTILS_H_
#define _UTILS_H_

#include <string>
#include <vector>

#include "XPLMUtilities.h"

namespace xmp{

	inline std::string trim(std::string inStr, bool inLeft = true, bool inRight = true, const std::string &inDelim = " \t\f\v\r\n"){
		if(inRight)
		{
			inStr.erase(inStr.find_last_not_of(inDelim) + 1);//trim right side
		}
		if(inLeft)
		{
			inStr.erase(0, inStr.find_first_not_of(inDelim));//trim left side
		}
		return inStr;
	}

	inline std::vector<std::string> explode(const std::string &inStr, const std::string &inDelim = " \t\f\v\r\n"){
		std::vector<std::string> out;
		std::size_t found = inStr.find_first_of(inDelim);
		std::size_t lastFound = 0;

		while (found != std::string::npos){
			if(lastFound != found)
				out.push_back(trim(inStr.substr(lastFound, found - lastFound)));
			found = inStr.find_first_of(inDelim, lastFound = found + 1);
		}

		if(lastFound != inStr.size())  // Critical end
			out.push_back(trim(inStr.substr(lastFound, inStr.size() - lastFound)));

		return out;
	}


	inline std::string &replaceAll(std::string &inStr, const std::string &inSubStr, const std::string &inForReplaceStr){
		int position = inStr.find(inSubStr); // find first sub str
		while (position != std::string::npos) 
		{
			inStr.replace(position, 1, inForReplaceStr);
			position = inStr.find(inSubStr, position + 1);
		}
		return inStr;
	}

	inline std::string replaceAll(const std::string &inStr, const std::string &inSubStr, const std::string &inForReplaceStr){
		std::string str = inStr;
		int position = str.find(inSubStr); // find first sub str
		while (position != std::string::npos) 
		{
			str.replace(position, 1, inForReplaceStr);
			position = str.find(inSubStr, position + 1);
		}
		return str;
	}

}// namespace xmp



#endif /*_UTILS_H_*/