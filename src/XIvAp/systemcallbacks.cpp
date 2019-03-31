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

#include "common.h"
#include "xivap.h"
#include "systemcallbacks.h"
#include "messageHandler.h"
#include "SoundGraphics.h"

extern char Graphics;
/* ******** Commands // snagar **************************************************** */

namespace xivap_cmd
{

  XPLMCommandRef toggleXivapCommand = NULL; // show/hide xivap console
  XPLMCommandRef toggleFlightPlanCommand = NULL; // show flight plan window
  XPLMCommandRef toggleXivapConnectCommand = NULL; // show xivap connection window
  XPLMCommandRef toggleXivapConsoleCommand = NULL; // show xivap console window (debug)

  int    toggleXivapCommandHandler        (XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void* inRefcon);
  int    toggleFlightPlanCommandHandler   (XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void* inRefcon);
  int    toggleXivapConnectCommandHandler (XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void* inRefcon);
  int    toggleXivapConsoleCommandHandler (XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void* inRefcon);

  int    toggleXivapCommandHandler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void *inRefcon) // snagar
  {
    //  Use this structure to have the command executed on button down.
    if (inPhase == xplm_CommandBegin)
    {
      // Toggle xivap window - from MenuHandlerCallback() function
      //uiToggleCallback(NULL);
      MenuHandlerCallback((void*)xivap.menuEntry, (void*)MENU_MAIN);
    }
    // Return 1 to pass the command to plugin windows and X-Plane.
    // Returning 0 disables further processing by X-Plane.
    return 0;
  }

  int    toggleFlightPlanCommandHandler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void *inRefcon) // snagar
  {

    //  Use this structure to have the command executed on button down.
    if (inPhase == xplm_CommandBegin)
    {
      // MenuHandlerCallback((void*)xivap.menuEntry, (void*)MENU_FLIGHTPLAN);
      xivap.flightplanForm().toggle();
    }
    // Returning 0 disables further processing by X-Plane.
    return 0;
  }

  int    toggleXivapConnectCommandHandler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void *inRefcon) // snagar
  {
    //  Use this structure to have the command executed on button down.
    if (inPhase == xplm_CommandBegin)
    {
      //MenuHandlerCallback((void*)xivap.menuEntry, (void*)MENU_CONNECT);
      xivap.connectForm().toggle();
    }
    // Returning 0 disables further processing by X-Plane.
    return 0;
  }

  int    toggleXivapConsoleCommandHandler(XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void *inRefcon) // snagar
  {
    //  Use this structure to have the command executed on button down.
    if (inPhase == xplm_CommandBegin)
    {
      MenuHandlerCallback((void*)xivap.menuEntry, (void*)MENU_CONSOLE);
    }
    // Returning 0 disables further processing by X-Plane.
    return 0;
  }

}
using namespace xivap_cmd;


