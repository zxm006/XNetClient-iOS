#import "AudioUnitAecTool.h"
#import <AVFoundation/AVFoundation.h>
#include <pthread/pthread.h>
#include <sys/sysctl.h>

static void CheckError(OSStatus error, const char *operation)
{
    //   if (error == noErr) return;
    //   char errorString[20];
    //   // See if it appears to be a 4-char-code
    //   *(UInt32 *)(errorString + 1) = CFSwapInt32HostToBig(error);
    //   if (isprint(errorString[1]) && isprint(errorString[2]) &&
    //       isprint(errorString[3]) && isprint(errorString[4])) {
    //      errorString[0] = errorString[5] = '\'';
    //      errorString[6] = '\0';
    //   } else
    //      // No, format it as an integer
    //      sprintf(errorString, "%d", (int)error);
    //   fprintf(stderr, "Error: %s (%s)\n", operation, errorString);
    //   exit(1);
}

OSStatus InputCallback(void *inRefCon,
                       AudioUnitRenderActionFlags *ioActionFlags,
                       const AudioTimeStamp *inTimeStamp,
                       UInt32 inBusNumber,
                       UInt32 inNumberFrames,
                       AudioBufferList *ioData)
{
    @autoreleasepool {
    
        AudioUnitAecTool *audioUnitTool=(AudioUnitAecTool*)inRefCon;
        
        AudioBuffer buffer;
        void* audiodata = malloc(inNumberFrames *2);
        buffer.mDataByteSize = inNumberFrames *2;
        buffer.mNumberChannels = 1;
        buffer.mData= audiodata;
        AudioBufferList bufferList;
        bufferList.mNumberBuffers = 1;
        bufferList.mBuffers[0] = buffer;
        CheckError(AudioUnitRender(audioUnitTool->mAudioUnit,
                                   ioActionFlags,
                                   inTimeStamp,
                                   1,
                                   inNumberFrames,
                                   &bufferList),
                   "AudioUnitRender failed");
        
        audioUnitTool.bufflength=buffer.mDataByteSize;
        
        if (audioUnitTool&& audioUnitTool. mismute)
        {
            
            NSArray *encodedSamples = [audioUnitTool.encoder encodeSample:bufferList];
            if (encodedSamples&&[encodedSamples count]>0) {
                //
                NSData *data= [NSData dataWithData: [encodedSamples objectAtIndex:0]];
                //                 dispatch_async(audioUnitTool.m_audio_send_queue, ^{
                if(data)
                {
                    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
                        
                    [audioUnitTool SendPacket:0 data:(const char *)[data bytes] len:(int)[data length]];
                    });
                }
                //               });
//                DLog(@"AudioUnitRender ==> %u",(unsigned int)buffer.mDataByteSize);
                
            }
            else
            {
//                DLog(@"encodeSample ==>数据不够？ %u",(unsigned int)buffer.mDataByteSize);
                
            }
            
        }
        if (audiodata)
        {
            free(audiodata);
            audiodata =NULL;
        }
        
        return noErr;
     }
}

static OSStatus outputCallback(void *udata, AudioUnitRenderActionFlags *flags, const AudioTimeStamp *ts,  UInt32 busnum, UInt32 nframes, AudioBufferList *buflist) {
    
    @autoreleasepool {
        AudioUnitAecTool *audioUnitTool=(AudioUnitAecTool*)udata;
        NSData *buff=[audioUnitTool getaudiodata:nframes];
        
        if (buff)
        {
             memcpy(buflist->mBuffers[0].mData,
                   [buff bytes],
                   [buff length] );
            
        }
        else
        {
            for (UInt32 i=0; i < buflist->mNumberBuffers; i++)
            {
                memset(buflist->mBuffers[i].mData, 0, buflist->mBuffers[i].mDataByteSize);
            }
        }
        return noErr;
     }
}

static AudioUnitAecTool* g_AudioUnitAecTool= nil;

@interface AudioUnitAecTool ()
{
    NSMutableData    *preaudiodata;
}
@end



@implementation AudioUnitAecTool

@synthesize streamFormat;

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
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&m_pMutex, &attr);
        pthread_mutexattr_destroy(&attr);
       
        _mismute=1;
        preaudiodata=[[NSMutableData alloc]init];
        
        _sampleRate = 8000;
        [self setupEncoder];
        [self setupDecoder];
        
        return self;
    }
    return nil;
}

+ (AudioUnitAecTool*)shareHandle
{
    if(!g_AudioUnitAecTool)
        g_AudioUnitAecTool = [[AudioUnitAecTool alloc] init];
    return g_AudioUnitAecTool;
}

