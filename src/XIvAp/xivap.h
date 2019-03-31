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

#ifndef XIVAP_H_
#define XIVAP_H_

#include "common.h"
#include "fsdapi.h"
#include "connectForm.h"
#include "disconnectForm.h"
#include "flightplanForm.h"
#include "FMCform.h"
#include "uiWindow.h"
#include "atcPosition.h"
#include "weatherPosition.h"
#include "weatherGod.h"
#include "multiplayer.h"
#include "HTTPClient.h"
#include "messageWindow.h"
#include "chatWindow.h"
#include "aircraftDB.h"
#include "airlinesDB.h"
#include "XPmessageBox.h"
#include "refInspector.h"
#include "navQuery.h"
#include "WristWatch.h"
#include "planeposition.h"
#include "Ivaosecurity.h"
#include "Tcas.h" //added 16/10/2012
#include "TeamSpeak.h"

#include <deque>

#define CONSOLE_LINES   28
#define CONSOLE_ROWS   150

// min. time span between sending 2 param packets
// DO NOT CHANGE - p2p pos updates depend on that value being 5
// (otherwise the framenumber calculation is b0rk3d)
#define PARAMS_MINDELAY	5.0f

// check for new ATIS'es this often
#define RADIO_ATIS_CHECK_INTERVAL	1.2f



class Xivap {
public:
  /** Constructor */
  Xivap();

  /** Destructor */
  ~Xivap();

  double nextFSDPosUpdate, nextFSDPoll, nextStatusUpdate;

  int	menuConnect, menuDisconnect, menuFlightplan;
  XPLMMenuID menuEntry;

  typedef std::pair<float*, pt::string> line;
  std::deque<line> consoleText;

  //AirlinesDB airlinesDB;
  FsdAPI fsd;
  Flightplan fpl;

  void XPluginStart();
  void XPluginStop();
  void XPluginEnable();
  void XPluginDisable();
  void aircraftChange();
  void airportChange();

  void setAcType(const pt::string& aircraft, const pt::string& airline, const pt::string& livery);

  void connect(const pt::string& callsign, const pt::string& vid, const pt::string& password,
    const pt::string& realname, bool hideadm, const pt::string& server, const pt::string& port,
    bool voice, bool fmcar);
  void disconnect();

  void flightLoopCallback();
  void FSDPoll();
  void sendPosUpdate();
  void updateStatus();
  void updatePosition(PlanePosition* position = NULL);
  bool CalculateSimRatio(void);

  double elevationft() const { return el * 3.2808399; };
  double flightlevelft() const;
  bool onGround() const { return _onground; };

  void sendFlightplan();
  void addText(float* color, const pt::string& text, bool logToScreen = true, bool logToFile = false);

  pt::string comStr(int radio);
  pt::string com1freq() { return freq2str(_com1freq); };
  pt::string com2freq() { return freq2str(_com2freq); };

  pt::string acType() { return _acType; };
  pt::string deptAirport() { return _deptAirport; };
  pt::string destAirport() { return _destAirport; };
  pt::string flightRoute() { return _flightRoute; };
  pt::string _PilotDisable1; //added for shared cockpit
  pt::string _PilotDisable2;

  // call this periodically
  // checks if one of the radios is tuned to a station for which we do not have an ATIS
  void checkRadioAtis();

  XPLMHotKeyID	uiWindowHotKey;
  XPLMWindowID	console, focusDumper;

  ConnectForm& connectForm();
  DisconnectForm& disconnectForm();
  FlightplanForm& flightplanForm();
  FMCForm& fmcForm();
  XPMessageBox& messageBox();
  RefInspector& refInspector();
  IVAOSecurity betaBlocker;
  UiWindow uiWindow;
  MessageWindow msgWindow;
  Tcas Tcasbox; //added 16/10/2012
  std::vector<ChatWindow*> chatWindows;
  void closeChatWindow(ChatWindow* win);
  
  void sendMessageTo(const std::string& destination, const std::string& message);

  IdentMode xpdrModus() { return xpdrMode; };
  int getXpdrCode() { return xpdrCode; };
  void setXpdrCode(int code);
  bool xpdrIdent() { return  xpdrMode == IdentID || _identOverride; };
  bool radioCoupled() { return _radioCoupled; };
  int activeRadio() { return _activeRadio; };

