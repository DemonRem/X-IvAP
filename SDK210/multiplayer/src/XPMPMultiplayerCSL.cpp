/*
 * Copyright (c) 2005, Ben Supnik and Chris Serio.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "XPMPMultiplayerCSL.h"
#include "XPLMUtilities.h"
#include "XPMPMultiplayerObj.h"
#include "XOGLUtils.h"
#include <stdio.h>
#include <algorithm>
#include "PlatformUtils.h"
#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include <string.h>

#include <errno.h>
#include <fstream>
#include <sstream>

#include "../../src/XIvAp/Utils.h"
#include <iomanip>

using std::max;

// Set this to 1 to get TONS of diagnostics on what the lib is doing.
#ifdef DEBUG
#define 	DEBUG_CSL_LOADING 1
#elif _DEBUG
#define 	DEBUG_CSL_LOADING 1
#endif // DEBUG||_DEBUG

// Set this to 1 to cause AIRLINE and LIVERY to create ICAO codes automatically
#define USE_DEFAULTING 0

enum {
	pass_Depend,
	pass_Load,
	pass_Count
};

/************************************************************************
 * UTILITY ROUTINES
 ************************************************************************/

static void MakePartialPathNativeObj(string& io_str)
{
	vector<char> chars(io_str.begin(),io_str.end());
	MakePartialPathNative(&*chars.begin(),&*chars.begin()+chars.size());
	io_str=string(chars.begin(),chars.end());
}

struct XPLMDump { 
	XPLMDump() { }
	
	XPLMDump(const string& inFileName, int lineNum, const char * line) {
		XPLMDebugString("XSB WARNING: Parse Error in file ");
		XPLMDebugString(inFileName.c_str());
		XPLMDebugString(" line ");
		char buf[32];
		sprintf(buf,"%d", lineNum);
		XPLMDebugString(buf);
		XPLMDebugString(".\n             ");
		XPLMDebugString(line);
		XPLMDebugString(".\n");
	}
	
	XPLMDump& operator<<(const char * rhs) {
		XPLMDebugString(rhs);
		return *this;
	}
	XPLMDump& operator<<(const std::string& rhs) {
		XPLMDebugString(rhs.c_str());
		return *this;
	}
	XPLMDump& operator<<(int n) {
		char buf[255];
		sprintf(buf, "%d", n);
		XPLMDebugString(buf);
		return *this;
	}
};


static 	char * 			fgets_multiplatform(char * s, int n, FILE * file);
static 	void			BreakStringPvt(const char * inString, std::vector<std::string>& outStrings,  int maxBreak, const std::string& inSeparators);
static	bool			DoPackageSub(std::string& ioPath);

bool			DoPackageSub(std::string& ioPath)
{
	for (std::map<string, string>::iterator i = gPackageNames.begin(); i != gPackageNames.end(); ++i)
	{
		if (strncmp(i->first.c_str(), ioPath.c_str(), i->first.size()) == 0)
		{
			ioPath.erase(0, i->first.size());
			ioPath.insert(0, i->second);
 			return true;
		}
	}
	return false;
}


// This routine gets one line, but handles any platforms crlf setup.
char * fgets_multiplatform(char * s, int n, FILE * file)
{
	char *	p = s;
	int			c;
	int			c1;

	// Save one slot for the null.  If we do not have enough memory
	// to do this, bail.	
	if (--n < 0)
		return(NULL);
	
	// Only bother to read if we have enough space in the char buf.
	if (n)
		do
		{
			c = getc(file);
			
			// EOF: this could mean I/O error or end of file.
			if (c == EOF)
				if (feof(file) && p != s)	// We read something and now the file's done, ok.
					break;
				else
				{
					// Haven't read yet?  I/O error?  Return NULL!
					return(NULL);
				}
			
			*p++ = c;
		}
		// Stop when we see either newline char or the buffer is full.
		// Note that the \r\n IS written to the line.
		while (c != '\n' && c != '\r' && --n);

	// Ben's special code: eat a \n if it follows a \r, etc.  Mac stdio
	// swizzles these guys a bit, so we will consolidate BOTH \r\n and \n\r into
	// just the first.  
	if (c == '\r')
	{
		c1 = getc(file);
		if (c1 != '\n') ungetc(c1, file);
	}
	if (c == '\n')
	{
		c1 = getc(file);
		if (c1 != '\r') ungetc(c1, file);
	}

	// Unless we're bailing with NULL, we MUST null terminate.	
	*p = 0;
	
	return(s);
}

// This routine breaks a line into one or more tokens based on delimitors.
void	BreakStringPvt(const char * inString, std::vector<std::string>& outStrings, 
				int maxBreak, const std::string& inSeparators)
{
	outStrings.clear();
	
	const char * endPos = inString + strlen(inString);
	const char * iter = inString;
	while (iter < endPos)
	{
		while ((iter < endPos) && (inSeparators.find(*iter) != std::string::npos))
			++iter;
		if (iter < endPos)
		{
			if (maxBreak && (maxBreak == (outStrings.size()+1)))
			{
				outStrings.push_back(std::string(iter, endPos));
				return;
			}
			const char * wordEnd = iter;
			while ((wordEnd < endPos) && (inSeparators.find(*wordEnd) == std::string::npos))
				++wordEnd;
			
			outStrings.push_back(std::string(iter, wordEnd));
			
			iter = wordEnd;			
		}
	}
}


/************************************************************************
 * CSL LOADING
 ************************************************************************/

ClsModelVertOffsetCalculator cslVertOffsetCalc;

static	bool			LoadOnePackage(const string& inPath, int pass);

bool			CSL_Init(
						const char* inTexturePath)
{
	obj_init();
	bool ok = OBJ_Init(inTexturePath);
	if (!ok)
		XPLMDump() << "XSB WARNING: we failed to find XSB's custom lighting texture at " << inTexturePath << ".\n";
	return ok;
}

void CSL_Deinit() {
	obj_deinit();
}

