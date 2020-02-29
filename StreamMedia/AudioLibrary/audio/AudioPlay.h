#import <Foundation/Foundation.h>
#import <OpenAL/al.h>
#import <OpenAL/alc.h>
#import <OpenAL/oalMacOSX_OALExtensions.h>
#import "CSIOpusDecoder.h"
//#include "webrtc/modules/audio_processing/ns/include/noise_suppression_x.h"

@interface AudioPlay : NSObject
{
    ALCcontext *mContext;
    ALCdevice  *mDevice;
    ALuint      outSourceId;
    ALenum      audioformat ;
    
    int audioSampleRate;
   dispatch_queue_t m_decodeQueue;
//    NsxHandle *pNS_inst;
}
@property(nonatomic,retain)    NSLock *m_audiolock  ;
- (void)LoudSpeaker:(int)bOpen;
- (void)initOpenAL;
- (void)openAudioFromQueue:(uint8_t *)data dataSize:(UInt32)dataSize;
- (BOOL)updataQueueBuffer;
- (void)playSound;
- (void)stopSound;
- (void)cleanUpOpenAL;
@property(retain,nonatomic) CSIOpusDecoder *decoder;
@end
