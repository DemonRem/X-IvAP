/* 
 * Copyright (c) 2013, Laminar Research.
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

#include "XPMPMultiplayerObj8.h"
#include "XPMPMultiplayerVars.h"
#include "XPLMScenery.h"
#include "XPLMUtilities.h"
#include "XPLMDataAccess.h"
#include "XPLMScenery.h"
#include <stddef.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <memory>

#include "../../src/XIvAp/Utils.h"

using namespace std;


struct	one_inst {
	one_inst * next;
	
	XPLMDrawInfo_t			location;
	xpmp_LightStatus		lights;
	XPLMPlaneDrawState_t *	state;
	
	~one_inst() { delete next; }
};

struct	one_obj {
	one_obj *			next;
	obj_for_acf *		model;
	one_inst *			head;
	
	~one_obj() { delete head; delete next; }
};

static	one_obj *	s_worklist = NULL;


static one_inst *	s_cur_plane = NULL;

enum {
	gear_rat = 0,
	flap_rat,
	spoi_rat,
	sbrk_rat,
	slat_rat,
	swep_rat,
	thrs_rat,
	ptch_rat,
	head_rat,
	roll_rat,
	thrs_rev,

	taxi_lite_on,
	lan_lite_on,
	bcn_lite_on,
	str_lite_on,
	nav_lite_on,
	
	dref_dim
};

const char * dref_names[dref_dim] = {
	"libxplanemp/controls/gear_ratio",
	"libxplanemp/controls/flap_ratio",
	"libxplanemp/controls/spoiler_ratio",
	"libxplanemp/controls/speed_brake_ratio",
	"libxplanemp/controls/slat_ratio",
	"libxplanemp/controls/wing_sweep_ratio",
	"libxplanemp/controls/thrust_ratio",
	"libxplanemp/controls/yoke_pitch_ratio",
	"libxplanemp/controls/yoke_heading_ratio",
	"libxplanemp/controls/yoke_roll_ratio",
	"libxplanemp/controls/thrust_revers",

	"libxplanemp/controls/taxi_lites_on",
	"libxplanemp/controls/landing_lites_on",
	"libxplanemp/controls/beacon_lites_on",
	"libxplanemp/controls/strobe_lites_on",
	"libxplanemp/controls/nav_lites_on"
};


static float obj_get_float(void * inRefcon)
{
	if(s_cur_plane == NULL) return 0.0f;
	
	int v = reinterpret_cast<intptr_t>(inRefcon);
	switch(v) 
	{
		case gear_rat:			return s_cur_plane->state->gearPosition;		break;
		case flap_rat:			return s_cur_plane->state->flapRatio;			break;
		case spoi_rat:			return s_cur_plane->state->spoilerRatio;		break;
		case sbrk_rat:			return s_cur_plane->state->speedBrakeRatio;		break;
		case slat_rat:			return s_cur_plane->state->slatRatio;			break;
		case swep_rat:			return s_cur_plane->state->wingSweep;			break;
		case thrs_rat:			return s_cur_plane->state->thrust;				break;
		case ptch_rat:			return s_cur_plane->state->yokePitch;			break;
		case head_rat:			return s_cur_plane->state->yokeHeading;			break;
		case roll_rat:			return s_cur_plane->state->yokeRoll;			break;
		case thrs_rev:
			// if thrust less than zerro then we will return true for revers active;
			return (s_cur_plane->state->thrust < 0.0f) ? 1.0f : 0.0f;
			break;

		case taxi_lite_on:		return s_cur_plane->lights.taxiLights;			break;
		case lan_lite_on:		return s_cur_plane->lights.landLights;			break;
		case bcn_lite_on:		return s_cur_plane->lights.bcnLights;			break;
		case str_lite_on:		return s_cur_plane->lights.strbLights;			break;
		case nav_lite_on:		return s_cur_plane->lights.navLights;			break;			
			
		default:
			return 0.0f;
	}
}

static int obj_get_float_array(
                                   void *               inRefcon,    
                                   float *              inValues,    
                                   int                  inOffset,    
                                   int                  inCount)
{
	if(inValues == NULL) 
		return 1;
	float rv = obj_get_float(inRefcon);
	for(int i = 0; i < inCount; ++i)
		inValues[i] = rv;
	return inCount;
}

static void (*XPLMLoadObjectAsync_p)(
                                   const char *         inPath,    
                                   XPLMObjectLoaded_f   inCallback,    
                                   void *               inRefcon)=NULL;

void	obj_init()
{
	int sim, xplm;
	XPLMHostApplicationID app;
	XPLMGetVersions(&sim,&xplm,&app);
	// Ben says: we need the 2.10 SDK (e.g. X-Plane 10) to have async load at all.  But we need 10.30 to pick up an SDK bug
	// fix where async load crashes if we queue a second load before the first completes.  So for users on 10.25, they get
	// pauses.

	// async is commented because it doesn't work. (Almik says that it doesn't work.)
	//if(sim >= 10300 && xplm >= 210)
	//{
	//	XPLMLoadObjectAsync_p = (void (*)(const char *, XPLMObjectLoaded_f, void *)) XPLMFindSymbol("XPLMLoadObjectAsync");
	//}
	
	for(int i = 0; i < dref_dim; ++i)
	{
		XPLMRegisterDataAccessor(
		dref_names[i], xplmType_Float|xplmType_FloatArray, 0,
		NULL, NULL, 
		obj_get_float, NULL, 
		NULL, NULL, 
		NULL, NULL, 
		obj_get_float_array, NULL, 
		NULL, NULL, reinterpret_cast<void *>(i), NULL);
	}	
}


static std::set<std::string> gCopyiedObj8Cache;

void obj_deinit() {
	for (auto &fileName : gCopyiedObj8Cache){
		char xsystem[1024];
		XPLMGetSystemPath(xsystem);
		std::string fullPath = std::string(xsystem) + fileName;
		int res = std::remove(fullPath.c_str());
		if (res) {
			XPLMDebugString(std::string("X-IvAp Warning: Can't delete temporary created obj8 file. The file: " + fullPath + "\n").c_str());
		}		
	}
}

static void		draw_objects_for_mode(one_obj * who, int want_translucent)
{
	while(who)
	{
		obj_draw_type dt = who->model->draw_type;
		if((want_translucent && dt == draw_glass) || (!want_translucent && dt != draw_glass)) {

			static XPLMDataRef night_lighting_ref = XPLMFindDataRef("sim/graphics/scenery/percent_lights_on");
			bool use_night = XPLMGetDataf(night_lighting_ref) > 0.25;

			for(one_inst * i = who->head; i; i = i->next) {
				s_cur_plane = i;
				// TODO: set obj sate to state datarefs(dref_names) from "one_inst".
				XPLMDrawObjects(who->model->handle, 1, &i->location, use_night, 0);
			}
		}
		who = who->next;
	}
	s_cur_plane = NULL;
}

void obj_loaded_cb(XPLMObjectRef obj, void * refcon)
{
	XPLMObjectRef * targ = (XPLMObjectRef *) refcon;
	*targ = obj;
}

/********************************************************************************************************/