+(void)closeHandle
{
    if(g_AudioUnitAecTool)
    {
        [g_AudioUnitAecTool dealloc];
        g_AudioUnitAecTool = nil;
    }
}

- (void)setupEncoder
{
    _encoder = [[CSIOpusEncoder encoderWithSampleRate:self.sampleRate channels:1 frameDuration:0.02]retain];
}

- (void)setupDecoder
{
    _decoder = [[CSIOpusDecoder decoderWithSampleRate:self.sampleRate channels:1 frameDuration:0.02]retain];
}


- (void)setaudiodata:(unsigned char *)data length:(int)length
{
     @autoreleasepool {
    
        (void)pthread_mutex_lock(&m_pMutex);
        
        NSData *dedata = [self.decoder decode:(const unsigned char*)data len:length];
        
        if(dedata)
        {
            [ preaudiodata appendData:dedata];
        }
        
        pthread_mutex_unlock(&m_pMutex);
        usleep(50);
     }
}

- (NSData*)getaudiodata:(int)nframes
{
    (void)pthread_mutex_lock(&m_pMutex);
    NSData *subFileData = nil;
    if ([ preaudiodata length]>nframes*2)
    {
        NSRange range = NSMakeRange(0,nframes*2);
        subFileData =  [ preaudiodata  subdataWithRange:range];
        [preaudiodata replaceBytesInRange:range withBytes:NULL length:0];
    }
    pthread_mutex_unlock(&m_pMutex);
    return subFileData;
}

-(void)setVoiceManage:(    ISendCmdServer *)linkMic
{
    m_LinkMicManage = linkMic;
}

-(int) SendPacket:(int) channel  data:(const char *)data  len:(int) len
{
    if (m_LinkMicManage )
    {
     m_LinkMicManage->SendAudioData((char*)data, len);
        
    }
    return 0;
}

- (void)startAudio
{
    AVAudioSession* session = [AVAudioSession sharedInstance];
    NSError *error = nil;
    
    [session setCategory:AVAudioSessionCategoryPlayAndRecord withOptions:AVAudioSessionCategoryOptionDefaultToSpeaker error:&error];
    
    if(preaudiodata)
    {
        [preaudiodata resetBytesInRange:NSMakeRange(0, [preaudiodata length])];
        [preaudiodata setLength:0];
        
    }
    
    [self setupDevice];
}

