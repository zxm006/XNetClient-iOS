
#import <Foundation/Foundation.h>
#include "VideoLibrary.h"
//extern "C"
//{
//#include "libswscale/swscale.h"
#include  "libavformat/avformat.h"
#include  "libavutil/imgutils.h"
//}



@interface CapScreen : NSObject
{
    NSMutableData *  SpsPpsdata;
    MediaScreenVideoCallBack* m_MediaScreenVideoCallBack;
}
@property (nonatomic, readonly) BOOL isRecording;

+ (instancetype)sharedInstance;
- (BOOL)startCapScreen:(int)resType;
- (void)stopCapScreen;
-(void)SetCallBack:(MediaScreenVideoCallBack*)pVideoCaptureDataCallBack;
-(void)SetResolution:(unsigned int)uiWidth uiHeight:(unsigned int) uiHeight;
@end