// This routine loads one CSL package.
bool	LoadOnePackage(const string& inPath, int pass)
{
		string					group, icao, livery, airline;
		bool					parse_err = false;
		char					line[1024*4];
		int						sim, xplm;
		XPLMHostApplicationID 	host;	

	// First locate and attempt to load the xsb_aircraft.txt file from th is package.
	string	path(inPath);
	path += (DIR_STR "xsb_aircraft.txt");
//	XPLMDebugString(path.c_str());
//	XPLMDebugString("<----gevonden\r\n");
	FILE * fi = fopen(path.c_str(), "r");

	XPLMGetVersions(&sim, &xplm, &host);
	int lineNum = 0;

	if (fi != NULL)
	{
		if (pass == pass_Load)
			XPLMDump() << "XSB: Loading package: " << path << "\n";
	
		if (pass == pass_Load)
			gPackages.push_back(CSLPackage_t());
		CSLPackage_t *	pckg = (pass == pass_Load) ? &gPackages.back() : NULL;
		if (pass == pass_Load)
			pckg->name = path;
		
		std::vector<std::string> tokens;

		// BEN SEZ: we need to understand why thsi hack would be needed!
		// I dont know why - but this seems to fix a Linux STL issue, somehow  -Martin
//		tokens.push_back("");
//		tokens.push_back("");
//		tokens.push_back("");
//		tokens.push_back("");
//		tokens.push_back("");

		// Go through the file and handle each token.
		while(!feof(fi))
		{
			if (!fgets_multiplatform(line, sizeof(line), fi))
				break;
			++lineNum;
			
			if (line[0] == '#') continue;
			
			char * p = line;
			while (*p)
			{
				if (*p == '\n' || *p == '\r') *p = 0;
				++p;
			}

			BreakStringPvt(line, tokens, 4, " \t\r\n");			

			//----------------------------------------------------------------------------------------------------
			// PACKAGE MANAGEMENT
			//----------------------------------------------------------------------------------------------------


			// EXPORT_NAME <package name>
			if (!tokens.empty() && tokens[0] == "EXPORT_NAME" && pass == pass_Depend)
			{
				if (tokens.size() == 2)				
				{
					if (gPackageNames.count(tokens[1]) == 0)
					{
						gPackageNames[tokens[1]] = inPath;
					} else {
						parse_err = true;
						XPLMDump(path, lineNum, line)  << "XSB WARNING: Package name " << tokens[1].c_str() << " already in use by "<< gPackageNames[tokens[1]].c_str() << " reqested by use by " << inPath.c_str() << "'\n";
					}
				} else {
					parse_err = true;
					XPLMDump(path, lineNum, line)  << "XSB WARNING: EXPORT_NAME command requires 1 argument.\n";
				}
			} 


			// DEPENDENCY <package name>
			if (!tokens.empty() && tokens[0] == "DEPENDENCY" && pass == pass_Load)
			{
				if (tokens.size() == 2)				
				{
					if (gPackageNames.count(tokens[1]) == 0)
					{
						XPLMDump(path, lineNum, line) << "XSB WARNING: required package " << tokens[1] << " not found.  Aborting processing of this package.\n";
						fclose(fi);
						return true;
					}
				} else {
					parse_err = true;
					XPLMDump(path, lineNum, line) << "XSB WARNING: DEPENDENCY command needs 1 argument.\n";
				}
			} 
			
			//----------------------------------------------------------------------------------------------------
			// AUSTIN OLD SCHOOL ACFS
			//----------------------------------------------------------------------------------------------------
			
			// AIRCAFT <min> <max> <path>
			if (!tokens.empty() && tokens[0] == "AIRCRAFT" && pass == pass_Load)
			{
				if (tokens.size() == 4)
				{
					if (sim >= atoi(tokens[1].c_str()) &&
						sim <= atoi(tokens[2].c_str()))
					{
						std::string fullPath = tokens[3];
						MakePartialPathNativeObj(fullPath);
						if (!DoPackageSub(fullPath))
						{
							XPLMDump(path, lineNum, line) << "XSB WARNING: package not found.\n";
							parse_err = true;
						}
						// start loading new model 
						pckg->planes.push_back(CSLPlane_t());
						pckg->planes.back().plane_type = plane_Austin;
						pckg->planes.back().file_path = fullPath;
						pckg->planes.back().moving_gear = true;
						pckg->planes.back().austin_idx = -1;
#if DEBUG_CSL_LOADING
						XPLMDebugString("      Got Airplane: ");
						XPLMDebugString(fullPath.c_str());
						XPLMDebugString("\n");
#endif
						
					}
				} else {
					parse_err = true;
					XPLMDump(path, lineNum, line) << "XSB WARNING: AIRCRAFT command takes 3 arguments.\n";
				}
			}
			
			//----------------------------------------------------------------------------------------------------
			// OBJ7 DRAWN WITH OUR CODE
			//----------------------------------------------------------------------------------------------------

			// OBJECT <filename>
			if (!tokens.empty() && tokens[0] == "OBJECT" && pass == pass_Load)
			{
				BreakStringPvt(line, tokens, 2, " \t\r\n");			
				if (tokens.size() == 2)
				{
					std::string fullPath = tokens[1];
					MakePartialPathNativeObj(fullPath);
					if (!DoPackageSub(fullPath))
					{
						XPLMDump(path, lineNum, line) << "XSB WARNING: package not found.\n";
						parse_err = true;
					}
					// start loading new model 
					pckg->planes.push_back(CSLPlane_t());
					pckg->planes.back().plane_type = plane_Obj;
					pckg->planes.back().file_path = fullPath;
					pckg->planes.back().moving_gear = true;
					pckg->planes.back().texID = 0;
					pckg->planes.back().texLitID = 0;
					// remove load obj; add flags; den_rain;
					//pckg->planes.back().obj_idx = OBJ_LoadModel(fullPath.c_str());
					pckg->planes.back().objLoaded = false;
					pckg->planes.back().objNeedLoad = true;
					pckg->planes.back().obj_idx = 0;
					/*if (pckg->planes.back().obj_idx == -1)
					{
						XPLMDump(path, lineNum, line) << "XSB WARNING: the model " << fullPath << " failed to load.\n";
						parse_err = true;
					}*/
#if DEBUG_CSL_LOADING
					XPLMDebugString("      Got Object: ");
					XPLMDebugString(fullPath.c_str());
					XPLMDebugString("\n");
#endif					
				} else {
					parse_err = true;
					XPLMDump(path, lineNum, line) << "XSB WARNING: OBJECT command takes 1 argument.\n";
				}
			}
			
			
			// TEXTURE
			if (!tokens.empty() && tokens[0] == "TEXTURE" && pass == pass_Load)
			{
				if(tokens.size() != 2)
				{
					parse_err = true;
					XPLMDump(path, lineNum, line) << "XSB WARNING: TEXTURE command takes 1 argument.\n";
				} else {
					// Load regular texture
					string texPath = tokens[1];
					MakePartialPathNativeObj(texPath);

					if (!DoPackageSub(texPath))
					{
						parse_err = true;
						XPLMDump(path, lineNum, line) << "XSB WARNING: package not found.\n";
					}
					pckg->planes.back().texLoaded = false;
					pckg->planes.back().texNeedLoad = true;
					pckg->planes.back().texPath = texPath;
					
					// Load the lit texture
					if (pckg->planes.back().texLitPath.empty()) {
						string texLitPath = texPath;
						string::size_type pos2 = texLitPath.find_last_of(".");
						if (pos2 != string::npos) {
							texLitPath.insert(pos2, "_LIT");
							pckg->planes.back().texLitLoaded = false;
							pckg->planes.back().texLitNeedLoad = true;
							pckg->planes.back().texLitPath = texLitPath;
						}
					}					
				}
			}
			// TEXTURE_LIT
			if (!tokens.empty() && tokens[0] == "TEXTURE_LIT" && pass == pass_Load) {
				if (tokens.size() != 2) {
					parse_err = true;
					XPLMDump(path, lineNum, line) << "XSB WARNING: TEXTURE_LIT command takes 1 argument.\n";
				}
				else {
					// Load regular texture
					string texLitPath = tokens[1];
					MakePartialPathNativeObj(texLitPath);

					if (!DoPackageSub(texLitPath)) {
						parse_err = true;
						XPLMDump(path, lineNum, line) << "XSB WARNING: package not found.\n";
					}
					// Load the specific lit texture
					pckg->planes.back().texLitLoaded = false;
					pckg->planes.back().texLitNeedLoad = true;
					pckg->planes.back().texLitPath = texLitPath;
				}
			}

			//----------------------------------------------------------------------------------------------------
			// OBJ8 MULTI-OBJ WITH SIM RENDERING
			//----------------------------------------------------------------------------------------------------

			// OBJ8_AIRCRAFT
			if (!tokens.empty() && tokens[0] == "OBJ8_AIRCRAFT" && pass == pass_Load)
			{
				BreakStringPvt(line, tokens, 2, " \t\r\n");			

				if(tokens.size() == 2)
				{
					// start loading new model 
					pckg->planes.push_back(CSLPlane_t());
					pckg->planes.back().plane_type = plane_Obj8;
					pckg->planes.back().file_path = tokens[1];		// debug str
					pckg->planes.back().moving_gear = true;
					pckg->planes.back().texID = 0;
					pckg->planes.back().texLitID = 0;
					pckg->planes.back().obj_idx = -1;
				}
				else
				{
					parse_err = true;
					XPLMDump(path, lineNum, line) << "XSB WARNING: OBJ8_AIRCARFT command takes 1 argument.\n";				
				}
			}

			// OBJ8 <group> <animate YES|NO> <filename> {<texture filename> <lit_texture filename>}
			if (!tokens.empty() && tokens[0] == "OBJ8" && pass == pass_Load)
			{
				BreakStringPvt(line, tokens, 6, " \t\r\n");			

				if(tokens.size() >= 4 && tokens.size() <= 6)
				{
					if(pckg->planes.empty() || pckg->planes.back().plane_type != plane_Obj8)
					{
						// err - obj8 record at stupid place in file
						parse_err = true;
						XPLMDump(path, lineNum, line) << "XSB WARNING: OBJ8 command must be placed after OBJ8_AIRCARFT command.\n";
					}
					else
					{

						obj_for_acf		att;
						// DRAW TYPE
						if(tokens[1] == "GLASS")
							att.draw_type = draw_glass;
						else if(tokens[1] == "LIGHTS")
							att.draw_type = draw_lights;
						else if(tokens[1] == "LOW_LOD")
							att.draw_type = draw_low_lod;
						else if(tokens[1] == "SOLID")
							att.draw_type = draw_solid;
						else {
							// err crap enum
							parse_err = true;
							XPLMDump(path, lineNum, line) << "XSB WARNING: Second argument of OBJ8 command must be: GLASS or LIGHTS or LOW_LOD or SOLID.\n";
						}
						// NEED ANIMATION
						if(tokens[2] == "YES")
							att.needs_animation = true;
						else if(tokens[2] == "NO")
							att.needs_animation = false;
						else
						{
							// crap flag
							parse_err = true;
							XPLMDump(path, lineNum, line) << "XSB WARNING: Third argument of OBJ8 command must be: YES or NO.\n";
						}
						// PATH TO OBJ
						std::string fullPath = tokens[3];
						MakePartialPathNativeObj(fullPath);
						if (!DoPackageSub(fullPath))
						{
							XPLMDump(path, lineNum, line) << "XSB WARNING: package not found.\n";
							parse_err = true;
						}

						char xsystem[1024];
						XPLMGetSystemPath(xsystem);
#if APL
						ConvertPath1(xsystem, xsystem, 1024);
						//HFS2PosixPath(xsystem, xsystem, 1024);
#endif
						int sys_len = strlen(xsystem);
						if (fullPath.size() > sys_len) {
							fullPath.erase(fullPath.begin(), fullPath.begin() + sys_len);
						}
						else {
							// should probably freak out here.
							XPLMDump(path, lineNum, line) << "XSB WARNING: Path to obj8 file is not correct!.\n";
							parse_err = true;
						}
						
						att.handle = NULL;
						att.file = fullPath;

						// PATH TO TEXTURE
						if (tokens.size() >= 5){
							std::string texturePath = tokens[4];
							att.textureFile = texturePath;
							if (tokens.size() >= 6 && !tokens[5].empty()) {// specific lit texture
								att.litTextureFile = tokens[5];
							}
							else {// regular lit texture
								string::size_type pos2 = texturePath.find_last_of(".");
								if (pos2 != string::npos) {
									texturePath.insert(pos2, "_LIT");
									att.litTextureFile = texturePath;
								}
								else {
									att.litTextureFile = texturePath + std::string("_LIT");
								}
							}							
						}
						else{
							att.textureFile.clear();
							att.litTextureFile.clear();
						}

						pckg->planes.back().attachments.push_back(att);
					}					
				}
				else
				{
					// err - f---ed line.
				}
			}

			//----------------------------------------------------------------------------------------------------
			// MATCHING CRAP AND OTHER COMMON META-DATA
			//----------------------------------------------------------------------------------------------------

			// VERT_OFFSET
			// this is the csl-model vertical offset for accurate putting planes onto the ground.
			if (!tokens.empty() && tokens[0] == "VERT_OFFSET" && pass == pass_Load) {
				if (tokens.size() != 2) {
					parse_err = true;
					XPLMDump(path, lineNum, line) << "XSB WARNING: VERT_OFFSET command takes 1 argument.\n";
				}
				else {
					pckg->planes.back().xsbVertOffset = atof(tokens[1].c_str());
					pckg->planes.back().isXsbVertOffsetAvail = true;
				}
			}
			// HASGEAR YES|NO
			// This line specifies whether the previous plane has retractable gear. 
			// Useful for preventing XSB from rolling up a C152's gear on takeoff!
			if (!tokens.empty() && tokens[0] == "HASGEAR" && pass == pass_Load)
			{
				if (tokens.size() != 2 || (tokens[1] != "YES" && tokens[1] != "NO"))
				{
					parse_err = true;
					XPLMDump(path, lineNum, line) << "XSB WARNING: HASGEAR takes one argument that must be YES or NO.\n";
				} else {
					if (tokens[1] == "YES")
						pckg->planes.back().moving_gear = true;
					else if (tokens[1] == "NO")
						pckg->planes.back().moving_gear = false;
					else {
						parse_err = true;
						XPLMDump(path, lineNum, line) << "XSB WARNING: HASGEAR must have a YES or NO argument, but we got " << tokens[1] << ".\n";
					}
				}
			}
			
			// ICAO <code>
			// This line maps one ICAO code to the previous airline, without 
			// specifying an airline or livery.
			if (!tokens.empty() && tokens[0] == "ICAO" && pass == pass_Load)
			{
				BreakStringPvt(line, tokens, 0, " \t");
				if (tokens.size() == 2)
				{
					icao = tokens[1];
					group = gGroupings[icao];
					// collecting the correct mtl code
					pckg->planes.back().mtlCode = icao;
					pckg->planes.back().icaoCode = icao;

					if (pckg->matches[match_icao].count(icao) == 0) {
						pckg->matches[match_icao][icao] = pckg->planes.size() - 1;
					}
					if (!group.empty()) {
						if (pckg->matches[match_group].count(group) == 0) {
							pckg->matches[match_group][group] = pckg->planes.size() - 1;
						}
					}
				} else {
					parse_err = true;
					XPLMDump(path, lineNum, line) << "XSB WARNING: ICAO command takes 1 argument.\n";
				}				
			}

			// AIRLINE <code> <airline>
			// This line maps one ICAO code to the previous airline, with
			// an airline but without a livery.  This will also create
			// an ICAO-only association for non-airline-specific matching.			
			if (!tokens.empty() && tokens[0] == "AIRLINE" && pass == pass_Load)
			{
				BreakStringPvt(line, tokens, 0, " \t");
				if (tokens.size() == 3)
				{
					icao = tokens[1];
					airline = tokens[2];
					group = gGroupings[icao];
					// collecting the correct mtl code
					std::ostringstream os;
					os << std::left << std::setw(4) << std::setfill(' ') << icao;
					pckg->planes.back().mtlCode = os.str() + airline;
					pckg->planes.back().icaoCode = icao;
					pckg->planes.back().airlineCode = airline;

					if (pckg->matches[match_icao_airline].count(icao + " " + airline) == 0)
						pckg->matches[match_icao_airline]      [icao + " " + airline] = pckg->planes.size() - 1;
#if USE_DEFAULTING						
					if (pckg->matches[match_icao		].count(icao				) == 0)
						pckg->matches[match_icao		]      [icao				] = pckg->planes.size() - 1;
#endif						
					if (!group.empty())
					{
#if USE_DEFAULTING					
						if (pckg->matches[match_group	     ].count(group				  ) == 0)
							pckg->matches[match_group	     ]		[group				  ] = pckg->planes.size() - 1;
#endif							
						if (pckg->matches[match_group_airline].count(group + " " + airline) == 0)
							pckg->matches[match_group_airline]		[group + " " + airline] = pckg->planes.size() - 1;
					}
				} else {
					parse_err = true;
					XPLMDump(path, lineNum, line) << "XB WARNING: AIRLINE command takes two arguments.\n";
				}				
			}
			
			// LIVERY <code> <airline> <livery>
			// This line maps one ICAO code to the previous airline, with
			// an airline and livery.  This will also create
			// an ICAO-only and ICAO/airline association for non-airline-specific 
			// matching.						
			if (!tokens.empty() && tokens[0] == "LIVERY" && pass == pass_Load)
			{
				BreakStringPvt(line, tokens, 0, " \t");
				if (tokens.size() == 4)
				{
					icao = tokens[1];
					airline = tokens[2];
					livery = tokens[3];
					group = gGroupings[icao];
					// collecting the correct mtl code
					std::ostringstream os;
					os << std::left << std::setw(4) << std::setfill(' ') << icao;
					pckg->planes.back().mtlCode = os.str() + airline + livery;
					pckg->planes.back().icaoCode = icao;
					pckg->planes.back().airlineCode = airline;
					pckg->planes.back().liveryCode = livery;

#if USE_DEFAULTING					
					if (pckg->matches[match_icao				].count(icao							   ) == 0)
						pckg->matches[match_icao				]	   [icao							   ] = pckg->planes.size() - 1;
					if (pckg->matches[match_icao				].count(icao							   ) == 0)
						pckg->matches[match_icao_airline 		]	   [icao + " " + airline			   ] = pckg->planes.size() - 1;
#endif						
					if (pckg->matches[match_icao_airline_livery ].count(icao + " " + airline + " " + livery) == 0)
						pckg->matches[match_icao_airline_livery ]	   [icao + " " + airline + " " + livery] = pckg->planes.size() - 1;
					if (!group.empty())
					{
#if USE_DEFAULTING					
						if (pckg->matches[match_group		 		 ].count(group							     ) == 0)
							pckg->matches[match_group		 		 ]		[group							     ] = pckg->planes.size() - 1;
						if (pckg->matches[match_group_airline		 ].count(group + " " + airline			     ) == 0)
							pckg->matches[match_group_airline		 ]		[group + " " + airline			     ] = pckg->planes.size() - 1;
#endif							
						if (pckg->matches[match_group_airline_livery ].count(group + " " + airline + " " + livery) == 0)
							pckg->matches[match_group_airline_livery ]		[group + " " + airline + " " + livery] = pckg->planes.size() - 1;
					}
				} else {
					parse_err = true;
					XPLMDump(path, lineNum, line) << "XSB WARNING: LIVERY command takes two arguments.\n";
				}				
			}
		}
		fclose(fi);
	} else {
		XPLMDump() << "XSB WARNING: package '" << inPath << "' could not be opened.  Error was: "  << strerror(errno) << ".\n";
	}
	return parse_err;		
}