  void setRadioCoupled(bool value);
  void setComActive(int radio);

  void tuneCom(int radio, int freq, pt::string name = "");

  void setXpdrIdent();
  void xpdrModeToggle();

  bool online() { return fsd.connected() && fsd.verified(); };

  bool usingWeather() { return _useWeather; };
  void setWeather(bool value);

  bool usingVoice() { return _useVoice; };
  void setVoice(bool value);

  bool usingPMSG() { return _usePMSG; };
  void setPMSG(bool value);

  bool UsingWindLayer() { return _WindLayer; };
  int  UsingTurbulance() { return _Turbulance; };
  int  UsingVisibility() { return _Maxvisibility; };

  bool saveCmdHistoryBetweenSessions() const {
	  return _saveCmdHistoryBetweenSessions;
  }
  bool modernKeyMapForCmdHistory() const {
	  return _modernKeyMapForCmdHistory;
  }

  void setLabels(bool value);
  void setClamping(bool value);
  bool usingLabels() { return _useLabels; };
  bool Clamping() { return _useClamping; };

  bool usingMultiplayer() { return _useMultiplayer; };
  void setMultiplayer(bool value);

  bool consoleVisible();
  void setConsoleVisible(bool value);

  bool autopilotOn() const;
  pt::string autopilotState() const;

  int fontWidth, fontHeight;

  std::vector<AtcPosition> getAtcPositions(SortOrder sort);

  // process a user input line
  void processLine(pt::string line);

  void dumpKeyboardFocus() { uiWindow.dumpKeyboardFocus(); };
  void grabKeyboardFocus() { uiWindow.grabKeyboardFocus(); };

  void handleCommand(pt::string line);

  //double frameTiming, oldFrameTiming, highResolutionTime;
  int lastNullFrameTime;
  //static const int frameTimingFrames = 15;

  const FSD::PlaneParams& updateParams();
  UInt32 getParams();

  int renderWindowWidth() const { return _renderWindowWidth; };
  int renderWindowHeight() const { return _renderWindowHeight; };

  pt::string realname, callsign;

  AircraftDB aircraftDB; //na deze stond ie
  AirlinesDB airlinesDB;

  // returns SVN revision number
  const pt::string& revision() const { return _revision; };

  double GetLat() const { return lat; };
  double GetLon() const { return lon; };
  float Getgroundalt() const { return agl*2.2f; }; //add for tcas
  float GetSpeed() const { return speed; };
  float GetPitch() const { return pitch; };
  float GetBank() const { return bank; };
  float GetHeading() const { return heading; };
  char simratio[30];
  bool posSent;

  void RequestP2P(const pt::string& callsign, const pt::string& protocol, int mode, const pt::string& ipport);
  void SendP2PReply(const pt::string& callsign, const pt::string& protocol, int mode, const pt::string& ipport);
  void Selcal(pt::string source);

  WristWatch watch;

  int p2pmode() const { return _multiplayer.p2pmode(); };

  int GetPlaneRes() { return _plane_resolution; };

  bool fmcar;
  pt::string com1name, com2name;
  pt::string& GetCOM1Name() { return com1name; };
  pt::string& GetCOM2Name() { return com2name; };
//#ifdef 



  bool _tsQuit; // set to true if TS is shuting down

//#endif
  WeatherGod _erwin;	//moved to public for use of weatherstation name

  // p2p parameters getters&setters
  void setP2p_enabled(bool inVal) {
	  _p2p_enabled = inVal;
  }
  bool p2p_enabled() const {
	  return _p2p_enabled;
  }
  void setP2p_forced_port(int inVal) {
	  _p2p_forced_port = inVal;
  }
  int p2p_forced_port() const {
	  return _p2p_forced_port;
  }
  void setP2p_max_sendbps(int inVal) {
	  _p2p_max_sendbps = inVal;
  }
  int p2p_max_sendbps() const {
	  return _p2p_max_sendbps;
  }
  void setP2p_max_recvbps(int inVal) {
	  _p2p_max_recvbps = inVal;
  }
  int p2p_max_recvbps() const {
	  return _p2p_max_recvbps;
  }
  void setDefault_icao(const pt::string & inVal) {
	  _default_icao = inVal;
  }
  const pt::string & default_icao() const {
	  return _default_icao;
  }

private:

  pt::string _lastPrivSender; // the callsign of the last station that sent a private message

  pt::string _acType, _acAirline, _acLivery; // MTL
  pt::string _deptAirport;
  pt::string _destAirport;
  pt::string _flightRoute;

  // Forms and Windows
  ConnectForm *_connectForm;
  DisconnectForm *_disconnectForm;
  FlightplanForm *_flightplanForm;
  bool _fplPrefiled, _fplSend;
  FMCForm *_fmcForm;
  XPMessageBox *_messageBox;
  RefInspector *_refInspector;

  // position packets
  XPLMDataRef gPlaneLat, gPlaneLon, gPlaneEl;
  XPLMDataRef gPlanePitch, gPlaneBank, gPlaneHeading, gPlaneSpeed, gPlaneAGL;
  XPLMDataRef gXpdrCode, gXpdrMode, gXpdrId;

  // panel integration
  XPLMDataRef gCom1, gCom2, gAudioSwitches, gBarometer, gIndicatedAlt;

  // autopilot
  XPLMDataRef gAPmode, gAPstate;

  // rendering window
  int _renderWindowWidth, _renderWindowHeight;

  // plane params
  int numEngines;
  XPLMDataRef gGearDeploy;
  XPLMDataRef gLightBeacon, gLightLanding, gLightNav, gLightStrobe, gLightTaxi;
  XPLMDataRef gEngineMode, gEngineThrust, gEngineRunning;
  XPLMDataRef gFlapsDeploy, gSpeedBrakes;
  XPLMDataRef gAcfTailnum, gAcfDescription;

  double _lastParams;
  FSD::PlaneParams _planeParams;	// current plane parameter flags
  void sendPlaneParams();

  void handleTextMessage(const FSD::Message& m);
  void handleInfoReply(const FSD::Message& m);
  void handleError(const FSD::Message& m);
  void handleKill(const FSD::Message& m);
  void handleWxReply(const FSD::Message& m);
  void handleAtisReply(const FSD::Message& m);

  // process FSDs weather feed
  void handleWxData(const FSD::Message& m);

  void addATC(const FSD::Message& m);
  void delATC(const FSD::Message& m);
  void updateATC(const FSD::Message& m);

  void handleFPchange(const FSD::Message& m);

  pt::string getActiveRadioFreq();
  int listeningToRadioFreq(pt::string freq);

  // converts a integer frequency into a string
  pt::string freq2str(int freq);

  // converts a integer frequency to atc station callsign if applicable
  pt::string freq2name(int freq);

  // status variables
  double lat, lon, el, groundalt;
  float pitch, bank, heading, speed, agl;
  int xpdrCode;
  IdentMode xpdrMode;
  bool _onground;

  int _activeRadio; // 1 or 2
  bool _radioCoupled; // true if listening to both
  int _com1freq, _com2freq;

  bool _identOverride;
  double _setXpdrIdOff;
  AtcList _atcList;
  bool isVoiceUrl(pt::string str);
  double _nextRadioAtisCheck;

  bool _useWeather;
  WxDB _weatherDB;

  WxStation currentWxStation;
  double _nextWxCheck, _nextWxTransition;
  void checkWeather(double elapsed);

  bool _saveCmdHistoryBetweenSessions;
  bool _modernKeyMapForCmdHistory;
  bool _useVoice;
  bool _useLabels;
  bool _useClamping;
  bool _usePMSG;
  bool _WindLayer;
  int	 _Maxvisibility;
  int	 _Turbulance;
#ifdef HAVE_TEAMSPEAK
  TeamSpeak tsRemote;
  bool _tsLoaded;
  bool _tsTuned;
#endif

  Multiplayer::MultiplayerEngine _multiplayer;
  bool _useMultiplayer;

  HTTP::HTTPClient _HTTPclient;
  bool _downloadingServerStatus;



  NavQuery _navQuery;

  pt::string _stun_server;
  bool _p2p_enabled;
  int _p2p_forced_port;
  int _p2p_max_sendbps, _p2p_max_recvbps;

  int _plane_resolution;
  pt::string _default_icao;
  pt::string _revision;
  int _xplane_version;

};

#endif