/* ************ snagar - END Commands ******************************** */
PLUGIN_API int XPluginStart(char *outName, char *outSig, char *outDesc)
{
#if APL
	XPLMEnableFeature("XPLM_USE_NATIVE_PATHS",1); //for native path MacOs
#endif
#ifdef INTERNAL_BETA
	if(!xivap.betaBlocker.CheckDate())
		return 0;
#endif
		
	strcpy(outName, "X-IvAp");
	strcpy(outSig, "ivao.xivap");
	
	string desc = string(SOFTWARE_NAME) + " " 
	  + SOFTWARE_VERSION + " (" + SOFTWARE_REVISION + ") - Fly online on IVAO " + SOFTWARE_COPYRIGHT1;
	strcpy(outDesc, pconst(desc));

	Graphics=1;

	int mySubMenuItem = XPLMAppendMenuItem(XPLMFindPluginsMenu(), // Put in plugins menu
						"X-IvAp",	// Item Title
						0,			// Item Ref
						1);			// Force English
	
	xivap.menuEntry = XPLMCreateMenu("X-IvAp", 
						XPLMFindPluginsMenu(),
						mySubMenuItem, 			// Menu Item to attach to.
						MenuHandlerCallback,	// The handler
						0);	
		
	XPLMAppendMenuItem(xivap.menuEntry, "Toggle X-IvAp main window", (void*)MENU_MAIN, 1);
	xivap.menuConnect = XPLMAppendMenuItem(xivap.menuEntry, "Connect...", (void*)MENU_CONNECT, 1);
	xivap.menuDisconnect = XPLMAppendMenuItem(xivap.menuEntry, "Disconnect", (void*)MENU_DISCONNECT, 1);
	xivap.menuFlightplan = XPLMAppendMenuItem(xivap.menuEntry, "Send Flightplan...", (void*)MENU_FLIGHTPLAN, 1);
	XPLMAppendMenuItem(xivap.menuEntry, "Toggle console (debug) window", (void*)MENU_CONSOLE, 1);
	XPLMAppendMenuItem(xivap.menuEntry, "About...", (void*)MENU_ABOUT, 1);

	// disable disconnect menu entry
	XPLMEnableMenuItem(xivap.menuEntry, xivap.menuDisconnect, 0);
	
	xivap.XPluginStart();

	// Register our callback for once every frame
	XPLMRegisterFlightLoopCallback(FlightLoopCallback,		// Callback
								   -1.0,					// Interval -1.0 = once per frame
								   NULL);					// refcon not used.

	// We must return 1 to indicate successful initialization, otherwise we
	// will not be called back again. */
	
	 if (!InitSound()) XPLMDebugString(" Xivap SoundInit error\r\n"); //added for sound init
	 if (!InitGraphics()) XPLMDebugString("XIvap Graphics init error\r\n");//added for graphics init


// set Command so it can be mapped to a joystick button
  xivap_cmd::toggleXivapCommand = XPLMCreateCommand("xivap/toggle-xivap-window", "Toggle X-IvAp main window");
  xivap_cmd::toggleFlightPlanCommand = XPLMCreateCommand("xivap/toggle-xivap-flight-plan-window", "Toggle Flightplan to Send...");
  xivap_cmd::toggleXivapConnectCommand = XPLMCreateCommand("xivap/toggle-xivap-connect-window", "Toggle X-Ivap Connect...");
  xivap_cmd::toggleXivapConsoleCommand = XPLMCreateCommand("xivap/toggle-xivap-debug-console", "Toggle X-Ivap Debug Console");

  // Register our custom command
  XPLMRegisterCommandHandler(xivap_cmd::toggleXivapCommand,// in command string name - ad hock
    xivap_cmd::toggleXivapCommandHandler,       // function handler
    1,                      // Receive input before plugin windows.
    (void *)0);            // inRefcon <- never understood this

  XPLMRegisterCommandHandler(xivap_cmd::toggleFlightPlanCommand,// in command string name - ad hock
    xivap_cmd::toggleFlightPlanCommandHandler,       // function handler
    1,                      // Receive input before plugin windows.
    (void *)0);            // inRefcon <- never understood this

  XPLMRegisterCommandHandler(xivap_cmd::toggleXivapConnectCommand,// in command string name - ad hock
    xivap_cmd::toggleXivapConnectCommandHandler,       // function handler
    1,                      // Receive input before plugin windows.
    (void *)0);            // inRefcon <- never understood this

  XPLMRegisterCommandHandler(xivap_cmd::toggleXivapConsoleCommand,// in command string name - ad hock
    xivap_cmd::toggleXivapConsoleCommandHandler,       // function handler
    1,                      // Receive input before plugin windows.
    (void *)0);            // inRefcon <- never understood this

  /****************************** End Custom Command implementation ***********************************************************/
	return 1;
}

PLUGIN_API void	XPluginStop(void)
{

#ifdef BETA
	if(!xivap.betaBlocker.CheckDate())
		return;
#endif

	// Unregister the callback
	XPLMUnregisterFlightLoopCallback(FlightLoopCallback, NULL);
	  // Snagar: Unregister x-plane custom commands
  XPLMUnregisterCommandHandler(toggleXivapCommand, toggleXivapCommandHandler, 0, 0);
  XPLMUnregisterCommandHandler(toggleFlightPlanCommand, toggleFlightPlanCommandHandler, 0, 0);
  XPLMUnregisterCommandHandler(toggleXivapConnectCommand, toggleXivapConnectCommandHandler, 0, 0);
  XPLMUnregisterCommandHandler(toggleXivapConsoleCommand, toggleXivapConsoleCommandHandler, 0, 0);

	xivap.XPluginStop();
	CloseAudioSystem(); //added for sound
	CloseGauge(); //added for Graphics


}


