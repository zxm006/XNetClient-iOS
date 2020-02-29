//
//  AudioUnitManager.h
//  AUGraphApp
//
//  Created by ShengQiangLiu on 16/4/15.
//  Copyright © 2016年 www.mob.com. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AudioToolBox/AudioToolbox.h>
#include <map>
#include "OpenLocalUser.h"
 
#import "CSIOpusDecoder.h"
#include "ConnectServer.h"

typedef std::map<unsigned long, NSMutableData*> Audio_MAP_data;

@interface AudioUnitManager : NSObject
{
     pthread_mutex_t               m_pMutex;
    double  currentHardwareSampleRate;
    ISendCmdServer *m_SendCmdManage;
    Audio_MAP_data  audio_MAP_data;
}

@property (nonatomic, assign) Boolean playing;
@property (retain,nonatomic) CSIOpusDecoder *decoder;

@property (assign,nonatomic)AudioUnit remoteIOUnit;
// 多通道混合单元
@property (assign,nonatomic)AudioUnit mixerUnit;
+ (AudioUnitManager*)shareHandle;

+ (void)closeHandle;

- (instancetype)initAUGraph;

- (NSData*)getaudiodata:(int)nframes audioid:(long)audioid;

- (void)setaudiodata:(unsigned char *)data  length:(int)length audioid:(long)audioid;
- (void)start:(int)audioCount;
- (void)stop;


@end
