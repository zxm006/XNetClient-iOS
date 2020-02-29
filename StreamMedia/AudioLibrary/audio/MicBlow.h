//
//  MicBlow.h
//  StreamMedia
//
//  Created by zhangxinming on 2017/10/24.
//  Copyright © 2017年 zhangxinming. All rights reserved.
//
#import<MicBlow.h>
#import <Foundation/Foundation.h>
#import<AVFoundation/AVFoundation.H>
#include"uuDefine.h"
@interface MicBlow : NSObject
{
   
}
@property (nonatomic,retain)AVAudioRecorder *recorder;
@property (nonatomic,assign)dispatch_source_t timer;

+ (MicBlow*)shareHandle;
+ (void)closeHandle;
- (void)setDelegate:(id<MicBlowDelegate>)del;

- (void)start;
- (void)stop;

@end