- (BOOL) setupDevice {
    UInt32 len;
    UInt32 val;
    OSStatus err;
    AudioComponent comp;
    AudioComponentDescription desc;
    AudioStreamBasicDescription fmt;
    
    desc.componentType = kAudioUnitType_Output;
    desc.componentSubType = kAudioUnitSubType_VoiceProcessingIO;
    desc.componentManufacturer = kAudioUnitManufacturer_Apple;
    desc.componentFlags = 0;
    desc.componentFlagsMask = 0;
    
    comp = AudioComponentFindNext(NULL, &desc);
    if (! comp) {
        DLog(@"MKVoiceProcessingDevice: Unable to find AudioUnit.");
        return NO;
    }
    
    err = AudioComponentInstanceNew(comp, (AudioComponentInstance *) &mAudioUnit);
    if (err != noErr) {
        DLog(@"MKVoiceProcessingDevice: Unable to instantiate new AudioUnit.");
        return NO;
    }
    
    val = 1;
    err = AudioUnitSetProperty(mAudioUnit, kAudioOutputUnitProperty_EnableIO, kAudioUnitScope_Input, 1, &val, sizeof(UInt32));
    if (err != noErr) {
        DLog(@"MKVoiceProcessingDevice: Unable to configure input scope on AudioUnit.");
        return NO;
    }
    
    val = 1;
    err = AudioUnitSetProperty(mAudioUnit, kAudioOutputUnitProperty_EnableIO, kAudioUnitScope_Output, 0, &val, sizeof(UInt32));
    if (err != noErr) {
        DLog(@"MKVoiceProcessingDevice: Unable to configure output scope on AudioUnit.");
        return NO;
    }
    
     AURenderCallbackStruct cb;
//    cb.inputProc = InputCallback;
//    cb.inputProcRefCon = self;
//    len = sizeof(AURenderCallbackStruct);
//    err = AudioUnitSetProperty(mAudioUnit, kAudioOutputUnitProperty_SetInputCallback, kAudioUnitScope_Output, 0, &cb, len);
//
//
//    if (err != noErr) {
//        DLog(@"MKVoiceProcessingDevice: Unable to setup callback.");
//        return NO;
//    }
    
    cb.inputProc = outputCallback;
    cb.inputProcRefCon = self;
    len = sizeof(AURenderCallbackStruct);
    err = AudioUnitSetProperty(mAudioUnit, kAudioUnitProperty_SetRenderCallback, kAudioUnitScope_Input, 0, &cb, len);
    if (err != noErr) {
        DLog(@"MKVoiceProcessingDevice: Could not set render callback.");
        return NO;
    }
    
    len = sizeof(AudioStreamBasicDescription);
    err = AudioUnitGetProperty(mAudioUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 1, &fmt, &len);
    if (err != noErr) {
        DLog(@"MKVoiceProcessingDevice: Unable to query device for stream info.");
        return NO;
    }
    
    if (fmt.mChannelsPerFrame > 1) {
        DLog(@"MKVoiceProcessingDevice: Input device with more than one channel detected. Defaulting to 1.");
    }
    //
    _micFrequency = _sampleRate;
    _numMicChannels = 1;
    _micSampleSize = _numMicChannels * sizeof(short);
    
    fmt.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
    fmt.mBitsPerChannel = sizeof(short) * 8;
    fmt.mFormatID = kAudioFormatLinearPCM;
    fmt.mSampleRate = _micFrequency;
    fmt.mChannelsPerFrame = _numMicChannels;
    fmt.mBytesPerFrame = _micSampleSize;
    fmt.mBytesPerPacket = _micSampleSize;
    fmt.mFramesPerPacket = 1;
    
//    len = sizeof(AudioStreamBasicDescription);
//    err = AudioUnitSetProperty(mAudioUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Output, 1, &fmt, len);
//    if (err != noErr) {
//        DLog(@"MKVoiceProcessingDevice: Unable to set stream format for output device. (output scope)");
//        return NO;
//    }
    
    len = sizeof(AudioStreamBasicDescription);
    err = AudioUnitSetProperty(mAudioUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0, &fmt, len);
    if (err != noErr) {
        DLog(@"MKVoiceProcessingDevice: Unable to set stream format for input device. (input scope)");
        return NO;
    }
    
    val = 0;
    len = sizeof(UInt32);
    err = AudioUnitSetProperty(mAudioUnit, kAUVoiceIOProperty_BypassVoiceProcessing, kAudioUnitScope_Input, 0, &val, len);
    if (err != noErr) {
        DLog(@"MKVoiceProcessingDevice: Unable to disable VPIO voice processing.");
        return NO;
    }
    //
//        val = 0;
//        len = sizeof(UInt32);
//        err = AudioUnitSetProperty(mAudioUnit, kAUVoiceIOProperty_VoiceProcessingEnableAGC, kAudioUnitScope_Global, 0, &val, len);
//        if (err != noErr) {
//            DLog(@"MKVoiceProcessingDevice: Unable to disable VPIO AGC.");
//            return NO;
//        }
//          val = 0;
//            len = sizeof(UInt32);
//            err = AudioUnitSetProperty(mAudioUnit, kAUVoiceIOProperty_MuteOutput, kAudioUnitScope_Global, 0, &val, len);
//            if (err != noErr) {
//                DLog(@"MKVoiceProcessingDevice: unable to unmute output.");
//                return NO;
//            }
    
    err = AudioUnitInitialize(mAudioUnit);
    if (err != noErr) {
        DLog(@"MKVoiceProcessingDevice: Unable to initialize AudioUnit.");
        return NO;
    }
    
    err = AudioOutputUnitStart(mAudioUnit);
    if (err != noErr) {
        DLog(@"MKVoiceProcessingDevice: Unable to start AudioUnit.");
        return NO;
    }
    
    return YES;
}

- (BOOL) teardownDevice
{
    OSStatus err;
     err = AudioOutputUnitStop(mAudioUnit);
    if (err != noErr) {
        DLog(@"MKVoiceProcessingDevice: unable to stop AudioUnit.");
        return NO;
    }
    
    err = AudioComponentInstanceDispose(mAudioUnit);
    if (err != noErr) {
        DLog(@"MKVoiceProcessingDevice: unable to dispose of AudioUnit.");
        return NO;
    }
    
    
    DLog(@"MKVoiceProcessingDevice: teardown finished.");
    return YES;
}




- (void)dealloc
{
    [self teardownDevice];
    
    if(preaudiodata)
    {
        [preaudiodata release];
        preaudiodata =nil;
    }
    if (_encoder) {
        [_encoder release];
        _encoder =nil;
    }
    
    if (_decoder) {
        [_decoder release];
        _decoder =nil;
    }
    
    pthread_mutex_destroy(&m_pMutex);
    
    [super dealloc];
}

@end
