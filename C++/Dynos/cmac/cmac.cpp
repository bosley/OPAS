/*
 * The MIT License
 *
 * Copyright 2018 Josh A. Bosley.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
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
 */

/* 
 * File:   Blocker.h
 * Author: Josh A. Bosley
 */

#include "cmac.h"

#ifdef _WIN32
#include <Winsock2.h>
WSADATA wsaData;
void InitNetworking() {
	// Init Winsock2 //
	WSAStartup( MAKEWORD(2,2), &wsaData );
	atexit( (void(*)())WSACleanup );
}
#else // _WIN32 //
void InitNetworking() { }
#endif // _WIN32 //

namespace macman{

bool GetPrimaryMac(char *macaddr)
{
    InitNetworking();
	{
		pNetAdapterInfo* Adapters = new_pNetAdapterInfo();	
		const NetAdapterInfo* Current = get_primary_pNetAdapterInfo( Adapters );	
		if ( Current ) {
            strncpy(macaddr, Current->MAC, MAC_ADDRESS_LENGTH);
		    delete_pNetAdapterInfo( Adapters );		
            return true;
		}
		else {
		    delete_pNetAdapterInfo( Adapters );		
			return false;
		}								
	}
}

bool GetIp(char *ip)
{
    InitNetworking();
	{
		pNetAdapterInfo* Adapters = new_pNetAdapterInfo();	
		const NetAdapterInfo* Current = get_primary_pNetAdapterInfo( Adapters );	
		if ( Current ) {
            strncpy(ip, Current->IP, IP_ADDRESS_LENGTH);
		    delete_pNetAdapterInfo( Adapters );		
            return true;
		}
		else {
		    delete_pNetAdapterInfo( Adapters );		
			return false;
		}								
	}
}

}