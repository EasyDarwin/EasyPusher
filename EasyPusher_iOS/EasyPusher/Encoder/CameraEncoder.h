//
//  CameraEncoder.h
//  EasyCapture
//
//  Created by phylony on 9/11/16.
//  Copyright © 2016 phylony. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import <CoreMedia/CoreMedia.h>

#import "H264HWEncoder.h"

#import <UIKit/UIKit.h>
#import "AACEncoder.h"
#import "EasyPusherAPI.h"


@interface CameraEncoder : NSObject <AVCaptureVideoDataOutputSampleBufferDelegate, AVCaptureAudioDataOutputSampleBufferDelegate, H264HWEncoderDelegate, AACEncoderDelegate>
{
    dispatch_queue_t _encodeVideoQueue;
    dispatch_queue_t _encodeAudioQueue;
    
    CMSimpleQueueRef vbuffQueue;
    CMSimpleQueueRef abuffQueue;
    BOOL running;
}
@property(nonatomic, assign)BOOL running;

@property (strong, nonatomic) AVCaptureVideoPreviewLayer *previewLayer;
@property (nonatomic, strong) dispatch_queue_t           videoQueue;
@property (nonatomic, strong) dispatch_queue_t           AudioQueue;
// 负责从 AVCaptureDevice 获得输入数据
@property (nonatomic, strong) AVCaptureDeviceInput       *captureDeviceInput;
@property (nonatomic, strong) AVCaptureVideoDataOutput   *videoOutput;
@property (nonatomic, strong) AVCaptureConnection        *videoConnection;
@property (nonatomic, strong) AVCaptureConnection        *audioConnection;
@property (nonatomic, strong)AVCaptureSession           *videoCaptureSession;

- (void) initCameraWithOutputSize:(CGSize)size;
- (void)startCapture;
- (void) startCamera;
- (void) stopCamera;

@end

