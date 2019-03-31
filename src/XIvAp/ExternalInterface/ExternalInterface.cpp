/* 
 * File:   ExternalInterface.cpp
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

#include "ExternalInterface.h"
#include "ExternalInterfaceServer.h"
#include "ExternalInterfaceController.h"
#include "DataInterface.h"

#include "XPLMUtilities.h"

ExternalInterfaceServer*     pExternalInterfaceServer = 0;

void ExternalInterface_Start()
{
    XPLMDebugString("Starting X-IvAp External Interface ...\n");
    
    // Create the data interface for clients to retrieve data.
    sDataInterface = new DataInterface();
    
    // Create the controller for clients to control the application.
    sExternalInterfaceController = new ExternalInterfaceController();
    
    // Create and start the server.
    pExternalInterfaceServer = new ExternalInterfaceServer("0.0.0.0", 4827);
    pExternalInterfaceServer->Start();
    
    // Start the data interface watch thread.
    sDataInterface->StartWatchThread();
    
    XPLMDebugString("External Interface started.\n");
}

void ExternalInterface_Stop()
{
    XPLMDebugString("Stopping X-IvAp External Interface ...\n");
    
    if(pExternalInterfaceServer)
    {
        // Stop the server and close client connections.
        pExternalInterfaceServer->Stop();

        // Destroy server.    
        delete pExternalInterfaceServer;    
        pExternalInterfaceServer = 0;
    }
    
    if(sExternalInterfaceController)
    {
        delete sExternalInterfaceController;
        sExternalInterfaceController = 0;
    }
    
    if(sDataInterface)
    {
        sDataInterface->StopWatchThread();
        
        // Destroy data interface.
        delete sDataInterface;
        sDataInterface = 0;
    }
    
    XPLMDebugString("External Interface stopped.\n");
}