// This routine loads the related.txt file and also all packages.
bool CSL_LoadCSL(const char * inResourcesDir, const char * inFolderPath, const char * inRelatedFile, const char * inDoc8643)
{
	cslVertOffsetCalc.setResourcesDir(inResourcesDir);

	bool ok = true;

	// read the list of aircraft codes
	FILE * aircraft_fi = fopen(inDoc8643, "r");

	if (gIntPrefsFunc("debug", "model_matching", 0))
		XPLMDebugString(string(string(inDoc8643) + " returned " + (aircraft_fi ? "valid" : "invalid") + " fp\n").c_str());
	
	if (aircraft_fi)
	{
		char	buf[1024];
		while (fgets_multiplatform(buf, sizeof(buf), aircraft_fi))
		{
			vector<string>	tokens;
			BreakStringPvt(buf, tokens, 0, "\t\r\n");

			/*
			if (gIntPrefsFunc("debug", "model_matching", 0)) {
				char str[20];
				sprintf(str, "size: %i", tokens.size());
				string s = string(str) + string(": ") + buf;
				XPLMDebugString(s.c_str());
			}
			*/

			// Sample line. Fields are separated by tabs
			// ABHCO	SA-342 Gazelle 	GAZL	H1T	-

			if(tokens.size() < 5) continue;
			CSLAircraftCode_t entry;
			entry.icao = tokens[2];
			entry.equip = tokens[3];
			entry.category = tokens[4][0];

			// Debugging stuff
			/*
			if (gIntPrefsFunc("debug", "model_matching", 0)) {
				XPLMDebugString("Loaded entry: icao code ");
				XPLMDebugString(entry.icao.c_str());
				XPLMDebugString(" equipment ");
				XPLMDebugString(entry.equip.c_str());
				XPLMDebugString(" category ");
				switch(entry.category) {
					case 'L': XPLMDebugString(" light"); break;
					case 'M': XPLMDebugString(" medium"); break;
					case 'H': XPLMDebugString(" heavy"); break;
					default: XPLMDebugString(" other"); break;
				}
				XPLMDebugString("\n");
			}
			*/

			gAircraftCodes[entry.icao] = entry;
		}
		fclose(aircraft_fi);
	} else {
		XPLMDump() << "XSB WARNING: could not open ICAO document 8643 at " << inDoc8643 << "\n";
		ok = false;
	}

	// First grab the related.txt file.
	FILE * related_fi = fopen(inRelatedFile, "r");
	if (related_fi)
	{
		char	buf[1024];
		while (fgets_multiplatform(buf, sizeof(buf), related_fi))
		{
			if (buf[0] != ';')
			{
				vector<string>	tokens;
				BreakStringPvt(buf, tokens, 0, " \t\r\n");
				string	group;
				for (int n = 0; n < tokens.size(); ++n)
				{
					if (n != 0) group += " ";
					group += tokens[n];
				}
				for (int n = 0; n < tokens.size(); ++n)
				{
					gGroupings[tokens[n]] = group;
				}				
			}
		}
		fclose(related_fi);
	} else {
		XPLMDump() << "XSB WARNING: could not open related.txt at " << inRelatedFile << "\n";
		ok = false;
	}

	// Iterate through all directories using the XPLM and load them.

	char *	name_buf = (char *) malloc(16384);
	char ** index_buf = (char **) malloc(65536);
	int	total, ret; //changed from long to int due x64 problems 24/12/2012 bvk
	
	char folder[1024];

	strcpy(folder,inFolderPath);

	XPLMGetDirectoryContents(inFolderPath, 0, name_buf, 16384, index_buf, 65536 / sizeof(char*),
							 &total, &ret);
	char txt[100];
	sprintf(txt,"I found now%d-%d files to load\r\n",total,ret); //this for a while as debugging 31/10/2012
	XPLMDebugString(txt);
	vector<string>	pckgs;
	for (int r = 0; r < ret; ++r)
	{

		char * foo = index_buf[r];
		string	path(folder); //changed 31/10/2012
		path += DIR_STR;
		path += foo;
		pckgs.push_back(path);		
	}
	free(name_buf);
	free(index_buf);
	
	for (int pass = 0; pass < pass_Count; ++pass)
	for (int n = 0; n < pckgs.size(); ++n)
	{
		if (LoadOnePackage(pckgs[n], pass))
			ok = false;
	}

#if 0
	::Microseconds((UnsignedWide*) &t2);
	double delta = (t2 - t1);
	delta /= 1000000.0;
	char buf[256];
	sprintf(buf,"CSL full load took: %lf\n", delta);
	XPLMDebugString(buf);
#endif	
	return ok;
}

