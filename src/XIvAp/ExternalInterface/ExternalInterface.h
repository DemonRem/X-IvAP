/* 
 * File:   ExternalInterface.h
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

#ifndef EXTERNALINTERFACE_H
#define EXTERNALINTERFACE_H

#include "ExternalInterfaceServer.h"

void ExternalInterface_Start();
void ExternalInterface_Stop();

extern ExternalInterfaceServer* pExternalInterfaceServer;

#endif /* EXTERNALINTERFACE_H */

