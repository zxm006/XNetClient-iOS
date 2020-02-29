
//
//  CameraHelp.m
//
//
//  Created by zcx. on 11-6-28.
//  Copyright 2011  . All rights reserved.
//
extern "C"

#include "libavformat/avformat.h"
#include  "libavutil/imgutils.h"

#import <mach/mach_time.h>

#import "CameraHelp.h"
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/un.h>
#include<stdio.h>
#include<unistd.h>
#import<AVFoundation/AVFoundation.h>


static bool doing =NO;
AVCaptureDevice * g_CaptureDevice=nil;
AVCaptureDevicePosition g_DevicePosition = AVCaptureDevicePositionUnspecified;

NSArray *cameras = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];

@interface CameraHelp (Private)


+(AVCaptureDevice *)cameraAtPosition:(AVCaptureDevicePosition)position;
-(void)startPreview;
-(void)stopPreview;



@end

@implementation CameraHelp  (Private)

+ (AVCaptureDevice *)cameraAtPosition:(AVCaptureDevicePosition)position{
    
    if(g_DevicePosition==position && g_CaptureDevice!=nil)
    {
        return g_CaptureDevice;
    }
    
    for (AVCaptureDevice *device in cameras){
        if (device.position == position){
            g_DevicePosition = position;
            g_CaptureDevice = device;
            return g_CaptureDevice;
        }
    }
    return [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];
}

- (void)stopPreview
{
    DLog(@"stopPreview ====================>");
    [self.cameraLock lock];
    if(self.mCaptureSession){
        if([self.mCaptureSession isRunning])
        {
            [self.mCaptureSession stopRunning];
            // remove all sublayers
            if(mPreview)
            {
                //                dispatch_async(dispatch_get_main_queue(), ^{
                for(CALayer *ly in mPreview.layer.sublayers){
                    if([ly isKindOfClass: [AVCaptureVideoPreviewLayer class]])
                    {
                        [ly removeFromSuperlayer];
                        
                        break;
                    }
                }
                //                });
            }
        }
    }
    
    [self.cameraLock unlock];
}

- (void)startPreview
{
    [self.cameraLock lock];
    DLog(@"startPreview ====================>");
    if(self.mCaptureSession && mPreview && mStarted)
    {
        dispatch_async(dispatch_get_main_queue(), ^{
            
            AVCaptureVideoPreviewLayer* previewLayer = [[[AVCaptureVideoPreviewLayer alloc ]initWithSession: self.mCaptureSession]autorelease];
            
            previewLayer.frame = mPreview.bounds;
            previewLayer.videoGravity = AVLayerVideoGravityResizeAspectFill;
            
            [mPreview.layer addSublayer: previewLayer];
            
        });
        if(![self.mCaptureSession isRunning]){
            [self.mCaptureSession startRunning];
        }
    }
    
    [self.cameraLock unlock];
}

@end

static CameraHelp* g_camera = nil;

@implementation CameraHelp
@synthesize isoutPutBuffer;
@synthesize mStarted;

- (id)init
{
    if(g_camera)
        return g_camera;
    else
    {
        if(self = [super init])
        {
            isinith264Encoder =NO;
            self.bvideoChatSnapshot = NO;
            self->mFps = 25;
            self->mFrontCamera = YES;
            mStarted = NO;
            g_camera = self;
            isoutPutBuffer=NO;
            _mCaptureDevice = nil;
            mStarted =NO;
            mPreview=nil;
            _mCaptureSession=nil;
            
            h264Encoder = [H264HwEncoderImpl alloc];
            SpsPpsdata=[[NSMutableData alloc]init];
            [h264Encoder initWithConfiguration];
        }
        return g_camera;
    }
}

-(void)initEncodeWith:(int)width hight:(int)hight
{
    self.mWidth=width;
    self.mHeight=hight;
}

- (void)setVideoDataOutputBuffer:(VideoCaptureEncod*)videoCaptureEncod
{
    m_VideoCaptureEncod = videoCaptureEncod;
}