void prepareObj8ForUsingWithAnotherTexture(CSLPlane_t * model, obj_for_acf * obj8) {
	// generate the name for new object
	std::string destObjFile = obj8->file;
	string::size_type pos2 = destObjFile.find_last_of(".");
	if (pos2 != string::npos) {
		destObjFile.insert(pos2, "_" + model->mtlCode);
	}
	else {
		destObjFile += std::string("_" + model->mtlCode);
	}

	// trying to find an object in cache
	auto findRes = gCopyiedObj8Cache.find(destObjFile);
	if (findRes != gCopyiedObj8Cache.end()) {
		obj8->file = destObjFile;
		XPLMDebugString(std::string("X-IvAp: Modified obj8 has been found in the cache: " + obj8->file + "\n").c_str());
		return;
	}
	// copy and edit new object
	if (!obj8->textureFile.empty()) {
		std::ifstream srcObj(obj8->file.c_str(), std::ios_base::in);
		if (!srcObj.is_open()) {
			XPLMDebugString(std::string("X-IvAp Warning: Can't open the source obj8 file during copying and modifying: " + obj8->file + "\n").c_str());
			return;
		}
		
		std::ofstream destObj(destObjFile, std::ios_base::out);
		if (!srcObj.is_open()) {
			XPLMDebugString(std::string("X-IvAp Warning: Can't open the destination obj8 file during copying and modifying: " + destObjFile + "\n").c_str());
			return;
		}
		bool textureHasWritten = false;
		bool litTextureHasWritten = false;
		while (srcObj.good()) {
			std::string line;
			std::getline(srcObj, line);
			if (!textureHasWritten || !litTextureHasWritten) {
				std::string trimmedLine = xmp::trim(line);
				std::vector<std::string> tokens;
				tokens = xmp::explode(trimmedLine);
				if (tokens.size() >= 2 && xmp::trim(tokens[0]) == "TEXTURE") {
					std::ostringstream os;
					os << "TEXTURE " << obj8->textureFile;
					line = os.str();
					textureHasWritten = true;
				}
				if (tokens.size() >= 2 && xmp::trim(tokens[0]) == "TEXTURE_LIT") {
					std::ostringstream os;
					os << "TEXTURE_LIT " << obj8->litTextureFile;
					line = os.str();
					litTextureHasWritten = true;
				}
				if (tokens.size() >= 2
					&& (xmp::trim(tokens[0]) == "VT"
						|| xmp::trim(tokens[0]) == "VLINE"
						|| xmp::trim(tokens[0]) == "VLIGHT"
						|| xmp::trim(tokens[0]) == "IDX"
						|| xmp::trim(tokens[0]) == "IDX10")
					) {

					if (!textureHasWritten) {
						std::ostringstream os;
						os << "TEXTURE " << obj8->textureFile;
						destObj << os.str() << std::endl;
						textureHasWritten = true;
					}
					if (!litTextureHasWritten) {
						std::ostringstream os;
						os << "TEXTURE_LIT " << obj8->litTextureFile;
						destObj << os.str() << std::endl;
						litTextureHasWritten = true;
					}
				}
			}
			destObj << line << std::endl;
		}
		obj8->file = destObjFile;
		gCopyiedObj8Cache.emplace(destObjFile);
		XPLMDebugString(std::string("X-IvAp: Modified obj8 has been created at: " + obj8->file + "\n").c_str());
	}
}

