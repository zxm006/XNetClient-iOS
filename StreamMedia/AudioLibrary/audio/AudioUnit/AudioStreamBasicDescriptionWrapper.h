//
//  AudioStreamBasicDescriptionWrapper.h
//  MultichannelMixerAUGraphApp
//
//  Created by Sniper on 16/4/16.
//  Copyright © 2016年 ShengQiang. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h>

typedef enum : NSUInteger {
    PCMFormatOther = 0,
    PCMFormatFloat32 = 1,
    PCMFormatInt16 = 2,
    PCMFormatFixed824 = 3
} CommonPCMFormat;


@interface AudioStreamBasicDescriptionWrapper : NSObject

+ (AudioStreamBasicDescription)formatASBDWithPCMFormat:(CommonPCMFormat)pcmf
                                             inSampleRate:(Float64)sampleRate
                                            inNumChannels:(UInt32)numChannels
                                          inIsInterleaved:(BOOL)isInterleaved;



@end
