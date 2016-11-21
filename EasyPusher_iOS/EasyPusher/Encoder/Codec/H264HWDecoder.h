//
//  H264HWDecoder.h
//  EasyCapture
//
//  Created by phylony on 9/11/16.
//  Copyright © 2016 phylony. All rights reserved.
//
#import <Foundation/Foundation.h>
#import <VideoToolbox/VideoToolbox.h>

@protocol H264HWDecoderDelegate <NSObject>

- (void)displayDecodedFrame:(CVImageBufferRef)imageBuffer;

@end

@interface H264HWDecoder : NSObject

@property (weak, nonatomic) id<H264HWDecoderDelegate> delegate;

-(void) receivedRawVideoFrame:(uint8_t *)frame withSize:(uint32_t)frameSize isIFrame:(int)isIFrame;

@end