-(void)dealloc
{
    if (h264Encoder) {
        [h264Encoder unint];
        [h264Encoder release];
        h264Encoder=nil;
        isinith264Encoder =NO;
    }
    if (mPreview) {
        //        [mPreview release];
        mPreview=nil;
    }
    if(SpsPpsdata)
    {
        [SpsPpsdata release];
        SpsPpsdata =nil;
    }
    
    if(_mCaptureSession)
    {
        if(_videoInput)
        {
            [_mCaptureSession removeInput:_videoInput];
            self.videoInput =nil;
            
        }
        if([_mCaptureSession isRunning])
        {
            [_mCaptureSession stopRunning];
        }
        
        [_mCaptureSession release];
        _mCaptureSession = nil;
        
    }
    
    if(_avCaptureVideoDataOutput)
    {
        self.avCaptureVideoDataOutput =nil;
    }
    
    [super dealloc];
    
}

+ (CameraHelp*)shareCameraHelp
{
    if(!g_camera)
        g_camera = [[CameraHelp alloc] init];
    g_camera.cameraLock = [[NSLock alloc] init];
    return g_camera;
}

+ (void)closeCamera
{
    if(g_camera)
    {
        [g_camera dealloc];
        g_camera = nil;
    }
}

int x = 0;


- (void)reSetCamera
{
    [self reOrientation];
    
    //     dispatch_async(dispatch_get_main_queue(), ^{
    [mPreview setBackgroundColor:[UIColor clearColor]];
    
    for(CALayer *ly in mPreview.layer.sublayers)
    {
        
        if([ly isKindOfClass: [AVCaptureVideoPreviewLayer class]])
        {
            
            [CATransaction begin];
            [CATransaction setDisableActions:YES];
            [ly setFrame:mPreview.bounds];
            [CATransaction commit];
            break;
            
        }
    }
    
    [mPreview setBackgroundColor:[UIColor whiteColor]];
    
}

-(void)prepareVideoCapture:(int) resType  andFps: (int) fps andFrontCamera:(BOOL) bfront andPreview:(UIView*) view
{
    self.mresType = resType;
    self->mFps = fps;
    self->mFrontCamera = bfront;
    if(view)
        self->mPreview =  view  ;
    
    h264Encoder.delegate = self;
    
    if([_mCaptureSession isRunning])
    {
        [self stopVideoCapture];
        [self startVideoCapture];
    }
}

-(void)setPreview:(void*)preview
{
    if(preview == nil)
    {
        [self stopPreview];
        mPreview = nil;
    }
    else
    {
        
        mPreview = (UIView*)preview;
        [self reOrientation];
        
        //        dispatch_async(dispatch_get_main_queue(), ^{
        for(CALayer *ly in mPreview.layer.sublayers){
            if([ly isKindOfClass: [AVCaptureVideoPreviewLayer class]])
            {
                [ly removeFromSuperlayer];
                
                break;
            }
        }
        
        AVCaptureVideoPreviewLayer* previewLayer = [[[AVCaptureVideoPreviewLayer alloc ]initWithSession: self.mCaptureSession]autorelease];
        
        previewLayer.frame = mPreview.bounds;
        previewLayer.videoGravity = AVLayerVideoGravityResizeAspectFill;
        //            previewLayer.masksToBounds=YES;
        //        previewLayer.needsDisplayOnBoundsChange = YES;
        //
        [mPreview.layer addSublayer: previewLayer];
        
        //        });
        
    }
}

- (void)reOrientation
{
    
    AVCaptureConnection *videoConnection = [_avCaptureVideoDataOutput connectionWithMediaType:AVMediaTypeVideo];
    dispatch_async(dispatch_get_main_queue(), ^{
        UIInterfaceOrientation  tion=  [[UIApplication sharedApplication] statusBarOrientation];
        
        if (tion == UIInterfaceOrientationLandscapeLeft )
        {
            
            mPreview.transform =CGAffineTransformMakeRotation(M_PI_2);
            [videoConnection setVideoOrientation:AVCaptureVideoOrientationLandscapeLeft];
            
        }
        else if (tion == UIInterfaceOrientationLandscapeRight )
        {
            mPreview.transform =CGAffineTransformMakeRotation(-M_PI_2);
            [videoConnection setVideoOrientation:AVCaptureVideoOrientationLandscapeRight];
            
        }
        else if (tion == UIInterfaceOrientationPortraitUpsideDown )
        {
            mPreview.transform =CGAffineTransformMakeRotation(M_PI);
            [videoConnection setVideoOrientation:AVCaptureVideoOrientationPortraitUpsideDown];
            
        }
        else
        {
            [videoConnection setVideoOrientation:AVCaptureVideoOrientationPortrait];
            
            mPreview.transform = CGAffineTransformIdentity;
            
        }
    });
}


