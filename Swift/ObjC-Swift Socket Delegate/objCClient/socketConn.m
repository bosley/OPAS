//
//  socketConn.m
//  objCClient
//
//  Created by Josh Allen Bosley on 10/31/14.
//  Copyright (c) 2014 Lssu. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "socketConn.h"

@implementation socketClass

-(void)initializeSocketClass
{
    server = @"tcp://107.170.205.216";
    port = 5002;
}

-(void)connectToServer
{
    arrResults  = [NSMutableArray array];
    NSURL *website = [NSURL URLWithString:server];
    CFStreamCreatePairWithSocketToHost(NULL, (__bridge CFStringRef)[website host], port, &readStream, &writeStream);
    inputStream = (__bridge  NSInputStream *)readStream;
    outputStream = (__bridge NSOutputStream *)writeStream;
    [inputStream setDelegate:self];
    [outputStream setDelegate:self];
    [inputStream scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    [outputStream scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
    [inputStream open];
    [outputStream open];
}

-(void)sendRequestForDevices
{
    _data = [NSData dataWithBytes: "s^ad" length:5];
    [(NSOutputStream *)outputStream write:[_data bytes] maxLength:[_data length]];
}

-(void)sendRequestForUpdates
{
    _data = [NSData dataWithBytes: "r^up" length:5];
    [(NSOutputStream *)outputStream write:[_data bytes] maxLength:[_data length]];
}

-(NSMutableArray*)getAllRetrievedData
{
    return arrResults;
}
 
- (void)stream:(NSStream *)theStream handleEvent:(NSStreamEvent)streamEvent
{
    switch (streamEvent)
    {
        case NSStreamEventHasBytesAvailable:
        {
            if (theStream == inputStream)
            {
                NSInteger len;
                uint8_t buffer[1024];
                
                while ([inputStream hasBytesAvailable])
                {
                    len = [inputStream read:buffer maxLength:sizeof(buffer)];
                    if (len > 0)
                    {
                        NSString *output = [[NSString alloc] initWithBytes:buffer length:len encoding:NSASCIIStringEncoding];
                        
                        if (nil != output)
                        {
                            NSLog(@"server said: %@", output);
                            [arrResults addObject: output];
                        }
                    }
                }
            }
            break;
        }
        case NSStreamEventHasSpaceAvailable:
        {
            // Sending Handled in seperate method
            break;
        }
        case NSStreamEventOpenCompleted:
        {
            NSLog(@"open complete: %@", theStream);
            
            break;
        }
        case NSStreamEventErrorOccurred:
        {
            NSLog(@"error occurred: %@", theStream);
            
            break;
        }
        case NSStreamEventEndEncountered:
        {
            NSLog(@"end encountered: %@", theStream);
            
            break;
        }
        default:
            break;
    }
}

@end
