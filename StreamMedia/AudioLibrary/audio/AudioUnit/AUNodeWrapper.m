//
//  AUNodeWrapper.m
//  MultichannelMixerAUGraphApp
//
//  Created by Sniper on 16/4/17.
//  Copyright © 2016年 ShengQiang. All rights reserved.
//

#import "AUNodeWrapper.h"
#import "AudioComponentDescriptionWrapper.h"

@implementation AUNodeWrapper



+ (void)setupRemoteIOUnitWithGraph:(AUGraph)graph
                            ioNode:(AUNode *)ioNode
                            ioUnit:(AudioUnit *)ioUnit
{
    // I/O Unit 组件描述
    AudioComponentDescription iOUnitACD = {0};
    iOUnitACD = [AudioComponentDescriptionWrapper voiceProcessingIOUnitAcd];
    CheckError(AUGraphAddNode(graph, &iOUnitACD, ioNode), "RemoteIOUnit AUGraphAddNode 错误");
    
//            AudioComponentDescription desc;
//        desc.componentType = kAudioUnitType_Output;
//        desc.componentSubType = kAudioUnitSubType_VoiceProcessingIO;
//
//
//    desc.componentManufacturer = kAudioUnitManufacturer_Apple;
//    desc.componentFlags = 0;
//    desc.componentFlagsMask = 0;
    
    CheckError(AUGraphNodeInfo(graph, *ioNode,NULL, ioUnit), "RemoteIOUnit AUGraphNodeInfo 错误");
}


+ (void)setupMultiChannelMixerWithGraph:(AUGraph)graph
                              mixerNode:(AUNode *)mixerNode
                              mixerUnit:(AudioUnit *)mixerUnit
{
    // 多通道混合单元组件描述
    AudioComponentDescription mixerUnitACD = [AudioComponentDescriptionWrapper multichannelMixerUnitAcd];
    CheckError(AUGraphAddNode(graph, &mixerUnitACD, mixerNode), "MultiChannelMixer AUGraphAddNode 错误");
    CheckError(AUGraphNodeInfo(graph, *mixerNode, NULL, mixerUnit), "MultiChannelMixer AuAUGraphNodeInfo 错误");
}

 

static void CheckError(OSStatus error, const char *operation)
{
    if (error == noErr) return;
    char errorString[20];
    // See if it appears to be a 4-char-code
    *(UInt32 *)(errorString + 1) = CFSwapInt32HostToBig(error);
    if (isprint(errorString[1]) && isprint(errorString[2]) && isprint(errorString[3]) && isprint(errorString[4]))
    {
        errorString[0] = errorString[5] = '\'';
        errorString[6] = '\0';
    } else
        // No, format it as an integer
        sprintf(errorString, "%d", (int)error);
    fprintf(stderr, "Error: %s (%s)\n", operation, errorString);
    exit(1);
}

@end
