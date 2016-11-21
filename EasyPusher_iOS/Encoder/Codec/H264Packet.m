//
//  H264Packet.m
//  EasyCapture
//
//  Created by phylony on 9/11/16.
//  Copyright © 2016 phylony. All rights reserved.
//

#import "H264Packet.h"

@interface H264Packet ()
{
}

@end

@implementation H264Packet

- (id)initWithCMSampleBuffer:(CMSampleBufferRef)sample
{
    self = [super init];
    if(self)
    {
        [self packetizeAVC:sample];
    }
    return self;
}

- (void)packetizeAVC:(CMSampleBufferRef)sample
{
    self.packet = [NSMutableData data];
    
    NSData *sps, *pps;
    
    CFDictionaryRef ref = (CFDictionaryRef)CFArrayGetValueAtIndex(CMSampleBufferGetSampleAttachmentsArray(sample, true), 0);
    // 判断当前帧是否为关键帧
    bool keyframe = !CFDictionaryContainsKey(ref , kCMSampleAttachmentKey_NotSync);
    
    int resultLen = 0;
    if (keyframe)
    {
        CMFormatDescriptionRef format = CMSampleBufferGetFormatDescription(sample);
        
        size_t sparameterSetSize, sparameterSetCount;
        const uint8_t *sparameterSet;
        OSStatus statusCode = CMVideoFormatDescriptionGetH264ParameterSetAtIndex(format, 0, &sparameterSet, &sparameterSetSize, &sparameterSetCount, 0 );
        if (statusCode == noErr)
        {
            // Found sps and now check for pps
            size_t pparameterSetSize, pparameterSetCount;
            const uint8_t *pparameterSet;
            OSStatus statusCode = CMVideoFormatDescriptionGetH264ParameterSetAtIndex(format, 1, &pparameterSet, &pparameterSetSize, &pparameterSetCount, 0 );
            if (statusCode == noErr)
            {
                // Found pps
                sps = [NSData dataWithBytes:sparameterSet length:sparameterSetSize];
                pps = [NSData dataWithBytes:pparameterSet length:pparameterSetSize];
                
                const char bytes[] = {0, 0, 0, 1}; // SPS PPS Header
                NSData *byteHeader = [NSData dataWithBytes:bytes length:4];
                NSMutableData *fullSPSData = [NSMutableData dataWithData:byteHeader];
                NSMutableData *fullPPSData = [NSMutableData dataWithData:byteHeader];
                
                [fullSPSData appendData:sps];
                [fullPPSData appendData:pps];
                
                sps = fullSPSData;
                pps = fullPPSData;
                
                [self.packet appendData:sps];
                [self.packet appendData:pps];
                
                resultLen += 8 + sparameterSetSize + pparameterSetSize;
            }
        }
    }

    CMBlockBufferRef dataBuffer = CMSampleBufferGetDataBuffer(sample);
    size_t totalLength;
    char *dataPointer;
    OSStatus statusCodeRet = CMBlockBufferGetDataPointer(dataBuffer, 0, NULL, &totalLength, &dataPointer);
    if (statusCodeRet == noErr) {
        
        size_t bufferOffset = 0;
        static const int AVCCHeaderLength = 4;
        while (bufferOffset < totalLength - AVCCHeaderLength) {
            // Read the NAL unit length
            uint32_t NALUnitLength = 0;
            memcpy(&NALUnitLength, dataPointer + bufferOffset, AVCCHeaderLength);
            
            // Convert the length value from Big-endian to Little-endian
            NALUnitLength = CFSwapInt32BigToHost(NALUnitLength);
            
            NSData* data = [[NSData alloc] initWithBytes:(dataPointer + bufferOffset + AVCCHeaderLength) length:NALUnitLength];
            
            const char bytes[] = {0, 0, 0, 1}; // SPS PPS Header
            NSData *byteHeader = [NSData dataWithBytes:bytes length:4];
            NSMutableData *fullAVCData = [NSMutableData dataWithData:byteHeader];
            [fullAVCData appendData:data];
        
            [self.packet appendData:fullAVCData];
            
            // Move to the next NAL unit in the block buffer
            bufferOffset += AVCCHeaderLength + NALUnitLength;
            resultLen += AVCCHeaderLength + NALUnitLength;
        }
    }
    self.keyFrame = keyframe;
}

@end
