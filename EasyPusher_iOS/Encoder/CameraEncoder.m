//
//  CameraEncoder.m
//  EasyCapture
//
//  Created by phylony on 9/11/16.
//  Copyright © 2016 phylony. All rights reserved.
//
#import "CameraEncoder.h"


#define KEY "6A34714D6C354F576B5971414A553558714C485A4576466C59584E356348567A6147567958334E6B61395A58444661672F704C67523246326157346D516D466962334E68514449774D545A4659584E355247467964326C75564756686257566863336B3D"

char* ConfigIP		= "121.40.50.44";	//Default EasyDarwin Address
char* ConfigPort	= "554";			//Default EasyDarwin Port
char* ConfigName	= "ios11.sdp";//Default Push StreamName
char* ConfigUName	= "admin";			//SDK UserName
char* ConfigPWD		= "admin";			//SDK Password
char* ConfigDHost	= "192.168.66.189";	//SDK Host
char* ConfigDPort	= "80";				//SDK Port
char *ProgName;		//Program Name

@interface CameraEncoder ()
{
    H264HWEncoder *h264Encoder;

    AACEncoder *aacEncoder;
    Easy_I32 isActivated;
    Easy_Pusher_Handle handle;
    
    dispatch_queue_t encodeQueue;
    
//    NSString *h264File;
//    NSString *aacFile;
//    NSFileHandle *fileH264Handle;
//    NSFileHandle *fileAACHandle;
}

@end

@implementation CameraEncoder
@synthesize running;

- (void)initCameraWithOutputSize:(CGSize)size
{
    h264Encoder = [[H264HWEncoder alloc] init];
    [h264Encoder setOutputSize:size];
    h264Encoder.delegate = self;
    
#if TARGET_OS_IPHONE
    aacEncoder = [[AACEncoder alloc] init];
    aacEncoder.delegate = self;
#endif
    
    running = NO;
    
    handle = EasyPusher_Create();
    _encodeVideoQueue = dispatch_queue_create( "encodeVideoQueue", DISPATCH_QUEUE_SERIAL );
    _encodeAudioQueue = dispatch_queue_create( "encodeAudioQueue", DISPATCH_QUEUE_SERIAL );
    CMSimpleQueueCreate(kCFAllocatorDefault, 2, &vbuffQueue);
    CMSimpleQueueCreate(kCFAllocatorDefault, 2, &abuffQueue);
    _videoCaptureSession = [[AVCaptureSession alloc] init];
    [self setupAudioCapture];
    [self setupVideoCapture];
    
    encodeQueue = dispatch_queue_create("encodeQueue", NULL);
}

- (void)dealloc {
#if TARGET_OS_IPHONE
    [h264Encoder invalidate];
#endif
    running = NO;
}

#pragma mark - Camera Control


- (void)setupAudioCapture
{

    AVCaptureDevice *audioDevice = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeAudio];
    
    NSError *error = nil;
    
    AVCaptureDeviceInput *audioInput = [[AVCaptureDeviceInput alloc]initWithDevice:audioDevice error:&error];
    
    if (error) {
        
        NSLog(@"Error getting audio input device:%@",error.description);
    }
    
    if ([self.videoCaptureSession canAddInput:audioInput]) {
        
        [self.videoCaptureSession addInput:audioInput];
    }
    
    self.AudioQueue = dispatch_queue_create("Audio Capture Queue", DISPATCH_QUEUE_SERIAL);
    
    AVCaptureAudioDataOutput *audioOutput = [AVCaptureAudioDataOutput new];
    [audioOutput setSampleBufferDelegate:self queue:self.AudioQueue];
    
    if ([self.videoCaptureSession canAddOutput:audioOutput]) {
        
        [self.videoCaptureSession addOutput:audioOutput];
    }
    
    self.audioConnection = [audioOutput connectionWithMediaType:AVMediaTypeAudio];
}

#pragma mark - 设置视频 capture  3
- (void)setupVideoCapture {
    
    if ([self.videoCaptureSession canSetSessionPreset:AVCaptureSessionPreset1280x720]) {
        // 设置分辨率
        self.videoCaptureSession.sessionPreset = AVCaptureSessionPreset1280x720;
    }
    //
    //设置采集的 Video 和 Audio 格式，这两个是分开设置的，也就是说，你可以只采集视频。
    //配置采集输入源(摄像头)
    
    NSError *error = nil;
    //获得一个采集设备, 例如前置/后置摄像头
    AVCaptureDevice *videoDevice = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];
    
    //    videoDevice = [self cameraWithPosition:AVCaptureDevicePositionBack];
    //   videoDevice.position = AVCaptureDevicePositionBack;
    //用设备初始化一个采集的输入对象
    AVCaptureDeviceInput *videoInput = [AVCaptureDeviceInput deviceInputWithDevice:videoDevice error:&error];
    if (error) {
        NSLog(@"Error getting video input device:%@",error.description);
        
    }
    if ([self.videoCaptureSession canAddInput:videoInput]) {
        [self.videoCaptureSession addInput:videoInput];
    }
    
    //配置采集输出,即我们取得视频图像的接口
    _videoQueue = dispatch_queue_create("Video Capture Queue", DISPATCH_QUEUE_SERIAL);
    _videoOutput = [AVCaptureVideoDataOutput new];
    [_videoOutput setSampleBufferDelegate:self queue:_videoQueue];
    
    // 配置输出视频图像格式
    NSDictionary *captureSettings = @{(NSString*)kCVPixelBufferPixelFormatTypeKey: @(kCVPixelFormatType_32BGRA)};
    _videoOutput.videoSettings = captureSettings;
    _videoOutput.alwaysDiscardsLateVideoFrames = YES;
    
    if ([self.videoCaptureSession canAddOutput:_videoOutput]) {
        [self.videoCaptureSession addOutput:_videoOutput];
    }
    
    // 设置采集图像的方向,如果不设置，采集回来的图形会是旋转90度的
    _videoConnection = [_videoOutput connectionWithMediaType:AVMediaTypeVideo];
    _videoConnection.videoOrientation = AVCaptureVideoOrientationPortrait;
    
    
    // 保存Connection,用于SampleBufferDelegate中判断数据来源(video or audio?)
    _videoConnection = [_videoOutput connectionWithMediaType:AVMediaTypeVideo];
    
    
    
    //将当前硬件采集视频图像显示到屏幕
    // 添加预览
    self.previewLayer = [AVCaptureVideoPreviewLayer    layerWithSession:self.videoCaptureSession];
    [self.previewLayer setVideoGravity:AVLayerVideoGravityResizeAspect];
}

