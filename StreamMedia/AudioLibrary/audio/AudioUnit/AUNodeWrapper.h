//
//  AUNodeWrapper.h
//  MultichannelMixerAUGraphApp
//
//  Created by Sniper on 16/4/17.
//  Copyright © 2016年 ShengQiang. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <AudioToolbox/AudioToolbox.h>

@interface AUNodeWrapper : NSObject

+ (void)setupRemoteIOUnitWithGraph:(AUGraph)graph
                            ioNode:(AUNode *)ioNode
                            ioUnit:(AudioUnit *)ioUnit;

+ (void)setupMultiChannelMixerWithGraph:(AUGraph)graph
                              mixerNode:(AUNode *)mixerNode
                              mixerUnit:(AudioUnit *)mixerUnit;

@end