void	obj_schedule_one_aircraft(
						CSLPlane_t *			model,
						double 					x,
						double 					y,
						double 					z,
						double 					pitch,
						double 					roll,
						double 					heading,
						int	   					full,		// 
						xpmp_LightStatus		lights,
						XPLMPlaneDrawState_t *	state)
{
	one_obj * iter;
	
	for(auto &att = model->attachments.begin(); att != model->attachments.end(); ++att)
	{
		obj_for_acf * obj8 = &*att;

		// loading obj if needed
		if(obj8->handle == NULL && !obj8->file.empty()) {
			// commented due to unused because Almik says that the async loading doesn't work actually.
// 			if (XPLMLoadObjectAsync_p) {
// 				XPLMLoadObjectAsync_p(model->file.c_str(), obj_loaded_cb, (void *)&model->handle);
// 			}
// 			else {
// 				model->handle = XPLMLoadObject(model->file.c_str());
// 			}

			// copy and edit an obj if needed for special livery/texture
			prepareObj8ForUsingWithAnotherTexture(model, obj8);

			// load an obj
			obj8->handle = XPLMLoadObject(obj8->file.c_str());			
			if (obj8->handle == NULL) {
				XPLMDebugString(std::string("X-IvAp Warning: Object8 " + obj8->file + " synchronous loading failed!\n").c_str());
			}
			XPLMDebugString(std::string("X-IvAp: Object8 " + obj8->file + " has been loaded synchronously.\n").c_str());
			obj8->file.clear();
		}
	
	
		for(iter = s_worklist; iter; iter = iter->next)
		{
			if(iter->model == obj8)
				break;
		}
		if(iter == NULL)
		{
			iter = new one_obj;
			iter->next = s_worklist;
			s_worklist = iter;
			iter->model = obj8;
			iter->head = NULL;
		}
		
		if(iter->model->handle)
		{		
			one_inst * i = new one_inst;
			i->next = iter->head;
			iter->head = i;
			i->lights = lights;
			i->state = state;
			i->location.structSize = sizeof(i->location);
			i->location.x = x;
			i->location.y = y;
			i->location.z = z;
			i->location.pitch = pitch;
			i->location.roll = roll;
			i->location.heading = heading;
		}
	}	
}
						
void	obj_draw_solid()
{
	draw_objects_for_mode(s_worklist, false);
}

void	obj_draw_translucent()
{
	draw_objects_for_mode(s_worklist, true);
}

void	obj_draw_done()
{
	delete s_worklist;
	s_worklist = NULL;
}