- (void)startCapture
{
    [self.videoCaptureSession startRunning];
}

- (void) startCamera
{
    EASY_MEDIA_INFO_T mediainfo;
    memset(&mediainfo, 0, sizeof(EASY_MEDIA_INFO_T));
    mediainfo.u32VideoCodec = EASY_SDK_VIDEO_CODEC_H264;
    mediainfo.u32VideoFps = 25;
    mediainfo.u32AudioCodec = EASY_SDK_AUDIO_CODEC_AAC;//SDK output Audio PCMA
    mediainfo.u32AudioSamplerate = 44100;
    mediainfo.u32AudioChannel = 2;
    mediainfo.u32AudioBitsPerSample = 16;
    EasyPusher_StartStream(handle, ConfigIP, atoi(ConfigPort), ConfigName, "admin", "admin", &mediainfo, 0, false);//1M缓冲区
    running = YES;
}

- (void) stopCamera
{
    running = NO;
    [h264Encoder invalidate];
    EasyPusher_StopStream(handle);
}

-(void) captureOutput:(AVCaptureOutput*)captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection*)connection
{
    CFRetain(sampleBuffer);
    if(connection == self.videoConnection)
    {
        if (running)
        {
            dispatch_async(encodeQueue, ^{
                
                [h264Encoder encode:sampleBuffer];
                CFRelease(sampleBuffer);
            });
        }

    }
    else if(connection == self.audioConnection)
    {
        if (running)
        {
            dispatch_async(encodeQueue, ^{
                
                [aacEncoder encode:sampleBuffer];
                CFRelease(sampleBuffer);
            });
        }
        
    }
    
    if (!running)
    {
        CFRelease(sampleBuffer);
    }
}

- (void)captureOutput:(AVCaptureOutput *)captureOutput didDropSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection
{
    NSLog(@"drop frame");
}

#pragma mark -  H264HWEncoderDelegate declare

- (void)gotH264EncodedData:(NSData *)packet keyFrame:(BOOL)keyFrame timestamp:(CMTime)timestamp error:(NSError*)error
{
//    NSLog(@"gotH264EncodedData %d", (int)[packet length]);
//    
//    [fileH264Handle writeData:packet];
    
//    if(isReadyVideo && isReadyAudio) [rtp_h264 publish:packet timestamp:timestamp payloadType:98];
    
    CGFloat secs = CMTimeGetSeconds(timestamp);
    UInt32 uSecs = (secs - (int)secs) * 1000 * 1000;

    EASY_AV_Frame frame;
    frame.pBuffer=(void*)packet.bytes;
    frame.u32AVFrameFlag=EASY_SDK_VIDEO_FRAME_FLAG;
    frame.u32AVFrameLen=(Easy_U32)packet.length;
    frame.u32TimestampSec = secs; //(Easy_U32)timestamp.value/timestamp.timescale;
    frame.u32TimestampUsec = uSecs;//timestamp.value%timestamp.timescale/1000;å
    
    frame.u32VFrameType= keyFrame ? EASY_SDK_VIDEO_FRAME_I : EASY_SDK_VIDEO_FRAME_P;
    
    if(running)
    {
        EasyPusher_PushFrame(handle, &frame);
    }//[publish:packet timestamp:timestamp payloadType:98];
}

#if TARGET_OS_IPHONE
#pragma mark - AACEncoderDelegate declare

- (void)gotAACEncodedData:(NSData *)data timestamp:(CMTime)timestamp error:(NSError*)error
{
//    NSLog(@"gotAACEncodedData %d", (int)[data length]);
//
//    if (fileAACHandle != NULL)
//    {
//        [fileAACHandle writeData:data];
//    }

//    if(isReadyVideo && isReadyAudio) [rtp_aac publish:data timestamp:timestamp payloadType:97];
    CGFloat secs = CMTimeGetSeconds(timestamp);
    UInt32 uSecs = (secs - (int)secs) * 1000 * 1000;
    
    EASY_AV_Frame frame;
    frame.pBuffer=(void*)[data bytes];
    frame.u32AVFrameLen = (Easy_U32)[data length];
    frame.u32VFrameType = EASY_SDK_AUDIO_CODEC_AAC;
    frame.u32AVFrameFlag=EASY_SDK_AUDIO_FRAME_FLAG;
   
    frame.u32TimestampSec= secs;//(Easy_U32)timestamp.value/timestamp.timescale;
    frame.u32TimestampUsec = uSecs;//timestamp.value%timestamp.timescale;
    if(running)
    {
        EasyPusher_PushFrame(handle,&frame);
    }//[rtp_aac publish:data timestamp:timestamp payloadType:97];

}
#endif


@end