-(void)startVideoCapture
{
    
    DLog(@"startVideoCapture");
    //防锁
    dispatch_async(dispatch_get_main_queue(), ^{
        
        [[UIApplication sharedApplication] setIdleTimerDisabled:YES];
    });
    
    if(_mCaptureDevice || _mCaptureSession)
    {
        DLog(@"Already capturing");
        return;
    }
    if(mFrontCamera)
    {
        if((_mCaptureDevice = [CameraHelp cameraAtPosition: AVCaptureDevicePositionFront]) == nil)
        {
            DLog(@"Failed to get valide capture device");
            return;
        }
    }
    else
    {
        if((_mCaptureDevice = [CameraHelp cameraAtPosition: AVCaptureDevicePositionBack]) == nil)
        {
            DLog(@"Failed to get valide capture device");
            return;
        }
    }
    
    
    NSError *error = nil;
    _videoInput = [AVCaptureDeviceInput deviceInputWithDevice:_mCaptureDevice error:&error];
    if (!_videoInput)
    {
        DLog(@"Failed to get video input");
        self.mCaptureDevice = nil;
        return;
    }
    
    _mCaptureSession = [[AVCaptureSession alloc] init];
    if( _mresType ==0){
        _mCaptureSession.sessionPreset = AVCaptureSessionPresetLow;
    }
    else if( _mresType ==1){
        _mCaptureSession.sessionPreset = AVCaptureSessionPreset352x288;//
    }
    else if( _mresType ==2){
        _mCaptureSession.sessionPreset = AVCaptureSessionPresetHigh;
    }
    else {
        _mCaptureSession.sessionPreset = AVCaptureSessionPreset640x480;
    }
    
    [_mCaptureSession addInput:_videoInput];
    self.avCaptureVideoDataOutput =  [[[AVCaptureVideoDataOutput alloc] init]autorelease];
    //kCVPixelFormatType_420YpCbCr8BiPlanarFullRange
    _avCaptureVideoDataOutput.videoSettings = [[[NSDictionary alloc] initWithObjectsAndKeys:
                                                [NSNumber numberWithUnsignedInt:kCVPixelFormatType_32BGRA], kCVPixelBufferPixelFormatTypeKey,nil]autorelease];
    
    _avCaptureVideoDataOutput.alwaysDiscardsLateVideoFrames = YES;
    
    [_mCaptureSession beginConfiguration];
    [_mCaptureDevice lockForConfiguration:&error];
    
    [_mCaptureDevice setActiveVideoMinFrameDuration:CMTimeMake(1, 18)];
    [_mCaptureDevice setActiveVideoMaxFrameDuration:CMTimeMake(1, 20)];
    
    [_mCaptureDevice unlockForConfiguration];
    
    [_mCaptureSession commitConfiguration];
    
    dispatch_queue_t queue = dispatch_queue_create("dispatch_queue--ouput", NULL);
    [_avCaptureVideoDataOutput setSampleBufferDelegate:self queue:queue];
    [_mCaptureSession addOutput:_avCaptureVideoDataOutput];
    
    dispatch_release(queue);
    
    mStarted = YES;
    doing=NO;
    
    
    [self reOrientation];
    NSLog(@"startRunning ====>");
    if(![_mCaptureSession isRunning]){
        [_mCaptureSession startRunning];
    }
    NSLog(@"startPreview ====>");
    [self startPreview];
    NSLog(@"endPreview ====>");
    [[NSNotificationCenter defaultCenter] addObserver:self
                                             selector:@selector(reOrientation)
                                                 name:UIDeviceOrientationDidChangeNotification
                                               object:nil];
    //
}

-(void)stopVideoCapture
{
    [[NSNotificationCenter defaultCenter] removeObserver:self name:UIDeviceOrientationDidChangeNotification object:nil];
    
    [self stopPreview];
    if(_mCaptureSession) {
        if(_videoInput) {
            [_mCaptureSession removeInput:_videoInput];
            self.videoInput =nil;
            
        }
        if([_mCaptureSession isRunning]) {
            [_mCaptureSession stopRunning];
        }
        
        [_mCaptureSession release];
        _mCaptureSession = nil;
    }
    
    if(self.mCaptureDevice){
        self.mCaptureDevice = nil;
    }
}

-(BOOL)setFrontCamera
{
    if(mFrontCamera)
        return YES;
    [self stopVideoCapture];
    mFrontCamera = YES;
    [self performSelector:@selector(startVideoCapture) withObject:nil afterDelay:0.5];
    //    [self startVideoCapture];
    return YES;
}

