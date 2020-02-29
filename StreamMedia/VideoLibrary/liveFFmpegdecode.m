#import "liveFFmpegdecode.h"
#include <pthread.h>
#include <sys/time.h>
#import <mach/mach_time.h>

@implementation liveFFmpegdecode


#define INBUF_OTHERVIDEOSIZE 0

- (id)init
{
    self = [super init];
    if (self)
    {
        _m_pUIImageView = nil;
        m_decodeQueue = nil;
        mdisplayTime = [self TimeGetTimestamp];
    }
    return self;
}

// Allocate RGB picture
-(UIImage *) imageFromAVPicture:(AVFrame *)pict  width:(int) width   height:(int )height;

{
    CGBitmapInfo bitmapInfo =kCGBitmapByteOrderDefault;
    CFDataRef data = CFDataCreateWithBytesNoCopy(kCFAllocatorDefault,pict->data[0],pict->linesize[0]*height,kCFAllocatorNull);
    CGDataProviderRef provider =CGDataProviderCreateWithCFData(data);
    CGColorSpaceRef colorSpace =CGColorSpaceCreateDeviceRGB();
    CGImageRef cgImage = CGImageCreate(width,height,
                                       8,24,pict->linesize[0],
                                       colorSpace,bitmapInfo,provider,NULL,NO,kCGRenderingIntentDefault);
    CGColorSpaceRelease(colorSpace);
    
    UIImage *image = [UIImage imageWithCGImage:cgImage];
    CGImageRelease(cgImage);
    CGDataProviderRelease(provider);
    CFRelease(data);
    
    return image;
}

-(int) Beginmpeg_decode_h264
{
    if (!m_decodeQueue) {
        m_decodeQueue = dispatch_queue_create("dispatch_queue--decode", NULL);
        
    }
    dispatch_async(dispatch_get_main_queue(), ^{
        if (_mivideoPlayView) {
            [_mivideoPlayView removeFromSuperview];
            [_mivideoPlayView release];
            _mivideoPlayView=nil;
        }
        UIView *subview=(UIView*)_m_pUIImageView ;
        if([subview isMemberOfClass:[UIView class]]||[subview isKindOfClass:[UIView class]]) {
            _mivideoPlayView=[[[iOSGLView alloc]initWithFrame:subview.bounds]autorelease];
            [_mivideoPlayView setBackgroundColor:[UIColor blackColor]];
            _mivideoPlayView.userInteractionEnabled = NO;
            [subview addSubview:_mivideoPlayView ];
            [subview setHidden:NO];
            [_mivideoPlayView setHidden:YES];
            [_mivideoPlayView setFps:30];
            [_mivideoPlayView startAnimation];
        }
        mTimer = [NSTimer timerWithTimeInterval:1.0 target:self selector:@selector(timerFired:)userInfo:nil repeats:YES];
        
        [[NSRunLoop mainRunLoop]addTimer:mTimer forMode:NSDefaultRunLoopMode];
    });
    av_register_all();
    AVCodec * decoder = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!decoder) {
        return -1;
    }
    m_codecContext= avcodec_alloc_context3(decoder);
    if (!m_codecContext) {
        
        return -1;
    }
    
    if(decoder->capabilities&CODEC_CAP_TRUNCATED)
        m_codecContext->flags|= CODEC_FLAG_TRUNCATED;
    m_codecContext->flags |=CODEC_FLAG_QSCALE
    |CODEC_FLAG2_FAST ;
    
    if (avcodec_open2(m_codecContext, decoder,0) < 0) {
        avcodec_close(m_codecContext);
        avcodec_free_context(&m_codecContext);
        m_codecContext = NULL;
        return -1;
    }
    m_IsDisplayVideo = true;
    return 0;
}

- (void)timerFired:(id)sender
{
    if([self TimeGetTimestamp] - mdisplayTime>10000) {
        if (_mivideoPlayView) {
            //            [_mivideoPlayView setHidden:YES];
        }
    }
}

-(int) Endinmpeg_decode_h264
{
    if (mTimer) {
        [mTimer invalidate];
        mTimer =nil;
    }
    
    m_IsDisplayVideo = false;
    if (m_codecContext) {
        avcodec_close(m_codecContext);
        avcodec_free_context(&m_codecContext);
        m_codecContext=NULL;
    }
    
    dispatch_async(dispatch_get_main_queue(), ^{
        // 更UI
        if(_mivideoPlayView) {
            [_mivideoPlayView removeFromSuperview];
            [_mivideoPlayView release];
            _mivideoPlayView=nil;
        }
    });
    
    if (m_decodeQueue) {
        dispatch_release(m_decodeQueue);
        m_decodeQueue = nil;
    }
    
    return 0;
}

