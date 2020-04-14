#import "AudioUnitTool.h"
#import <AVFoundation/AVFoundation.h>
#include <pthread/pthread.h>
#include <sys/sysctl.h>

#define kInputBus 1

//int pcm_db_count(const unsigned char* ptr, size_t size)
//{
//    int ndb = 0;
//    
//    short int value;
//    
//    int i;
//    long v = 0;
//    for(i=0; i<size; i+=2)
//    {
//        memcpy((char*)&value, ptr+i, 1);
//        memcpy((char*)&value+1, ptr+i+1, 1);
//        v += abs(value);
//    }
//    
//    v = v/(size/2);
//    
//    if(v != 0) {
//        ndb = (int)(20.0*log10((double)v / 65535.0 ));
//    }
//    else {
//        ndb = -96;
//    }
//    
//    return ndb;
//}

OSStatus recordingCallback(void *inRefCon,
                           AudioUnitRenderActionFlags *ioActionFlags,
                           const AudioTimeStamp *inTimeStamp,
                           UInt32 inBusNumber,
                           UInt32 inNumberFrames,
                           AudioBufferList *ioData)
{
    AudioUnitTool *audioUnitTool=(AudioUnitTool*)inRefCon;
   
    AudioUnit  audioUnit = audioUnitTool.mAudioUnit;
   
    
    
    AudioBuffer buffer;
   void* audiodata = malloc(inNumberFrames *2);
     buffer.mDataByteSize = inNumberFrames *2;
    buffer.mNumberChannels = 1;
    buffer.mData= audiodata;
    AudioBufferList bufferList;
    bufferList.mNumberBuffers = 1;
    bufferList.mBuffers[0] = buffer;
    OSStatus status = AudioUnitRender(audioUnit, ioActionFlags, inTimeStamp, inBusNumber, inNumberFrames, &bufferList);
    if(status != noErr)
    {
        DLog(@"AudioUnitRender 音频采集错误!");
    }
    
    audioUnitTool.bufflength=buffer.mDataByteSize;
    
    if (audioUnitTool&& audioUnitTool. mismute)
    {
        NSArray *encodedSamples = [audioUnitTool.encoder encodeSample:bufferList];
        if ([encodedSamples count]>0)
        {
             NSData *data= [NSData dataWithData: [encodedSamples objectAtIndex:0]];
            if(data)
//             [audioUnitTool SendPacket:0 data:(const char *)[data bytes] len:(int)[data length]];
            dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
                
                [audioUnitTool SendPacket:0 data:(const char *)[data bytes] len:(int)[data length]];
            });
            
 
        }
    }
    
    if (audiodata)
    {
        free(audiodata);
        audiodata =NULL;
    }
    return noErr;
}

static AudioUnitTool* g_audioUnitTool= nil;

@interface AudioUnitTool ()
{
    
}
@end

@implementation AudioUnitTool

- (BOOL) isiphone6s
{
    int mib[2];
    size_t len;
    void *machine;
    mib[0] = CTL_HW;
    mib[1] = HW_MACHINE;
    sysctl(mib, 2, NULL, &len, NULL, 0);
    machine = malloc(len);
    sysctl(mib, 2, machine, &len, NULL, 0);
    NSString *platform = [NSString stringWithCString:(char*)machine encoding:NSASCIIStringEncoding];
    free(machine);
    if ([platform isEqualToString:@"iPhone8,1"])
        return YES;
    if ([platform isEqualToString:@"iPhone8,2"])
        return YES;
    return NO;
}

- (id)init
{
    if(self = [super init])
    {
        self.mismute=1;
        self.sampleRate = 8000;
        
        [self setupEncoder];
        [self initunit];
//        [self setupSession];
        
        return self;
    }
    return nil;
}

+ (AudioUnitTool*)shareHandle
{
    if(!g_audioUnitTool)
        g_audioUnitTool = [[AudioUnitTool alloc] init];
    return g_audioUnitTool;
}

+(void)closeHandle
{
    if(g_audioUnitTool)
    {
        [g_audioUnitTool dealloc];
        g_audioUnitTool = nil;
    }
}

- (void)setupEncoder
{
    _encoder = [[CSIOpusEncoder encoderWithSampleRate:self.sampleRate channels:1 frameDuration:0.02]retain];
}

