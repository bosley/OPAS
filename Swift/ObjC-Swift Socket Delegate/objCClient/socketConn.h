//
//  socketConn.h
//  objCClient
//
//  Created by Josh Allen Bosley on 10/31/14.
//  Copyright (c) 2014 Lssu. All rights reserved.
//

#ifndef objCClient_socketConn_h
#define objCClient_socketConn_h

#import <Foundation/NSObject.h>

@interface socketClass:  NSOperation<NSStreamDelegate>
{
    NSString *server;
    NSData *_data;
    CFReadStreamRef readStream;
    CFWriteStreamRef writeStream;
    NSInputStream *inputStream;
    NSOutputStream *outputStream;
    NSMutableArray *arrResults;
    int port;
}

-(void)initializeSocketClass;
-(void)connectToServer;
-(void)sendRequestForDevices;
-(void)sendRequestForUpdates;
-(NSMutableArray*)getAllRetrievedData;
@end

#endif