PLUGIN_API void XPluginDisable(void)
{

#ifdef BETA
	if(!xivap.betaBlocker.CheckDate())
		return;
#endif

	xivap.XPluginDisable();
}

// macro to register RW callbacks
#define CALLBACK_REGISTER_INT(ref) \
	XPLMRegisterDataAccessor(ref, xplmType_Int, 1, &Handler_getInteger, &Handler_setInteger, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, (void*) ref, (void*) ref)

#define CALLBACK_REGISTER_STR(ref) \
	XPLMRegisterDataAccessor(ref, xplmType_Data, 1, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &Handler_getString, &Handler_setString, (void*) ref, (void*) ref)

PLUGIN_API int XPluginEnable(void)
{
#ifdef BETA
	if(!xivap.betaBlocker.CheckDate())
		return 0;
#endif

	xivap.XPluginEnable();

	// ----------------------------------
	// register callbacks for plugin SDK
	// ----------------------------------

	// int, readonly
	XPLMRegisterDataAccessor(XSB_VERS_NUMBER, xplmType_Int, 0, &Handler_getInteger, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,(void *) XSB_VERS_NUMBER,(void *) XSB_VERS_NUMBER);
	XPLMRegisterDataAccessor(XSB_CON_STATUS, xplmType_Int, 0, &Handler_getInteger, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, (void *)XSB_CON_STATUS, (void *)XSB_CON_STATUS);

	// string, readonly
	XPLMRegisterDataAccessor(XSB_VERS_STRING, xplmType_Data, 0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &Handler_getString, NULL,(void *) XSB_VERS_STRING,(void *) XSB_VERS_STRING);

	CALLBACK_REGISTER_INT(XSB_CON_PORT);
	CALLBACK_REGISTER_INT(XSB_FP_FLIGHT_TYPE);
	CALLBACK_REGISTER_INT(XSB_FP_TCAS_TYPE);
	CALLBACK_REGISTER_INT(XSB_FP_NAV_TYPE);
	CALLBACK_REGISTER_INT(XSB_FP_SPEED);
	CALLBACK_REGISTER_INT(XSB_FP_DEPARTURE_TIME);
	CALLBACK_REGISTER_INT(XSB_FP_DEPARTURE_TIME_ACTUAL);
	CALLBACK_REGISTER_INT(XSB_FP_ENROUTE_HRS);
	CALLBACK_REGISTER_INT(XSB_FP_ENROUTE_MINS);
	CALLBACK_REGISTER_INT(XSB_FP_FUEL_HRS);
	CALLBACK_REGISTER_INT(XSB_FP_FUEL_MINS);

	CALLBACK_REGISTER_STR(XSB_VERS_STRING);
	CALLBACK_REGISTER_STR(XSB_CON_CALLSIGN);
	CALLBACK_REGISTER_STR(XSB_CON_SERVER);
	CALLBACK_REGISTER_STR(XSB_CON_PILOT_ID);
	CALLBACK_REGISTER_STR(XSB_CON_PASSWORD);
	CALLBACK_REGISTER_STR(XSB_CON_REALNAME);
	CALLBACK_REGISTER_STR(XSB_CON_MODEL);
	CALLBACK_REGISTER_STR(XSB_FP_CRUISE_ALTITUDE);
	CALLBACK_REGISTER_STR(XSB_FP_CRUISE_ALTITUDE);
	CALLBACK_REGISTER_STR(XSB_FP_ALTERNATE_AIRPORT);
	CALLBACK_REGISTER_STR(XSB_FP_DEPARTURE_AIRPORT);
	CALLBACK_REGISTER_STR(XSB_FP_REMARKS);
	CALLBACK_REGISTER_STR(XSB_FP_ROUTE);

	// ----------------------------------

	return 1;
}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFrom, long inMsg, void *inParam)
{
#ifdef BETA
	if(!xivap.betaBlocker.CheckDate())
		return;
#endif

	MessageHandler(inFrom, inMsg, inParam);
}

float FlightLoopCallback(float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop,    
                         int inCounter, void *inRefcon)
{
	// multiplayer paint hook
	xivap.flightLoopCallback();

	return -1.0;
}                                   