/************************************************************************
 * CSL MATCHING
 ************************************************************************/
 
 // Here's the basic idea: there are six levels of matching we can get,
 // from the best (direct match of ICAO, airline and livery) to the worst
 // (match an airplane's ICAO group but not ICAO, no livery or airline).
 // So we will make six passes from best to worst, trying to match.  For
 // each pass we try each package in turn from highest to lowest priority.
 
// These structs tell us how to build the matching keys for a given pass.
static	int kUseICAO[] = { 1, 1, 0, 0, 1, 0 };
static	int kUseLivery[] = { 1, 0, 1, 0, 0, 0 };
static	int kUseAirline[] = { 0, 1, 0, 1, 0, 0 };

CSLPlane_t *	CSL_MatchPlane(const char * inICAO, const char * inAirline, const char * inLivery, bool * got_livery, bool use_default)
{
	XPLMPluginID	who;
	int		total, active;
	XPLMCountAircraft(&total, &active, &who);
	
	// First build up our various keys and info we need to do the match.
	string	icao(inICAO);
	string	airline(inAirline ? inAirline : "");
	string	livery(inLivery ? inLivery : "");
	string	group;
	string	key;

	map<string, string>::iterator group_iter = gGroupings.find(inICAO);
	if (group_iter != gGroupings.end())
		group = group_iter->second;

	char	buf[4096];

	if (gIntPrefsFunc("debug", "model_matching", 0))
	{
		sprintf(buf,"XSB MATCH - ICAO=%s AIRLINE=%s LIVERY=%s GROUP=%s\n", icao.c_str(), airline.c_str(), livery.c_str(), group.c_str());
		XPLMDebugString(buf);
	}

	// Now we go through our six passes.
	for (int n = 0; n < match_count; ++n)
	{
		// Build up the right key for this pass.
		key = kUseICAO[n] ? icao : group;
		if (kUseLivery[n])
		{
			key += " ";
			key += airline;
			key += " ";
			key += livery;
		}
		if (kUseAirline[n])
		{
			key += " ";
			key += airline;
		}

		if (gIntPrefsFunc("debug", "model_matching", 0))
		{			
			sprintf(buf,"XSB MATCH -    Group %d key %s\n", n, key.c_str());
			XPLMDebugString(buf);
		}
		
		// Now go through each group and see if we match.
		for (int p = 0; p < gPackages.size(); ++p)
		{
			map<string,int>::iterator iter = gPackages[p].matches[n].find(key);
			if (iter != gPackages[p].matches[n].end())
			if (gPackages[p].planes[iter->second].plane_type != plane_Austin ||		// Special check - do NOT match a plane that isn't loaded.
				(gPackages[p].planes[iter->second].austin_idx != -1 && gPackages[p].planes[iter->second].austin_idx < total))
			if (gPackages[p].planes[iter->second].plane_type != plane_Obj ||
				gPackages[p].planes[iter->second].obj_idx != -1)
			{
				if (got_livery) *got_livery = (kUseLivery[n] || kUseAirline[n]);

				if (gIntPrefsFunc("debug", "model_matching", 0))
				{
					sprintf(buf, "XB MATCH - Found: %s\n", gPackages[p].planes[iter->second].file_path.c_str());
					XPLMDebugString(buf);
				}
				
				return &gPackages[p].planes[iter->second];
			}
		}
	}

	if (gIntPrefsFunc("debug", "model_matching", 0))
	{
		XPLMDebugString("XSB MATCH - No match.\n");
	}



	// try the next step:
	// For each aircraft, we know the equiment type "L2T" and the WTC category.
	// try to find a model that has the same equipment type and WTC

	std::map<string, CSLAircraftCode_t>::const_iterator model_it = gAircraftCodes.find(icao);
	if(model_it != gAircraftCodes.end()) {

		if (gIntPrefsFunc("debug", "model_matching", 0))
		{
			XPLMDebugString("XSB MATCH/acf - Looking for a ");
			switch(model_it->second.category) {
				case 'L': XPLMDebugString(" light "); break;
				case 'M': XPLMDebugString(" medium "); break;
				case 'H': XPLMDebugString(" heavy "); break;
				default: XPLMDebugString(" funny "); break;
			}
			XPLMDebugString(model_it->second.equip.c_str());
			XPLMDebugString(" aircraft\n");
		}

		// 1. match WTC, full configuration ("L2P")
		// 2. match WTC, #engines and enginetype ("2P")
		// 3. match WTC, #egines ("2")
		// 4. match WTC, enginetype ("P")
		// 5. match WTC
		for(int pass = 1; pass <= 5; ++pass) {

			if (gIntPrefsFunc("debug", "model_matching", 0))
			{
				switch(pass) {
					case 1: XPLMDebugString("XSB Match/acf - matching WTC and configuration\n"); break;
					case 2: XPLMDebugString("XSB Match/acf - matching WTC, #engines and enginetype\n"); break;
					case 3: XPLMDebugString("XSB Match/acf - matching WTC, #engines\n"); break;
					case 4: XPLMDebugString("XSB Match/acf - matching WTC, enginetype\n"); break;
					case 5: XPLMDebugString("XSB Match/acf - matching WTC\n"); break;
				}
			}

			for (int p = 0; p < gPackages.size(); ++p)
			{
				std::map<string, int>::const_iterator it = gPackages[p].matches[4].begin();
				while(it != gPackages[p].matches[4].end()) {

					if (gPackages[p].planes[it->second].plane_type != plane_Austin ||		// Special check - do NOT match a plane that isn't loaded.
						(gPackages[p].planes[it->second].austin_idx != -1 && gPackages[p].planes[it->second].austin_idx < total))
					if (gPackages[p].planes[it->second].plane_type != plane_Obj ||
						gPackages[p].planes[it->second].obj_idx != -1)
					{
						// we have a candidate, lets see if it matches our criteria
						std::map<string, CSLAircraftCode_t>::const_iterator m = gAircraftCodes.find(it->first);
						if(m != gAircraftCodes.end()) {
							// category
							bool match = (m->second.category == model_it->second.category);

							// make sure we have a valid equip type if we need it
							if(pass < 5 && m->second.equip.length() != 3) match = false;

							// engine type
							if(match && (pass <= 2 || pass == 4))
								match = (m->second.equip[2] == model_it->second.equip[2]);

							// #engines
							if(match && pass <= 3)
								match = (m->second.equip[1] == model_it->second.equip[1]);

							// full configuration string
							if(match && pass == 1)
								match = (m->second.equip == model_it->second.equip);

							if(match) {
								// bingo
								if (gIntPrefsFunc("debug", "model_matching", 0))
								{
									XPLMDebugString("XSB MATCH/acf - found: ");
									XPLMDebugString(it->first.c_str());
									XPLMDebugString("\n");
								}

								return &gPackages[p].planes[it->second];
							}
						}
					}

					++it;
				}
			}
		}
	}

	if (gIntPrefsFunc("debug", "model_matching", 0)) {
		XPLMDebugString(string("gAircraftCodes.find(" + icao + ") returned no match.\n").c_str());
	}

	if (!strcmp(inICAO, gDefaultPlane.c_str())) return NULL;
	if (!use_default) return NULL;
	return CSL_MatchPlane(gDefaultPlane.c_str(), "", "", got_livery, false);
}

