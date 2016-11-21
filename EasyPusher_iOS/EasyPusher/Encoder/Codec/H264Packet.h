//
//  H264Packet.h
//  EasyCapture
//
//  Created by phylony on 9/11/16.
//  Copyright © 2016 phylony. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <VideoToolbox/VideoToolbox.h>

@interface H264Packet : NSObject

@property (strong, nonatomic) NSMutableData *packet;
@property (assign, nonatomic) BOOL keyFrame;
- (id)initWithCMSampleBuffer:(CMSampleBufferRef)sample;

- (void)packetizeAVC:(CMSampleBufferRef)sample;

@end