-(BOOL)setBackCamera
{
    if(!mFrontCamera)
        return YES;
    [self stopVideoCapture];
    mFrontCamera = NO;
    [self performSelector:@selector(startVideoCapture) withObject:nil afterDelay:0.5];
    //    [self startVideoCapture];
    return YES;
}



-(UIImage *)imageFromAVPicture:(AVFrame *)pict  width:(int) width   height:(int )height

{
    CGBitmapInfo bitmapInfo = kCGBitmapByteOrderDefault;
    CFDataRef data = CFDataCreateWithBytesNoCopy(kCFAllocatorDefault, pict->data[0], pict->linesize[0]*height,kCFAllocatorNull);
    CGDataProviderRef provider = CGDataProviderCreateWithCFData(data);
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGImageRef cgImage = CGImageCreate(width,
                                       height,
                                       8,
                                       24,
                                       pict->linesize[0],
                                       colorSpace,
                                       bitmapInfo,
                                       provider,
                                       NULL,
                                       NO,
                                       kCGRenderingIntentDefault);
    CGColorSpaceRelease(colorSpace);
    UIImage *image = [UIImage imageWithCGImage:cgImage];
    CGImageRelease(cgImage);
    CGDataProviderRelease(provider);
    CFRelease(data);
    
    return image;
}

double MachTimeToSecsCH(uint64_t time)
{
    mach_timebase_info_data_t timebase;
    mach_timebase_info(&timebase);
    return (double)time * (double)timebase.numer /
    (double)timebase.denom /1e9;
}

-(void)videoChatSnapshot:(id<VideoChatDelegate>) delegate
{
    if([_mCaptureSession isRunning]) {
        self.videoChatDelegate = delegate;
        self.bvideoChatSnapshot = YES;
    }
    else {
        self.videoChatDelegate = delegate;
        
        dispatch_async(dispatch_get_main_queue(), ^{
            
            UIWindow * window = [UIApplication sharedApplication].keyWindow;
            UIGraphicsBeginImageContextWithOptions(window.bounds.size, YES, [UIScreen mainScreen].scale);
            NSInvocation *invocation = [NSInvocation invocationWithMethodSignature:
                                        [window methodSignatureForSelector:
                                         @selector(drawViewHierarchyInRect:afterScreenUpdates:)]];
            [invocation setTarget:window];
            [invocation setSelector:@selector(drawViewHierarchyInRect:afterScreenUpdates:)];
            CGRect arg2 = window.bounds;
            BOOL arg3 = YES;
            [invocation setArgument:&arg2 atIndex:2];
            [invocation setArgument:&arg3 atIndex:3];
            [invocation invoke];
            
            UIImage *screenshot = UIGraphicsGetImageFromCurrentImageContext();
            
            UIGraphicsEndImageContext();
            
            if (self.videoChatDelegate &&[self.videoChatDelegate respondsToSelector:@selector(videoChatSnapshot:)]) {
                [self.videoChatDelegate videoChatSnapshot:screenshot ];
                
            }
        });
    }
}

-(void)Snapshot:(CMSampleBufferRef)  sampleBuffer
{
    dispatch_async(dispatch_get_main_queue(), ^{
        
        UIImageView *imageview =[[UIImageView alloc]init];
        
        UIImage * image  = [self imageFromSampleBuffer:sampleBuffer];
        [imageview setImage:image];
        
        //获取正在显示的VC
        UIViewController *currentVC = [self currentViewController];
        
        [currentVC.view addSubview:imageview];
        [currentVC.view sendSubviewToBack:imageview];
        
        [imageview setBounds:mPreview.frame];
        [imageview setFrame:mPreview.frame];
        
        UIWindow* window = [UIApplication sharedApplication].keyWindow;
        
        UIGraphicsBeginImageContextWithOptions(window.bounds.size, YES, [UIScreen mainScreen].scale);
        
        NSInvocation *invocation = [NSInvocation invocationWithMethodSignature:
                                    [window methodSignatureForSelector:
                                     @selector(drawViewHierarchyInRect:afterScreenUpdates:)]];
        [invocation setTarget:window];
        [invocation setSelector:@selector(drawViewHierarchyInRect:afterScreenUpdates:)];
        CGRect arg2 = window.bounds;
        BOOL arg3 = YES;
        [invocation setArgument:&arg2 atIndex:2];
        [invocation setArgument:&arg3 atIndex:3];
        [invocation invoke];
        
        UIImage *screenshot = UIGraphicsGetImageFromCurrentImageContext();
        
        UIGraphicsEndImageContext();
        
        [imageview removeFromSuperview];
        
        if (self.videoChatDelegate &&[self.videoChatDelegate respondsToSelector:@selector(videoChatSnapshot:)]) {
            
            [self.videoChatDelegate videoChatSnapshot:screenshot ];
            
        }
    });
}



