//
//  ViewController.m
//  EasyCapture
//
//  Created by phylony on 9/11/16.
//  Copyright © 2016 phylony. All rights reserved.
//

#import "ViewController.h"
#import "PureLayout.h"

@interface ViewController ()
{
    UIButton *startButton;
    UIButton *settingButton;
}
@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    encoder = [[CameraEncoder alloc] init];
    
    [encoder initCameraWithOutputSize:CGSizeMake(480, 640)];
    
    encoder.previewLayer.frame = self.view.bounds;
    [self.view.layer addSublayer:encoder.previewLayer];
    AVCaptureVideoPreviewLayer *prev = encoder.previewLayer;
    [[prev connection] setVideoOrientation:AVCaptureVideoOrientationPortrait];
    prev.frame = self.view.bounds;
    encoder.previewLayer.hidden = NO;
    [encoder startCapture];

    startButton = [UIButton buttonWithType:UIButtonTypeCustom];
    startButton.translatesAutoresizingMaskIntoConstraints = NO;
    [self.view addSubview:startButton];
    [startButton setTitle:@"开始推流" forState:UIControlStateNormal];
    [startButton autoPinEdgeToSuperviewEdge:ALEdgeLeading withInset:20.0];
    [startButton autoPinEdgeToSuperviewEdge:ALEdgeBottom withInset:20.0];
    [startButton autoSetDimension:ALDimensionWidth toSize:80];
    [startButton autoSetDimension:ALDimensionHeight toSize:40];
    startButton.backgroundColor = [[UIColor blackColor] colorWithAlphaComponent:0.7];
    [startButton addTarget:self action:@selector(startAction:) forControlEvents:UIControlEventTouchUpInside];
    
    settingButton = [UIButton buttonWithType:UIButtonTypeCustom];
    settingButton.translatesAutoresizingMaskIntoConstraints = NO;
    [self.view addSubview:settingButton];
    [settingButton setTitle:@"设置" forState:UIControlStateNormal];
    [settingButton autoPinEdge:ALEdgeLeading toEdge:ALEdgeTrailing ofView:startButton withOffset:10.0];
    [settingButton autoPinEdgeToSuperviewEdge:ALEdgeBottom withInset:20.0];
    [settingButton autoSetDimension:ALDimensionWidth toSize:80];
    [settingButton autoSetDimension:ALDimensionHeight toSize:40];
    settingButton.backgroundColor = [[UIColor blackColor] colorWithAlphaComponent:0.7];
    [settingButton addTarget:self action:@selector(settingAction:) forControlEvents:UIControlEventTouchUpInside];
}

- (IBAction)startAction:(id)sender
{
    if (!encoder.running)
    {
        [startButton setTitle:@"停止推流" forState:UIControlStateNormal];
        [encoder startCamera];
    }
    else
    {
        [startButton setTitle:@"开始推流" forState:UIControlStateNormal];
        [encoder stopCamera];
    }
}

- (IBAction)settingAction:(id)sender
{
    
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

@end