-(bool)StartDisplayVideo
{
    m_IsDisplayVideo = true;
    return true;
}

-(void)StopDisplayVideo
{
    m_IsDisplayVideo = false;
}

double MachTimeToSecsdec(uint64_t time)
{
    mach_timebase_info_data_t timebase;
    mach_timebase_info(&timebase);
    return (double)time * (double)timebase.numer / (double)timebase.denom /1e9;
}

-(unsigned long)TimeGetTimestamp
{
    struct timeval now;
    gettimeofday(&now,NULL);
    return now.tv_sec*1000+now.tv_usec/1000;
}

-(int) ffmpeg_decode_h264_inqueue:(unsigned char*) ucbuffer Len:(int) buffer_size  iCount:(int) iCount
{
    if(!m_IsDisplayVideo&&buffer_size<=0)
        return 0;
    
    AVPacket avpkt;
    av_init_packet(&avpkt);
    
    avpkt.data = ucbuffer ;
    avpkt.size = buffer_size;
    
    int got_picture = 0;
    
    if(m_codecContext==NULL) {
        av_packet_unref(&avpkt);
        return 0;
    }
    
    AVFrame *   pictureFrame= av_frame_alloc();
    if (avcodec_send_packet(m_codecContext,&avpkt)==0) {
        got_picture= avcodec_receive_frame(m_codecContext, pictureFrame);
    } else {
        if (pictureFrame) {
            av_frame_free(&pictureFrame);
            pictureFrame=NULL;
        }
        av_packet_unref(&avpkt);
        return 0;
    }
    if (got_picture==0) {
        if(_mivideoPlayView) {
            int width =  m_codecContext->width;
            int height =m_codecContext->height;
            int bufsize =     height *width * 3 / 2;
            
            char* buf =  (char*)malloc(bufsize);
            memset(buf, 0,bufsize);
            int a = 0, i;
            for (i = 0; i<height; i++) {
                memcpy(buf + a,pictureFrame->data[0] + i *pictureFrame->linesize[0], width);
                a += width;
            }
            for (i = 0; i<height / 2; i++) {
                memcpy(buf + a,pictureFrame->data[1] + i *pictureFrame->linesize[1], width / 2);
                a += width / 2;
            }
            for (i = 0; i<height / 2; i++) {
                memcpy(buf + a,pictureFrame->data[2] + i *pictureFrame->linesize[2], width / 2);
                a += width / 2;
            }
            
            if(m_IsDisplayVideo&&_mivideoPlayView&&width>0&&height>0) {
                dispatch_async(dispatch_get_main_queue(), ^{
                    [_mivideoPlayView setHidden:NO];
                });
                
                mdisplayTime = [self TimeGetTimestamp];
                [_mivideoPlayView setBufferYUV:(const uint8_t*)buf width:width height:height];
            }
            free(buf);
            buf=NULL;
        }
    }
    
    if (pictureFrame) {
        av_frame_free(&pictureFrame);
        pictureFrame=NULL;
        
    }
    av_packet_unref(&avpkt);
    return 0;
}

-(int) ffmpeg_decode_h264:(unsigned char*) ucbuffer Len:(int) buffer_size  iCount:(int) iCount
{
    if (m_decodeQueue) {
        dispatch_sync(m_decodeQueue, ^{
            [self ffmpeg_decode_h264_inqueue:ucbuffer Len:buffer_size iCount:iCount];
        });
    }
    return 0;
}

- (void)SetLocalVideoWindow:(void  *)pUIImageView
{
    _m_pUIImageView = (UIView*)pUIImageView;
}

UIImage* imageWithImage(UIImage* image ,CGSize newSize)
{
    // Create a graphics image context
    UIGraphicsBeginImageContext(newSize);
    // Tell the old image to draw in this new context, with the desired
    // new size
    [image drawInRect:CGRectMake(0,0,newSize.width,newSize.height)];
    // Get the new image from the context
    UIImage* newImage = UIGraphicsGetImageFromCurrentImageContext();
    // End the context
    UIGraphicsEndImageContext();
    // Return the new image.
    return newImage;
}

-(void)dealloc
{
    self.m_pUIImageView = nil;
//    usleep(5000);
    [super dealloc];
}

@end

