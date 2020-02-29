//
//  MicBlow.m
//  StreamMedia
//
//  Created by zhangxinming on 2017/10/24.
//  Copyright © 2017年 zhangxinming. All rights reserved.
//

#import "MicBlow.h"
static MicBlow* g_MicBlow= nil;


@interface MicBlow ()
{
    
}
@property (nonatomic,assign)id<MicBlowDelegate>  delegate;

@end

@implementation MicBlow

+ (MicBlow*)shareHandle
{
    if(!g_MicBlow)
        g_MicBlow = [[MicBlow alloc] init];
    return g_MicBlow;
}

+(void)closeHandle
    {
        if(g_MicBlow)
        {
            [g_MicBlow dealloc];
            g_MicBlow = nil;
        }
     
}

- (id)init
{
    if(self = [super init])
    {
        _recorder =nil;
        _timer = nil;
        return self;
    }
    return nil;
    
}

- (void)setDelegate:(id<MicBlowDelegate>)del
{
    _delegate =del;
}

/* 该方法确实会随环境音量变化而变化，但具体分贝值是否准确暂时没有研究 */
- (void)levelTimerCallback  {
    [_recorder updateMeters];
    
    float   level;                // The linear 0.0 .. 1.0 value we need.
    float   minDecibels = -80.0f; // Or use -60dB, which I measured in a silent room.
    float   decibels    = [_recorder averagePowerForChannel:0];
    
    if (decibels < minDecibels)
    {
        level = 0.0f;
    }
    else if (decibels >= 0.0f)
    {
        level = 1.0f;
    }
    else
    {
        float   root            = 1.0f;
        float   minAmp          = powf(10.0f, 0.05f * minDecibels);
        float   inverseAmpRange = 1.0f / (1.0f - minAmp);
        float   amp             = powf(10.0f, 0.05f * decibels);
        float   adjAmp          = (amp - minAmp) * inverseAmpRange;
        
        level = powf(adjAmp, 1.0f / root);
    }
    
    /* level 范围[0 ~ 1], 转为[0 ~120] 之间 */
    dispatch_async(dispatch_get_main_queue(), ^{
      
        if (_delegate && [_delegate respondsToSelector:@selector(micBlowAudiolevel:)]) {
            [_delegate micBlowAudiolevel:level];
        }
      
//     [_textLabel setText:[NSString stringWithFormat:@"%f", level*120]];
    });
}


- (void)start
{
    NSURL *url = [NSURL fileURLWithPath:@"/dev/null"];
    
    NSDictionary *settings = [NSDictionary dictionaryWithObjectsAndKeys:
                              [NSNumber numberWithFloat: 8000.0], AVSampleRateKey,
                              [NSNumber numberWithInt: kAudioFormatAppleLossless], AVFormatIDKey,
                              [NSNumber numberWithInt: 2], AVNumberOfChannelsKey,
                              [NSNumber numberWithInt: AVAudioQualityLow], AVEncoderAudioQualityKey,
                              nil];
    
    NSError *error;
    _recorder = [[AVAudioRecorder alloc] initWithURL:url settings:settings error:&error];
    if (_recorder)
    {
        [_recorder prepareToRecord];
        _recorder.meteringEnabled = YES;
        [_recorder record];
        
       NSTimeInterval period = 1.0;//设置时间间隔
       dispatch_queue_t queue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
        
        _timer = dispatch_source_create(DISPATCH_SOURCE_TYPE_TIMER, 0, 0, queue);
        dispatch_source_set_timer(_timer, DISPATCH_TIME_NOW, period * NSEC_PER_SEC, 0 * NSEC_PER_SEC);
        
        dispatch_source_set_event_handler(_timer, ^{
//            DLog(@"%@" , [NSThread currentThread]);//打印当前线程
        dispatch_async(dispatch_get_main_queue(), ^{
         [self levelTimerCallback];
      });
            
        });
        
        dispatch_resume(_timer);
    }
    else
    {
        DLog(@"%@", [error description]);
    }
}

- (void)stop
{
    if (_timer) {
          dispatch_source_cancel(_timer);
        dispatch_release(_timer);
//        [_timer release];
        _timer = nil;
    }
    
    if (_recorder) {
        [_recorder stop];
        [_recorder release];
        _recorder = NULL;
    }
    
}
@end
