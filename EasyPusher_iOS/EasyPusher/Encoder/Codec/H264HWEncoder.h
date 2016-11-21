//
//  H264HWEncoder.h
//  EasyCapture
//
//  Created by phylony on 9/11/16.
//  Copyright © 2016 phylony. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <VideoToolbox/VideoToolbox.h>
#import "H264Packet.h"

@protocol H264HWEncoderDelegate <NSObject>

@required
- (void)gotH264EncodedData:(NSData *)packet keyFrame:(BOOL)keyFrame timestamp:(CMTime)timestamp error:(NSError*)error;

@end

@interface H264HWEncoder : NSObject
{
    int     _spsppsFound;
    unsigned char *_EncoderBuffer;
}
- (void) invalidate;
- (void) setOutputSize:(CGSize)size;
- (void) encode:(CMSampleBufferRef )sampleBuffer;

@property (weak, nonatomic) id<H264HWEncoderDelegate> delegate;

@end
