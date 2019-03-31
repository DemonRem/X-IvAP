/***************************************************************************
 *   Copyright (C) 2006 by Martin Domig                                    *
 *   martin@domig.net                                                      *
 *                                                                         *
 *   For license information see LICENSE.xivap in the root folder of the   *
 *   source code.                                                          *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 ***************************************************************************/

#include "airlinesDB.h"
#include "helpers.h"

#include <algorithm>

#include <string>
#include <fstream>

#include "Utils.h"

using namespace xmp;

AirlinesDB::AirlinesDB()
{
	// A320AEE,Aegean Airlines (AEE),7.95,-0.4,2
	// A320@EE,Aegean Airlines (AEE),7.95,-0.4,2
	// A3  @@E2,Aegean Airlines (Acropolis) (AEE),7.95,-0.4,3
	// A320AEE3,Aegean Airlines (Star Alliance) (AEE),7.95,-0.4,1

	std::string fileName =  getXivapRessourcesDir().stl_str() + "mtl.dat";
	
	std::ifstream file(fileName.c_str(), std::ios_base::in);
		if (!file.is_open()){
		XPLMDebugString(std::string("X-IvAp Warning: Can't open the mtl file: " + fileName + "\n").c_str());
		return;
	}
	// GET AIRLINES AND LIVERIES
	while(file.good()) {
		std::string line;
		std::getline(file, line);
		//XPLMDebugString(std::string(line + "\n").c_str());
		line = trim(line);
		if (line.size() == 0 || line.at(0) == ';' || line.at(0) == '#' || line.at(0) == '/') continue;
		std::vector<std::string> tokens;
		tokens = explode(line, ",");
		// tokens[0] = A320AEE2
		// tokens[1] = Aege...
		if (tokens.size() < 2) continue;
		// GET AIRLINES
		if (tokens[0].size() == 7){
			Airline entry;
			entry.acfIcao = trim(tokens[0].substr(0, 4));
			entry.icao = trim(tokens[0].substr(4));
			entry.name = tokens[1];				
			_database.push_back(entry);
		}
		// GET LIVERIES
		else if (tokens[0].size() > 7){
			Livery entry;
			entry.acfIcao = trim(tokens[0].substr(0, 4));
			entry.icao = trim(tokens[0].substr(4, 3));
			entry.code = trim(tokens[0].substr(7));
			entry.description = tokens[1];
			_liveries.push_back(entry);
		}
		else{
			continue;
		}			
	}
}

AirlinesDB::AirlinesList AirlinesDB::getAirlines(string airlineIcao, string aircraftIcao)
{
	AirlinesList result;	
	if (length(aircraftIcao) < 1) return result;
	if (length(airlineIcao) < 1) return result;

	airlineIcao = strupcase(airlineIcao);
	aircraftIcao = strupcase(aircraftIcao);

	for(AirlinesList::const_iterator it = _database.begin(); it != _database.end(); ++it) {
		int findAirlinePos = pos(airlineIcao, it->icao);
		int findAircraftPos = pos(aircraftIcao, it->acfIcao);
		if(findAirlinePos == 0 && findAircraftPos == 0){
			result.push_back(*it);
			continue;
		}

		int findAirlineNamePos = pos(airlineIcao, it->name);
		if(findAirlineNamePos == 0 && findAircraftPos == 0){
			result.push_back(*it);
		}
	}	
	// sort result
	std::sort(result.begin(), result.end(), ICAOComparator(airlineIcao));
	return result;
}

AirlinesDB::LiveryList AirlinesDB::getLiveries(string airlineIcao, string aircraftIcao)
{
	LiveryList result;
	if (length(airlineIcao) < 1) return result;
	if (length(aircraftIcao) < 1) return result;

	airlineIcao = strupcase(airlineIcao);
	aircraftIcao = strupcase(aircraftIcao);

	for(LiveryList::const_iterator it = _liveries.begin(); it != _liveries.end(); ++it) {
		if(airlineIcao == it->icao && aircraftIcao == it->acfIcao)
			result.push_back(*it);
	}
	
	return result;
}
