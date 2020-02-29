#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h>
#include "OpenLocalUser.h"
#import <Foundation/Foundation.h>
#include "ConnectServer.h"
#import "CSIOpusEncoder.h"

typedef int (*AUDIO_CALLBACK_FUNC)(int iMediaID, unsigned char *pcData, int iDataLen);


@interface AudioUnitTool : NSObject
{
    double  currentHardwareSampleRate;
    ISendCmdServer *m_SendCmdManage;
}

@property (nonatomic,retain)CSIOpusEncoder *encoder;
@property (nonatomic,assign)AudioUnit mAudioUnit;
@property (nonatomic,assign)AudioStreamBasicDescription streamFormat;
@property (nonatomic,assign)long   bufflength;
@property (nonatomic,assign)double sampleRate;
@property (nonatomic,assign)int    mismute;

+ (AudioUnitTool*)shareHandle;
+ (void)closeHandle;
- (void)start;
- (void)stop;
- (void)setSendCmdManage:(ISendCmdServer *)sendCmdManage;
- (int) SendPacket:(int) channel  data:(const char *)data  len:(int) len;

@end