//获取Window当前显示的ViewController

- (UIViewController*)currentViewController{
    UIViewController* vc = [UIApplication sharedApplication].keyWindow.rootViewController;
    
    while (1) {
        if ([vc isKindOfClass:[UITabBarController class]]) {
            vc = ((UITabBarController*)vc).selectedViewController;
        }
        
        if ([vc isKindOfClass:[UINavigationController class]]) {
            vc = ((UINavigationController*)vc).visibleViewController;
        }
        
        if (vc.presentedViewController) {
            vc = vc.presentedViewController;
        }else{
            break;
        }
    }
    
    return vc;
}

- (void)setEncodebitrate:(long)bitrate
{
    if  (h264Encoder){
        [h264Encoder setEncodebitrate:bitrate];
    }
}

-(void)captureOutput:(AVCaptureOutput *)captureOutput didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer fromConnection:(AVCaptureConnection *)connection
{
    if (self.bvideoChatSnapshot) {
        self.bvideoChatSnapshot = NO;
        [self Snapshot:sampleBuffer];
    }
    
    if  (h264Encoder) {
        CVImageBufferRef imageBuffer = (CVImageBufferRef)CMSampleBufferGetImageBuffer(sampleBuffer);
        
        if(!isinith264Encoder) {
            int nWidth=(int)CVPixelBufferGetWidth(imageBuffer);
            int nHeight= (int)CVPixelBufferGetHeight(imageBuffer);
            self.mWidth = nWidth;
            self.mHeight = nHeight;
            
            int bitrate =150*2 * 1024;
            
            switch (self.mresType) {
                case 0:
                    bitrate= bitrate;
                    break;
                case 1:
                    bitrate= bitrate*3;
                    break;
                case 2:
                    bitrate=  bitrate*6;
                    break;
                default:
                    bitrate= bitrate*3;
                    break;
            }
            [h264Encoder initEncode:nWidth height:nHeight  framerate:25  bitrate:bitrate];
            isinith264Encoder =YES;
        }
        //          begin =mach_absolute_time();
        [h264Encoder encode:imageBuffer];
    }
}

#pragma mark - 摄像头采集回调
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
    UIImage *image = [UIImage imageWithCGImage:quartzImage];
    
    image = [UIImage imageWithCGImage:quartzImage scale:1.0f orientation:UIImageOrientationUp];
    
    CGImageRelease(quartzImage);
    
    return image;
}

- (void)gotSpsPps:(NSData*)sps pps:(NSData*)pps
{
    if( [SpsPpsdata  length]>0) {
        [SpsPpsdata setLength:0];
    }
    const char bytes[] = "\x00\x00\x00\x01";
    size_t length = (sizeof bytes) - 1; //string literals have implicit trailing '\0'
    NSMutableData *ByteHeader = [NSMutableData dataWithBytes:bytes length:length];
    [SpsPpsdata setData:ByteHeader];
    [SpsPpsdata appendData:sps];
    [SpsPpsdata appendData:ByteHeader];
    [SpsPpsdata appendData:pps];
}

- (void)gotEncodedData:(NSData*)data isKeyFrame:(BOOL)isKeyFrame
{
    const char bytes[] = "\x00\x00\x00\x01";
    size_t length = (sizeof bytes) - 1; //string literals have implicit trailing '\0'
    NSMutableData *ByteHeader = [NSMutableData dataWithBytes:bytes length:length];
    NSMutableData *Bytedata = [[[NSMutableData alloc]init]autorelease];
    if (isKeyFrame) {
        [Bytedata setData:SpsPpsdata];
        [Bytedata appendData:ByteHeader];
        [Bytedata appendData:data];
    }
    else {
        [Bytedata setData:ByteHeader];
        [Bytedata appendData:data];
    }
    
    if (m_VideoCaptureEncod) {
        m_VideoCaptureEncod->On_MediaReceiverCallbackCameraVideo((unsigned char*)[Bytedata bytes]  ,(int)[Bytedata length],isKeyFrame ,_mWidth,_mHeight);
    }
}


@end