void	CSL_Dump(void)
{
	// DIAGNOSTICS - print out everything we know.
	for (int n = 0; n < gPackages.size(); ++n)
	{
		XPLMDump() << "XSB CSL: Package " << n << " path = " << gPackages[n].name << "\n";
		for (int p = 0; p < gPackages[n].planes.size(); ++p)
		{
			XPLMDump() << "XSB CSL:         Plane " << p << " = " << gPackages[n].planes[p].file_path << "\n";
		}
		for (int t = 0; t < 6; ++t)
		{
			XPLMDump() << "XSB CSL:           Table " << t << "\n";
			for (map<string, int>::iterator i = gPackages[n].matches[t].begin(); i != gPackages[n].matches[t].end(); ++i)
			{
				XPLMDump() << "XSB CSL:                " << i->first << " -> " << i->second << "\n";
			}
		}
	}
}

/************************************************************************
 * CSL DRAWING
 ************************************************************************/

int				CSL_GetOGLIndex(CSLPlane_t *		model)
{
	// Dynamic load obj; den_rain;
	if (model->objNeedLoad && !model->objLoaded)
	{
		model->objLoaded = true;
		model->obj_idx = OBJ_LoadModel(model->file_path.c_str());
		if (model->obj_idx == -1)
		{
			XPLMDump() << "XSB WARNING: the model " << model->file_path << " failed to dynamic load.\n";
		}
	}
	switch(model->plane_type) {
	case plane_Austin:
		return model->austin_idx;
	case plane_Obj:
		if (model->texID != 0) 
			return model->texID;
		return OBJ_GetModelTexID(model->obj_idx);
	default:
		return 0;
	}		
}

