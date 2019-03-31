/* 
 * File:   ASIOCore.h
 * Author: christophe.delondre
 *
 * License: see LICENSE.xivap
 */

#ifndef ASIOCORE_H
#define ASIOCORE_H

#include "asio.hpp"

extern asio::io_service sASIOCoreIOService;

void ASIOCore_Initialize();
void ASIOCore_Shutdown();

#endif /* ASIOCORE_H */

