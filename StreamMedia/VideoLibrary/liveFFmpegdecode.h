#ifndef _FFMPEGDECODE_H_
#define _FFMPEGDECODE_H_
extern "C"
{
#import "libavformat/avformat.h"
}


#import <UIKit/UIKit.h>


#import "iOSGLView.h"
 
@interface liveFFmpegdecode:NSObject 
{
  AVCodecContext *m_codecContext;
  bool         m_IsDisplayVideo;
  iOSGLView *_mivideoPlayView;
    NSTimer *mTimer;
    unsigned long mdisplayTime;
    dispatch_queue_t m_decodeQueue;
}

@property(assign,nonatomic) UIView* m_pUIImageView;
- (int)Beginmpeg_decode_h264;
- (int)Endinmpeg_decode_h264;

- (int)ffmpeg_decode_h264:(unsigned char*) ucbuffer Len:(int) buffer_size iCount:(int) iCount;
- (UIImage *)imageFromAVPicture:(AVFrame *)pict  width:(int) width   height:(int )height;

- (void)SetLocalVideoWindow:(void *)pUIImageView;
- (bool)StartDisplayVideo;
- (void)StopDisplayVideo;

@end
 
#endif
