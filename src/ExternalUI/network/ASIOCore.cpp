/* 
 * File:   ASIOCore.cpp
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

#include "ASIOCore.h"

#include "asio.hpp"
#include <thread>
#include <QDebug>

asio::io_service    sASIOCoreIOService;

std::thread*        asioIOThread = 0;
bool                asioIOThreadContinue = false;

void ASIOCore_Initialize()
{
    if(!asioIOThread)
    {
        qDebug() << "Starting X-IvAp ASIO Core ...\n";
                
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
            
            qDebug() << "ASIO Core IO Service Terminated.\n";
            
            sASIOCoreIOService.reset();
        });
                
        qDebug() << "ASIO Core started.\n";
    }
}

void ASIOCore_Shutdown()
{
    if(asioIOThread)
    {
        qDebug() << "Stopping ASIO Core.\n";
        
        asioIOThreadContinue = false;
        
        asioIOThread->join();
        
        delete asioIOThread;
        
        asioIOThread = 0;
        
        qDebug() << "ASIO Core stopped.\n";
    }
}
