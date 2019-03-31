/* 
 * File:   ASIOCore.cpp
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

#include "ASIOCore.h"

#include "asio.hpp"
#include <thread>
#include "XPLMUtilities.h"

asio::io_service    sASIOCoreIOService;

std::thread*        asioIOThread = 0;
bool                asioIOThreadContinue = false;

void ASIOCore_Initialize()
{
    if(!asioIOThread)
    {
        XPLMDebugString("Starting X-IvAp ASIO Core ...\n");
                
        asioIOThread = new std::thread([]() 
        {
            asioIOThreadContinue = true;
            
            while(asioIOThreadContinue)
            {
                sASIOCoreIOService.run();
                
                if(asioIOThreadContinue)
                {
                    sASIOCoreIOService.reset();
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                }
            }
            
            XPLMDebugString("ASIO Core IO Service Terminated.\n");
            
            sASIOCoreIOService.reset();
        });
                
        XPLMDebugString("ASIO Core started.\n");
    }
}

void ASIOCore_Shutdown()
{
    if(asioIOThread)
    {
        XPLMDebugString("Stopping ASIO Core.\n");
        
        asioIOThreadContinue = false;
        
        asioIOThread->join();
        
        delete asioIOThread;
        
        asioIOThread = 0;
        
        XPLMDebugString("ASIO Core stopped.\n");
    }
}