// Plane drawing couldn't be simpler - it's just a "switch" between all
// of our drawing techniques.
void			CSL_DrawObject(
							CSLPlane_t *			model,
							float					distance,
							double 					x,
							double 					y,
							double 					z,
							double 					pitch,
							double 					roll,
							double 					heading,
							int						type,
							int	   					full,
							xpmp_LightStatus		lights,
							XPLMPlaneDrawState_t *	state)
{
	// Setup OpenGL for this plane render
	if(type != plane_Obj8)
	{
		glMatrixMode(GL_MODELVIEW); 
		glPushMatrix();
		glTranslatef(x, y, z);			
		glRotatef(heading, 0.0, -1.0, 0.0);
		glRotatef(pitch, 01.0, 0.0, 0.0);
		glRotatef(roll, 0.0, 0.0, -1.0);
	}	
	float colWhite[]		= { 1.0f, 1.0f, 1.0f };

	switch (type)
	{
		
		case plane_Austin:
			{
				XPLMPluginID	who;
				int		total, active;
				XPLMCountAircraft(&total, &active, &who);
				if (model->austin_idx > 0  && model->austin_idx < active)
					XPLMDrawAircraft(model->austin_idx,
							x, y ,z, pitch, roll, heading,
							full, state);
			}
			break;
		case plane_Obj:			
			if (model->obj_idx != -1)
			{				
				// Dynamic loading texture; den_rain;
				if (model->texNeedLoad && !model->texLoaded)
				{
					model->texLoaded = true;
					model->texID = OBJ_LoadTexture(model->texPath.c_str(), false);
					if (model->texID == -1)
					{
						XPLMDump() << "Texture " << model->texPath << " failed to dynamic load.\n";
					}
				}
				if (model->texLitNeedLoad && !model->texLitLoaded)
				{
					model->texLitLoaded = true;
					
					model->texLitID = OBJ_LoadTexture(model->texLitPath.c_str(), false);
					//XPLMDebugString(model->texLitPath.c_str());

					if (model->texLitID == -1)
					{
						XPLMDump() << "Texture LIT " << model->texLitPath << " failed to dynamic load.\n";
					}
				}
				OBJ_PlotModel(model->obj_idx, model->texID, model->texLitID, full ? distance : max(distance, 10000.0f),
						x, y ,z, pitch, roll, heading);
			}
			break;
		case plane_Lights:
			if (model->obj_idx != -1)
				OBJ_DrawLights(model->obj_idx, distance,
						x, y ,z, pitch, roll, heading, lights);
						
			break;
		case plane_Obj8:
			obj_schedule_one_aircraft(
				model,
				x,
				y,
				z,
				pitch,
				roll,
				heading,
				full,		// 
				lights,
				state);
			break;

	}
	if(type != plane_Obj8)
		glPopMatrix();
}


/**************************************************************************************************/
///////////////////////////////////////////* ClsModelYOffsetCalculator CLASS *//////////////////////
/**************************************************************************************************/

ClsModelVertOffsetCalculator::~ClsModelVertOffsetCalculator() {
	saveUserOffsets();
}

