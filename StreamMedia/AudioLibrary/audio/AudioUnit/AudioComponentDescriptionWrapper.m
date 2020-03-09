//
//  AudioComponentDescriptionWrapper.m
//  AUGraphApp
//
//  Created by admin on 16/4/15.
//  Copyright © 2016年 ShengQiangLiu. All rights reserved.
//

#import "AudioComponentDescriptionWrapper.h"
#import <AudioUnit/AUComponent.h>

@implementation AudioComponentDescriptionWrapper

/**
 *  Converter Unit AudioComponentDescription
 */
+ (AudioComponentDescription)converterUnitAcd
{
    return [self setupAcdWithComponentType:kAudioUnitType_FormatConverter componentSubType:kAudioUnitSubType_AUConverter];
}

/**
 *  iPod Equalizer Unit AudioComponentDescription
 */
+ (AudioComponentDescription)equalizerUnitAcd
{
    return [self setupAcdWithComponentType:kAudioUnitType_Effect componentSubType:kAudioUnitSubType_AUiPodEQ];
}

/**
 *  3D Mixer Unit
 */
+ (AudioComponentDescription)mixerUnitAcd
{
    return [self setupAcdWithComponentType:kAudioUnitType_Mixer componentSubType:kAudioUnitSubType_SpatialMixer];
}

/**
 *  Multichannel Mixer Unit AudioComponentDescription
 */
+ (AudioComponentDescription)multichannelMixerUnitAcd
{
    return [self setupAcdWithComponentType:kAudioUnitType_Mixer componentSubType:kAudioUnitSubType_MultiChannelMixer];
}

/**
 *  Generic Output Unit AudioComponentDescription
 */
+ (AudioComponentDescription)genericOutputUnitAcd
{
    return [self setupAcdWithComponentType:kAudioUnitType_Output componentSubType:kAudioUnitSubType_GenericOutput];
}

/**
 *  RemoteI/O Unit AudioComponentDescription
 */
+ (AudioComponentDescription)remoteIOUnitAcd
{
    return [self setupAcdWithComponentType:kAudioUnitType_Output componentSubType:kAudioUnitSubType_RemoteIO];
}

/**
 * Voice Processing I/O Unit AudioComponentDescription
 */
+ (AudioComponentDescription)voiceProcessingIOUnitAcd
{
    return [self setupAcdWithComponentType:kAudioUnitType_Output componentSubType:kAudioUnitSubType_VoiceProcessingIO];
}

+ (AudioComponentDescription)setupAcdWithComponentType:(OSType)componentType componentSubType:(OSType)subType
{
    AudioComponentDescription acd;
    acd.componentType = componentType;
    acd.componentSubType = subType;
    acd.componentManufacturer = kAudioUnitManufacturer_Apple;
    acd.componentFlags = 0;
    acd.componentFlagsMask = 0;
    return acd;
}


@end