void consoleDrawCallback(XPLMWindowID inWindowID, void *inRefcon)
{
	int		left, top, right, bottom;

	// First we get the location of the window passed in to us.
	XPLMGetWindowGeometry(inWindowID, &left, &top, &right, &bottom);

	// We now use an XPLMGraphics routine to draw a translucent dark
	// rectangle that is our window's shape.
	 XPLMDrawTranslucentDarkBox(left, top, right, bottom);

	// Finally we draw the text into the window, also using XPLMGraphics
	// routines.  The NULL indicates no word wrapping.
	for(unsigned i = 0; i < xivap.consoleText.size(); ++i) {
		XPLMDrawString(xivap.consoleText[i].first,	// color
					   left + 5, bottom + 5 + i * 10, // bottom left
					   const_cast<char*>(pconst(xivap.consoleText[i].second)),
					   NULL, xplmFont_Basic);		
	}
}                                   

void emptyDrawCallback(XPLMWindowID inWindowID,  void *inRefcon)
{
}

void emptyKeyCallback(XPLMWindowID inWindowID,
						char inKey, XPLMKeyFlags inFlags,
						char inVirtualKey, void *inRefcon, int losingFocus)
{
}

int focusdumpMouseCallback(XPLMWindowID inWindowID,
						 int x, int y, XPLMMouseStatus inMouse, void *inRefcon)
{
	xivap.dumpKeyboardFocus();
	XPLMTakeKeyboardFocus(0);
	return 0;
}

void MenuHandlerCallback(void *inMenuRef, void *inItemRef)
{
	int menuEntry = (intptr_t)inItemRef;
	switch(menuEntry) {
		case MENU_CONNECT:
			xivap.connectForm().show();
			break;
		case MENU_DISCONNECT:
			xivap.disconnectForm().show();
			break;
		case MENU_FLIGHTPLAN:
			xivap.flightplanForm().show();
			break;
		case MENU_CONSOLE:
			if(xivap.consoleVisible()) xivap.setConsoleVisible(false);
			else xivap.setConsoleVisible(true);
			break;
		case MENU_MAIN:
			uiToggleCallback(NULL);
			break;
		case MENU_ABOUT:
			xivap.messageBox().show(string(FSD_SOFTWARE_NAME)
				+ " " + SOFTWARE_VERSION + "(Rev. " + xivap.revision() + ")  -  " 
				+ SOFTWARE_COPYRIGHT1 + "     www.ivao.aero/softdev/X-IvAp");
			break;

		default:
			break;
	}
}                                   


int	connectFormHandler(XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t inParam1, intptr_t inParam2)
{
	if(inMessage == xpMsg_Create) return 0; // prevent endless loop
	return xivap.connectForm().handler(inMessage, inWidget, inParam1, inParam2);
}						

int	disconnectFormHandler(XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t inParam1, intptr_t inParam2)
{
	if(inMessage == xpMsg_Create) return 0; // prevent endless loop
	return xivap.disconnectForm().handler(inMessage, inWidget, inParam1, inParam2);
}						

int	msgBoxHandler(XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t inParam1, intptr_t inParam2)
{
	if(inMessage == xpMsg_Create) return 0; // prevent endless loop
	return xivap.messageBox().handler(inMessage, inWidget, inParam1, inParam2);
}						

int	flightplanFormHandler(XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t inParam1, intptr_t inParam2)
{
	if(inMessage == xpMsg_Create) return 0; // prevent endless loop
	return xivap.flightplanForm().handler(inMessage, inWidget, inParam1, inParam2);
}						

int	fmcFormHandler(XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t inParam1, intptr_t inParam2)
{
	if(inMessage == xpMsg_Create) return 0; // prevent endless loop
	return xivap.fmcForm().handler(inMessage, inWidget, inParam1, inParam2);
}						

int	inspectorFormHandler(XPWidgetMessage inMessage, XPWidgetID inWidget, intptr_t inParam1, intptr_t inParam2)
{
	if(inMessage == xpMsg_Create) return 0; // prevent endless loop
	return xivap.refInspector().handler(inMessage, inWidget, inParam1, inParam2);
}						