void ClsModelVertOffsetCalculator::setResourcesDir(std::string inResDir) {
	if (mResourcesDir != inResDir) {
		mResourcesDir = inResDir;
		loadUserOffsets();
	}	
}

void ClsModelVertOffsetCalculator::findOrUpdateActualVertOffset(CSLPlane_t & inOutCslModel) {
	// plane updating user vert offset if it's needed
	if (!mUpdateUserOffsetForThisMtl.empty()) {
		// check if we want to update user offset for this mtl
		auto result = mUpdateUserOffsetForThisMtl.find(inOutCslModel.mtlCode);
		if (result != mUpdateUserOffsetForThisMtl.end()) {
			inOutCslModel.isUserVertOffsetUpToDate = false;
			mUpdateUserOffsetForThisMtl.erase(result);
		}
	}
	// set/update user vert offset if it's needed
	if (!inOutCslModel.isUserVertOffsetUpToDate) {
		inOutCslModel.isUserVertOffsetUpToDate = true;
		inOutCslModel.userVertOffset = 0.0;
		inOutCslModel.isUserVertOffsetAvail = false;
		// search for specific user offset
		auto result2 = mAvailableUserOffsets.find(inOutCslModel.mtlCode);
		if (result2 != mAvailableUserOffsets.end()) {
			inOutCslModel.userVertOffset = result2->second;
			inOutCslModel.isUserVertOffsetAvail = true;
			XPLMDebugString(std::string("X-IvAp: The USER Y offset (" + std::to_string(inOutCslModel.userVertOffset) + ") for the model has been found; "
										"Mtl Code: " + inOutCslModel.mtlCode + "\n").c_str());
		}
	}

	// just check and log the fact of avail of xsb offset
	if (!inOutCslModel.isXsbVertOffsetUpToDate) {
		inOutCslModel.isXsbVertOffsetUpToDate = true;
		if (inOutCslModel.isXsbVertOffsetAvail) {
			XPLMDebugString(std::string("X-IvAp: The Y offset (" + std::to_string(inOutCslModel.xsbVertOffset) + ") for the model has been found in the xsb file; "
										"Mtl Code: " + inOutCslModel.mtlCode + "\n").c_str());
		}
	}

	// set/update calculated vert offset if it's needed
	if (!inOutCslModel.isCalcVertOffsetUpToDate) {
		inOutCslModel.isCalcVertOffsetUpToDate = true;
		inOutCslModel.calcVertOffset = 0.0;
		inOutCslModel.isCalcVertOffsetAvail = false;
		if (inOutCslModel.plane_type == plane_Obj) {
			// trying to calculate it from the obj file
			findOffsetInObj(inOutCslModel);
		}
		else if (inOutCslModel.plane_type == plane_Obj8) {
			// trying to calculate it from the obj8 files
			findOffsetInObj8(inOutCslModel);
		}
	}

	//-------------------------------------------------------------------------
	// setup actual Y offset
	inOutCslModel.actualVertOffsetType = eVertOffsetType::none;
	// if user offset is avail
	if (inOutCslModel.isUserVertOffsetAvail) {
		inOutCslModel.actualVertOffset = inOutCslModel.userVertOffset;
		inOutCslModel.actualVertOffsetType = eVertOffsetType::user;
	}
	// if xsb offset is avail
	else if (inOutCslModel.isXsbVertOffsetAvail) {
		inOutCslModel.actualVertOffset = inOutCslModel.xsbVertOffset;
		inOutCslModel.actualVertOffsetType = eVertOffsetType::xsb;
	}
	// if calc offset is avail
	else if (inOutCslModel.isCalcVertOffsetAvail) {
		inOutCslModel.actualVertOffset = inOutCslModel.calcVertOffset;
		inOutCslModel.actualVertOffsetType = eVertOffsetType::calculated;
	}
	// if something wrong and we not have any vert offsets, use 0.0
	if (inOutCslModel.actualVertOffsetType == eVertOffsetType::none) {
		XPLMDebugString(std::string("X-IvAp Warning: The Y offset for the model is not found. Will use 0 as the vert offset. "
									"Mtl code: " + inOutCslModel.mtlCode + "\n").c_str());
		inOutCslModel.calcVertOffset = 0.0;
		inOutCslModel.isCalcVertOffsetAvail = true;
		inOutCslModel.actualVertOffset = inOutCslModel.calcVertOffset;
		inOutCslModel.actualVertOffsetType = eVertOffsetType::byDefault;
	}
	if (inOutCslModel.actualVertOffsetType != inOutCslModel.prevActualVertOffsetType) {
		inOutCslModel.prevActualVertOffsetType = inOutCslModel.actualVertOffsetType;
		XPLMDebugString(std::string("X-IvAp: Using the " + offsetTypeToString(inOutCslModel.actualVertOffsetType) + " Y offset (" + std::to_string(inOutCslModel.actualVertOffset) + ") for the model. "
									"Mtl code: " + inOutCslModel.mtlCode + "\n").c_str());
	}
}

void ClsModelVertOffsetCalculator::actualVertOffsetInfo(std::string inMtl, std::string &outType, double &outOffset) {
	for (auto &package : gPackages){
		for (auto &model : package.planes){
			if (model.mtlCode == inMtl) {
				findOrUpdateActualVertOffset(model);
				outType = offsetTypeToString(model.actualVertOffsetType);
				outOffset = model.actualVertOffset;
				return;
			}
		}
	}
}

void ClsModelVertOffsetCalculator::setUserVertOffset(std::string inMtlCode, double inOffset) {
	mAvailableUserOffsets[inMtlCode] = inOffset;
	mUpdateUserOffsetForThisMtl.emplace(inMtlCode);

}

void ClsModelVertOffsetCalculator::removeUserVertOffset(std::string inMtlCode) {
	mAvailableUserOffsets.erase(inMtlCode);
	mUpdateUserOffsetForThisMtl.emplace(inMtlCode);
}

/**************************************************************************************************/
///////////////////////////////////////////* Functions *////////////////////////////////////////////
/**************************************************************************************************/

