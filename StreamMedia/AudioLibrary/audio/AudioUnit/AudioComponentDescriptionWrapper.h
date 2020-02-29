//
//  AudioComponentDescriptionWrapper.h
//  AUGraphApp
//
//  Created by admin on 16/4/15.
//  Copyright © 2016年 ShengQiangLiu. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AudioUnit/AudioComponent.h>

@interface AudioComponentDescriptionWrapper : NSObject

/**
 *  Converter Unit AudioComponentDescription
 */
+ (AudioComponentDescription)converterUnitAcd;

/**
 *  iPod Equalizer Unit AudioComponentDescription
 */
+ (AudioComponentDescription)equalizerUnitAcd;

/**
 *  3D Mixer Unit
 */
+ (AudioComponentDescription)mixerUnitAcd;

/**
 *  Multichannel Mixer Unit AudioComponentDescription
 */
+ (AudioComponentDescription)multichannelMixerUnitAcd;

/**
 *  Generic Output Unit AudioComponentDescription
 */
+ (AudioComponentDescription)genericOutputUnitAcd;

/**
 *  RemoteI/O Unit AudioComponentDescription
 */
+ (AudioComponentDescription)remoteIOUnitAcd;

/**
 * Voice Processing I/O Unit AudioComponentDescription
 */
+ (AudioComponentDescription)voiceProcessingIOUnitAcd;


@end
