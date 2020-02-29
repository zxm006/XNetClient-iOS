//
//  JLView.m
//  View的拖拽
//
//  Created by Mac on 15-3-29.
//  Copyright (c) 2015年 vxinyou. All rights reserved.
//

#import "RtmpTouchMoveView.h"

#import <AVFoundation/AVFoundation.h>
#define WIDTH self.view.frame.size.width
#define HEIGHT self.view.frame.size.height
#define kScreenWidth [[UIScreen mainScreen] bounds].size.width
#define kScreenHeight [[UIScreen mainScreen] bounds].size.height

#define kWidth  100
#define kHeight 100

@interface RtmpTouchMoveView ()<AVCaptureVideoDataOutputSampleBufferDelegate>
{}
@property (strong,nonatomic) AVCaptureSession *captureSession;//负责输入和输出设置之间的数据传递
@property (strong,nonatomic)AVCaptureVideoDataOutput *videoOutput;
@property (strong,nonatomic) AVCaptureVideoPreviewLayer *captureVideoPreviewLayer;//相机拍摄预览图层

@end

@implementation RtmpTouchMoveView

- (id)init
{
    if(self = [super init])
        {
            
        }
        return self;
}
- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(nullable UIEvent *)event
{
      // 当前触摸点
      CGPoint center = self.center;
  
    if ( center.x<kWidth/2) {
        center.x =kWidth/2;
    }
    if ( center.y<kHeight/2) {
        center.y =kHeight/2;
    }
    
    if ( center.x>kScreenWidth-kWidth/2) {
        center.x =kScreenWidth-kWidth/2;
    }
    if ( center.y>kScreenHeight-kHeight/2) {
        center.y =kScreenHeight-kHeight/2;
    }
    
    
    
    // 修改当前view的中点(中点改变view的位置就会改变)
    self.center = center;
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event{
    
    UITouch *touch = [touches anyObject];
    
    // 当前触摸点
    CGPoint currentPoint = [touch locationInView:self.superview];
    // 上一个触摸点
    CGPoint previousPoint = [touch previousLocationInView:self.superview];
    
    // 当前view的中点
    CGPoint center = self.center;
    
    center.x += (currentPoint.x - previousPoint.x);
    center.y += (currentPoint.y - previousPoint.y);
    
  // 修改当前view的中点(中点改变view的位置就会改变)
    self.center = center;
    
    
}

-(AVCaptureDevice *)getCameraDeviceWithPosition:(AVCaptureDevicePosition )position{
    NSArray *cameras= [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];
    for (AVCaptureDevice *camera in cameras) {
        if ([camera position]==position) {
            return camera;
        }
    }
    return nil;
}

- (void)stopPreview{
    if(_captureSession){
        if([_captureSession isRunning]){
            [_captureSession stopRunning];
           usleep(5000);
        }
    }
    [self removeFromSuperview];
}

- (void)startPreview{
    
    [self startCAP];
}

- (void)reOrientation
{
 
    AVCaptureConnection *videoConnection = [_videoOutput connectionWithMediaType:AVMediaTypeVideo];
    // SET THE ORIENTATION HERE -------------------------------------------------
    
    
    UIInterfaceOrientation  tion=  [[UIApplication sharedApplication] statusBarOrientation];
    
    if (tion == UIInterfaceOrientationLandscapeLeft )
    {
        
        [videoConnection setVideoOrientation:AVCaptureVideoOrientationLandscapeLeft];
    }
    else if (tion == UIInterfaceOrientationLandscapeRight )
    {
        [videoConnection setVideoOrientation:AVCaptureVideoOrientationLandscapeRight];
        
    }
    else
    {
        [videoConnection setVideoOrientation:AVCaptureVideoOrientationPortrait];
    }

}

- (void)startCAP
{
    if(_captureSession )
    {
        [_captureSession stopRunning];
        _captureSession =nil;
    }
    
    _captureSession=[[AVCaptureSession alloc]init];
    if ([_captureSession canSetSessionPreset:AVCaptureSessionPreset352x288]) {//设置分辨率
        _captureSession.sessionPreset=AVCaptureSessionPreset352x288;
    }
    //获得输入设备
    AVCaptureDevice *captureDevice=[self getCameraDeviceWithPosition:AVCaptureDevicePositionFront];//取得后置摄像头
    if (!captureDevice) {
        NSLog(@"取得后置摄像头时出现问题.");
        return;
    }
    NSError *error = nil;
    AVCaptureDeviceInput *videoInput = [AVCaptureDeviceInput deviceInputWithDevice:captureDevice error:&error];
    if (!videoInput)
    {
        NSLog(@"Failed to get video input");
        
        return;
    }
    [ _captureSession addInput:videoInput];
    
    [  _captureSession beginConfiguration];
    [ captureDevice lockForConfiguration:&error];
    
    
    [captureDevice setActiveVideoMinFrameDuration:CMTimeMake(1, 12)];
    [captureDevice setActiveVideoMaxFrameDuration:CMTimeMake(1, 15)];
    
    [captureDevice unlockForConfiguration];
    [ _captureSession commitConfiguration];
    
    
   _videoOutput =  [[AVCaptureVideoDataOutput alloc] init]  ;
    _videoOutput.videoSettings = [[NSDictionary alloc] initWithObjectsAndKeys:
                                 [NSNumber numberWithUnsignedInt:kCVPixelFormatType_32BGRA], kCVPixelBufferPixelFormatTypeKey,nil];
    
    _videoOutput.alwaysDiscardsLateVideoFrames = YES;

    
    
    dispatch_queue_t queue = dispatch_queue_create("video.capture.queue", NULL);
 
    [_videoOutput setSampleBufferDelegate: self queue: queue];
    [_captureSession addOutput:_videoOutput];
    
    
    
    AVCaptureConnection *videoConnection = [_videoOutput connectionWithMediaType:AVMediaTypeVideo];
    // SET THE ORIENTATION HERE -------------------------------------------------
    
    
    UIInterfaceOrientation  tion=  [[UIApplication sharedApplication] statusBarOrientation];
    
    if (tion == UIInterfaceOrientationLandscapeLeft )
    {
        
        [videoConnection setVideoOrientation:AVCaptureVideoOrientationLandscapeLeft];
    }
    else if (tion == UIInterfaceOrientationLandscapeRight )
    {
        [videoConnection setVideoOrientation:AVCaptureVideoOrientationLandscapeRight];
        
    }
    else
    {
       [videoConnection setVideoOrientation:AVCaptureVideoOrientationPortrait];
    }
 
    if(![_captureSession isRunning]){
        [_captureSession startRunning];
    }
}

- (void)stopCAP
{
    if(self.captureSession )
    {
        [self.captureSession stopRunning];
        self.captureSession =nil;
    }
}



- (void)clean
{
    [self removeFromSuperview];
}
 
- (UIImage *) imageFromSampleBuffer:(CMSampleBufferRef) sampleBuffer
{
    // Get a CMSampleBuffer's Core Video image buffer for the media data
    CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer);
    // Lock the base address of the pixel buffer
    CVPixelBufferLockBaseAddress(imageBuffer, 0);
    
    // Get the number of bytes per row for the pixel buffer
    void *baseAddress = CVPixelBufferGetBaseAddress(imageBuffer);
    
    // Get the number of bytes per row for the pixel buffer
    size_t bytesPerRow = CVPixelBufferGetBytesPerRow(imageBuffer);
    // Get the pixel buffer width and height
    size_t width = CVPixelBufferGetWidth(imageBuffer);
    size_t height = CVPixelBufferGetHeight(imageBuffer);
    
    // Create a device-dependent RGB color space
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    
    // Create a bitmap graphics context with the sample buffer data
    CGContextRef context = CGBitmapContextCreate(baseAddress, width, height, 8,
                                                 bytesPerRow, colorSpace, kCGBitmapByteOrder32Little | kCGImageAlphaPremultipliedFirst);
    // Create a Quartz image from the pixel data in the bitmap graphics context
    CGImageRef quartzImage = CGBitmapContextCreateImage(context);
    // Unlock the pixel buffer
    CVPixelBufferUnlockBaseAddress(imageBuffer,0);
    
    // Free up the context and color space
    CGContextRelease(context);
    CGColorSpaceRelease(colorSpace);
    
    // Create an image object from the Quartz image
// UIImage *image = [UIImage imageWithCGImage:quartzImage];
//    UIImage *image =nil;
//
//    UIInterfaceOrientation  tion=  [[UIApplication sharedApplication] statusBarOrientation];
    
//    if (tion == UIInterfaceOrientationLandscapeLeft )
//    {
//        image = [UIImage imageWithCGImage:quartzImage scale:1.0f orientation:UIImageOrientationLeft];
//        
//      }
//    else if (tion == UIInterfaceOrientationLandscapeRight )
//    {
//         image = [UIImage imageWithCGImage:quartzImage scale:1.0f orientation:UIImageOrientationRight];
//        
//    }
//    else
//    {
       UIImage *image = [UIImage imageWithCGImage:quartzImage scale:1.0f orientation:UIImageOrientationUpMirrored];
        
//    }

    
    // Release the Quartz image
    CGImageRelease(quartzImage);
    
    return image;
    
}
-(void)captureOutput:(AVCaptureOutput *)captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection
{
    @autoreleasepool {
        
        //        CVImageBufferRef frame = nil;
        @synchronized(self) {
            //            @try {
            dispatch_sync(dispatch_get_main_queue(), ^{
                
                
                [self setImage:[self imageFromSampleBuffer:sampleBuffer]];
                            });
         
        }
    }
}
    
@end
