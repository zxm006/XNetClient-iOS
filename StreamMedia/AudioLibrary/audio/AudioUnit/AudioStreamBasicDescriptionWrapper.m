//
//  AudioStreamBasicDescriptionWrapper.m
//  MultichannelMixerAUGraphApp
//
//  Created by Sniper on 16/4/16.
//  Copyright © 2016年 ShengQiang. All rights reserved.
//

#import "AudioStreamBasicDescriptionWrapper.h"

@implementation AudioStreamBasicDescriptionWrapper
+ (AudioStreamBasicDescription)formatASBDWithPCMFormat:(CommonPCMFormat)pcmf
                                             inSampleRate:(Float64)sampleRate
                                            inNumChannels:(UInt32)numChannels
                                          inIsInterleaved:(BOOL)isInterleaved
{
    unsigned wordsize;
    
    AudioStreamBasicDescription asbd = {0};
    asbd.mSampleRate = sampleRate;
    asbd.mFormatID = kAudioFormatLinearPCM;
    asbd.mFormatFlags = kAudioFormatFlagsNativeEndian | kAudioFormatFlagIsPacked;
    asbd.mFramesPerPacket = 1;
    asbd.mChannelsPerFrame = numChannels;
    asbd.mBytesPerFrame = asbd.mBytesPerPacket = 0;
    asbd.mReserved = 0;
    
    
    if (pcmf == PCMFormatFloat32)
    {
        wordsize = 4;
        asbd.mFormatFlags |= kAudioFormatFlagIsFloat;
    }
    else if (pcmf == PCMFormatInt16)
    {
        wordsize = 2;
        asbd.mFormatFlags |= kAudioFormatFlagIsSignedInteger;
    }
    else if (pcmf == PCMFormatFixed824)
    {
        wordsize = 4;
        asbd.mFormatFlags |= kAudioFormatFlagIsSignedInteger | (24 << kLinearPCMFormatFlagsSampleFractionShift);
    }
    else
    {
        NSLog(@"PCMFormatOther");
    }
    
    
    asbd.mBitsPerChannel = wordsize * 8;
    if (isInterleaved)
    {
        asbd.mBytesPerFrame = asbd.mBytesPerPacket = wordsize * numChannels;
    }
    else
    {
        asbd.mFormatFlags |= kAudioFormatFlagIsNonInterleaved;
        asbd.mBytesPerFrame = asbd.mBytesPerPacket = wordsize;
    }
    
    return asbd;
}

@end