// UI callbacks
void uiTextWinDrawCallback(XPLMWindowID inWindowID, void *inRefcon)
{
	xivap.uiWindow.textDrawCallback(inWindowID, inRefcon);
}

void uiTextWinToggleCallback(void *inRefcon)
{
	xivap.uiWindow.textToggleCallback(inRefcon);
}

void uiTextWinKeyCallback(XPLMWindowID inWindowID, char inKey, XPLMKeyFlags inFlags, char inVirtualKey,
						 void *inRefcon, int losingFocus)
{
	xivap.uiWindow.textKeyCallback(inWindowID, inKey, inFlags, inVirtualKey, inRefcon, losingFocus);
}

int uiTextWinMouseCallback(XPLMWindowID inWindowID, int x, int y, XPLMMouseStatus inMouse, void *inRefcon)
{
	return xivap.uiWindow.textMouseCallback(inWindowID, x, y, inMouse, inRefcon);
}

void uiToggleCallback(void *inRefcon)
{
	if(xivap.uiWindow.visible()) {xivap.uiWindow.hide();Graphics&=~1;} 
	else {xivap.uiWindow.show();Graphics|=1;} //UI toggle  5/10/2012
}

// message window callbacks
void msgTextWinDrawCallback(XPLMWindowID inWindowID, void *inRefcon)
{
	xivap.msgWindow.messageDrawCallback(inWindowID, inRefcon);
}

void msgTextWinKeyCallback(XPLMWindowID inWindowID, char inKey, XPLMKeyFlags inFlags, char inVirtualKey,
						 void *inRefcon, int losingFocus)
{
	xivap.msgWindow.messageKeyCallback(inWindowID, inKey, inFlags, inVirtualKey, inRefcon, losingFocus);
}

int msgTextWinMouseCallback(XPLMWindowID inWindowID, int x, int y, XPLMMouseStatus inMouse, void *inRefcon)
{
	return xivap.msgWindow.messageMouseCallback(inWindowID, x, y, inMouse, inRefcon);
}

// chat window callbacks
void chatTextWinDrawCallback(XPLMWindowID inWindowID, void *inRefcon)
{
	ChatWindow *obj = static_cast<ChatWindow*>(inRefcon);
	obj->chatDrawCallback(inWindowID, inRefcon);
}

void chatTextWinKeyCallback(XPLMWindowID inWindowID, char inKey, XPLMKeyFlags inFlags, char inVirtualKey,
						 void *inRefcon, int losingFocus)
{
	ChatWindow *obj = static_cast<ChatWindow*>(inRefcon);
	obj->chatKeyCallback(inWindowID, inKey, inFlags, inVirtualKey, inRefcon, losingFocus);
}

int chatTextWinMouseCallback(XPLMWindowID inWindowID, int x, int y, XPLMMouseStatus inMouse, void *inRefcon)
{
	ChatWindow *obj = static_cast<ChatWindow*>(inRefcon);
	return obj->chatMouseCallback(inWindowID, x, y, inMouse, inRefcon);
}
///////////////////////////////////////////////////////////////
//tcas window callback 16/10/2012 added
void TcasBoxWinDrawCallback(XPLMWindowID inWindowID, void *inRefcon)
{
	xivap.Tcasbox.TcasWinDrawCallback(inWindowID, inRefcon);
}

int TcasBoxWinMouseCallback(XPLMWindowID inWindowID, int x, int y, XPLMMouseStatus inMouse, void *inRefcon)
{
	return xivap.Tcasbox.TcasWinMouseCallback(inWindowID, x, y, inMouse, inRefcon);
	
return 1;
}

void TcasBoxKeyCallback(XPLMWindowID inWindowID, char inKey, XPLMKeyFlags inFlags, char inVirtualKey,
						 void *inRefcon, int losingFocus)
{
	
}
float	TcasBoxFlightLoopCallback(
									   float                inElapsedSinceLastCall,    
									   float                inElapsedTimeSinceLastFlightLoop,    
									   int                  inCounter,    
									   void *               XFMCinRefcon)
{
	static int timer=0;
	
	xivap.Tcasbox.TcasFlightLoopcallback();
	timer++;
	if (timer>=60) {timer=0; xivap.CalculateSimRatio();}
	
	return 1;
}
