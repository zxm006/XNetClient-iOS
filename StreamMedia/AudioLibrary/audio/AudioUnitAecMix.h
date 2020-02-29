//
//  AudioUnitTool.h
//  rztimedia
//
//  Created by zhangxinming on 16/1/11.
//  Copyright © 2016年 rongzhitong. All rights reserved.
//
#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h>
#include "OpenLocalUser.h"
#import <Foundation/Foundation.h>
//#include "CLinkMicManage.h"

#import "CSIOpusEncoder.h"
#import "CSIOpusDecoder.h"

typedef int (*AUDIO_CALLBACK_FUNC)(int iMediaID, unsigned char *pcData, int iDataLen);
 

@interface AudioUnitAecMix : NSObject
{
    pthread_mutex_t               m_pMutex;
    double                        currentHardwareSampleRate;
 
    int                          _micFrequency;
    int                          _micSampleSize;
    int                          _numMicChannels;
    @public
    AudioUnit                     mAudioUnit;
}

@property (retain,nonatomic) CSIOpusEncoder *encoder;
@property (retain,nonatomic) CSIOpusDecoder *decoder;
@property (nonatomic,assign)AudioStreamBasicDescription streamFormat;
@property (nonatomic,assign)long    bufflength;
@property (assign) double sampleRate;
@property (assign,nonatomic) int mismute;

- (NSData*)getaudiodata:(int)nframes;

- (void)     setaudiodata:(unsigned char *)data length:(int)length;

- (void)     startAudio:(bool) mixer; 
@end