-(void)setSendCmdManage:( ISendCmdServer *)sendCmdManage
{
    m_SendCmdManage =sendCmdManage;
}

-(int) SendPacket:(int) channel  data:(const char *)data  len:(int) len
{
    if (m_SendCmdManage )
    {
        m_SendCmdManage->SendAudioData((char*)data, len);
    }
    return 0;
}

- (void)initunit
{
    AudioComponentDescription desc;
    desc.componentType = kAudioUnitType_Output;
    desc.componentSubType = kAudioUnitSubType_RemoteIO;
    desc.componentFlags = 0;
    desc.componentFlagsMask = 0;
    desc.componentManufacturer = kAudioUnitManufacturer_Apple;
    
    // Get component
    AudioComponent inputComponent = AudioComponentFindNext(NULL, &desc);
    OSStatus status;
    
    // Get audio units
    status = AudioComponentInstanceNew(inputComponent,&_mAudioUnit);
    if(status != noErr)
    {
        DLog(@"AudioUnit AudioComponentInstanceNew  %d!",(int)status);
    }
    // Enable IO for recording
    UInt32 flag = 1;
    status = AudioUnitSetProperty(self.mAudioUnit,
                                  kAudioOutputUnitProperty_EnableIO,
                                  kAudioUnitScope_Input,
                                  kInputBus,
                                  &flag,
                                  sizeof(flag));
    if(status != noErr)
    {
        DLog(@"AudioUnit AudioUnitSetProperty  %d!",(int)status);
    }
    AudioStreamBasicDescription audioFormat;
    audioFormat.mSampleRate            = self.sampleRate;
    audioFormat.mFormatID            = kAudioFormatLinearPCM;
    audioFormat.mFormatFlags        = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
    audioFormat.mFramesPerPacket    = 1;
    audioFormat.mChannelsPerFrame    = 1;
    audioFormat.mBitsPerChannel        = 16;
    audioFormat.mBytesPerPacket        = 2;
    audioFormat.mBytesPerFrame        = 2;
    // Apply format
    if(status != noErr)
    {
        DLog(@"AudioUnit AudioUnitSetProperty  %d!",(int)status);
    }
    status = AudioUnitSetProperty(self.mAudioUnit,
                                  kAudioUnitProperty_StreamFormat,
                                  kAudioUnitScope_Output,
                                  kInputBus,
                                  &audioFormat,
                                  sizeof(audioFormat));
    if(status != noErr)
    {
        DLog(@"AudioUnit AudioUnitSetProperty  kInputBus %d!",(int)status);
    }
    AURenderCallbackStruct callbackStruct;
    callbackStruct.inputProc = recordingCallback;
    callbackStruct.inputProcRefCon = self;
    status = AudioUnitSetProperty(self.mAudioUnit,
                                  kAudioOutputUnitProperty_SetInputCallback,
                                  kAudioUnitScope_Global,
                                  kInputBus,
                                  &callbackStruct,
                                  sizeof(callbackStruct));
    if(status != noErr)
    {
        DLog(@"AudioUnit AudioUnitSetProperty  %d!",(int)status);
    }
    flag = 0;
    status = AudioUnitSetProperty(self.mAudioUnit,
                                  kAudioUnitProperty_ShouldAllocateBuffer,
                                  kAudioUnitScope_Output,
                                  kInputBus,
                                  &flag,
                                  sizeof(flag));
    
    if(status != noErr)
    {
        DLog(@"AudioUnit AudioUnitSetProperty  %d!",(int)status);
    }
    
    status = AudioUnitInitialize(self.mAudioUnit);
    
    if(status != noErr)
    {
        DLog(@"AudioUnit AudioUnitInitialize  %d 初始化失败!",(int)status);
    }
}

- (void)start
{
    OSStatus status = AudioOutputUnitStart(self.mAudioUnit);
    
    if(status != noErr)
    {
        DLog(@" AudioUnit AudioOutputUnitStart  失败!");
    }
}

- (void)stop
{
    OSStatus status = AudioOutputUnitStop(self.mAudioUnit);
    
    if(status != noErr)
    {
        DLog(@" AudioUnit AudioOutputUnitStart  失败!");
    }
}


- (void)dealloc
{
    AudioUnitUninitialize(self.mAudioUnit);
    if(_encoder)
    {
        [_encoder release];
        _encoder=nil;
    }
    
    [super dealloc];
}

@end