bool ClsModelVertOffsetCalculator::findOffsetInObj8(CSLPlane_t &inOutCslModel) {
	// todo: add warning for rotate and translate
	if (inOutCslModel.plane_type == plane_Obj8) {
		double min = 0.0;
		double max = 0.0;
		bool isRotateOrTranslateAnimDetected = false;
		for (auto &obj8 : inOutCslModel.attachments){			
			if (obj8.draw_type == draw_solid) {
				std::ifstream file(obj8.file.c_str(), std::ios_base::in);
				if (!file.is_open()) {
					XPLMDebugString(std::string("X-IvAp Warning: The Y offset for the model is not found in obj8. Can't open the file: " + obj8.file + "\n").c_str());
					return false;
				}
				int coordLinesNumber = 0;
				for (size_t lineNumber = 0; file.good(); lineNumber++) {
					std::string line;
					std::getline(file, line);
					line = xmp::trim(line);
					if (line.size() == 0 || line.at(0) == ';' || line.at(0) == '#' || line.at(0) == '/') continue;
					if (lineNumber == 1 && atoi(line.c_str()) < 800) {
						XPLMDebugString(std::string("X-IvAp Warning: The Y offset for the model is not found in obj. "
													"Expect obj8 or higher but obj7 or lower is given. The obj: " + obj8.file + "\n").c_str());
						return false;
					}
					std::vector<std::string> tokens;
					tokens = xmp::explode(line);
					if (tokens.size() >= 4) {
						if (xmp::trim(tokens[0]) == "VT" || xmp::trim(tokens[0]) == "VLINE") {
							double y = std::atof(xmp::trim(tokens[2]).c_str());
							if (y < min) {
								min = y;
							}
							if (y > max) {
								max = y;
							}
							++coordLinesNumber;
						}
					}
					if (xmp::trim(tokens[0]) == "ANIM_trans" || xmp::trim(tokens[0]) == "ANIM_rotate") {
						isRotateOrTranslateAnimDetected = true;
					}
				}
				if (coordLinesNumber < 3) {
					XPLMDebugString(std::string("X-IvAp Warning: The Y offset for the model is not found in obj8. "
												"Number of lines with coordinates (VT&VLINE) is too small. The obj: " + obj8.file + "\n").c_str());
					return false;
				}
			}
		}
		if (min < 0.0) {
			inOutCslModel.calcVertOffset = std::fabs(min);
		}
		else {
			inOutCslModel.calcVertOffset = -max;
		}
		inOutCslModel.isCalcVertOffsetAvail = true;
		if (isRotateOrTranslateAnimDetected) {
			XPLMDebugString(std::string("X-IvAp Warning: During calculating the Y offset for the model Translate or/and Rotate animation has been found in an obj8; "
										"So, the calculated Y offset can be wrong due to animations. "
										"Mtl code: " + inOutCslModel.mtlCode + "\n").c_str());
		}
		XPLMDebugString(std::string("X-IvAp: The Y offset (" + std::to_string(inOutCslModel.calcVertOffset) + ") for the model has been calculated from the obj8; "
									"Mtl code: " + inOutCslModel.mtlCode + "\n").c_str());
		return true;
	}
	return false;
}

bool ClsModelVertOffsetCalculator::findOffsetInObj(CSLPlane_t &inOutCslModel) {
	if (inOutCslModel.plane_type == plane_Obj) {
		std::ifstream file(inOutCslModel.file_path.c_str(), std::ios_base::in);
		if (!file.is_open()) {
			XPLMDebugString(std::string("X-IvAp Warning: The Y offset for the model is not found in obj. Can't open the file: " + inOutCslModel.file_path + "\n").c_str());
			return false;
		}
		double min = 0.0;
		double max = 0.0;
		int coordLinesNumber = 0;
		for (size_t lineNumber = 0; file.good(); lineNumber++) {
			std::string line;
			std::getline(file, line);
			line = xmp::trim(line);
			if (line.size() == 0 || line.at(0) == ';' || line.at(0) == '#' || line.at(0) == '/') continue;
			if (lineNumber == 1 && atoi(line.c_str()) >= 800) {
				XPLMDebugString(std::string("X-IvAp Warning: The Y offset for the model is not found in obj. "
											"Expect obj6 or obj7 but obj8 or higher is given. The obj: " + inOutCslModel.file_path + "\n").c_str());
				return false;
			}
			std::vector<std::string> tokens;
			tokens = xmp::explode(line);
			if (tokens.size() >= 3) {
				double x, y, z;
				x = std::atof(tokens[0].c_str());
				y = std::atof(tokens[1].c_str());
				z = std::atof(tokens[2].c_str());
				if (x != 0.0 && y != 0.0 && z != 0.0) {					
					if (y < min) {
						min = y;
					}
					if (y > max) {
						max = y;
					}
					++coordLinesNumber;
				}
			}
		}
		if (coordLinesNumber < 3) {
			XPLMDebugString(std::string("X-IvAp Warning: The Y offset for the model is not found in obj. "
										"Number of lines with coordinates is too small. The obj: " + inOutCslModel.file_path + "\n").c_str());
			return false;
		}
		if (min < 0.0) {
			inOutCslModel.calcVertOffset = std::fabs(min);
		}
		else {
			inOutCslModel.calcVertOffset = -max;
		}
		inOutCslModel.isCalcVertOffsetAvail = true;
		XPLMDebugString(std::string("X-IvAp: The Y offset (" + std::to_string(inOutCslModel.calcVertOffset) + ") for the model has been calculated from its obj8 files; "
									"Mtl code: " + inOutCslModel.mtlCode + "\n").c_str());
		return true;
	}
	return false;
}

void ClsModelVertOffsetCalculator::loadUserOffsets() {
	std::string fileName = mResourcesDir + "userVertOffsets.txt";
	std::ifstream file(fileName.c_str(), std::ios_base::in);
	if (!file.is_open()) {
		XPLMDebugString(std::string("X-IvAp Warning: Can't open the user vertical offsets file: " + fileName + "\n").c_str());
		return;
	}
	mAvailableUserOffsets.clear();
	while (file.good()) {
		std::string line;
		std::getline(file, line);
		line = xmp::trim(line);
		if (line.size() == 0 || line.at(0) == ';' || line.at(0) == '#' || line.at(0) == '/') continue;
		std::vector<std::string> tokens;
		tokens = xmp::explode(line, ",");
		if (tokens.size() < 2) continue;
		if (tokens[0].size() <= 4) {// only icao
			mAvailableUserOffsets.emplace(xmp::trim(tokens[0]), std::atof(tokens[1].c_str()));
		}
		else if (tokens[0].size() == 7) {// icao and airline
			mAvailableUserOffsets.emplace(xmp::trim(tokens[0].substr(0, 4)) + xmp::trim(tokens[0].substr(4)),
											  std::atof(tokens[1].c_str()));
		}
		else if (tokens[0].size() > 7) {// icao, airline, livery
			mAvailableUserOffsets.emplace(xmp::trim(tokens[0].substr(0, 4))
											  + xmp::trim(tokens[0].substr(4, 3))
											  + xmp::trim(tokens[0].substr(7)),
											  std::atof(tokens[1].c_str()));
		}
		else {
			continue;
		}
	}
}

void ClsModelVertOffsetCalculator::saveUserOffsets() {
	std::string fileName = mResourcesDir + "userVertOffsets.txt";
	std::ofstream file(fileName.c_str(), std::ios_base::out);
	if (!file.is_open()) {
		XPLMDebugString(std::string("X-IvAp Warning: Can't open the user vertical offsets file: " + fileName + "\n").c_str());
		return;
	}
	for (auto &item : mAvailableUserOffsets){
		file << item.first << ", " << item.second << std::endl;
	}
	file.close();
}

std::string ClsModelVertOffsetCalculator::offsetTypeToString(eVertOffsetType inOffsetType) {
	switch (inOffsetType) {
		case eVertOffsetType::byDefault:
			return "Default";
			break;
		case eVertOffsetType::user:
			return "User";
			break;
		case eVertOffsetType::xsb:
			return "Xsb";
			break;
		case eVertOffsetType::calculated:
			return "Calculated";
			break;
		case eVertOffsetType::none:			
		default:
			return "Not defined";
			break;
	}
}
