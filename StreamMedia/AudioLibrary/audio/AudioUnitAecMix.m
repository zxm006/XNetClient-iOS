#import "AudioUnitAecMix.h"
#import <AVFoundation/AVFoundation.h>
#include <pthread/pthread.h>
#include <sys/sysctl.h>
#import<AudioToolbox/AudioToolbox.h>
bool CheckOSStatus(int status,NSString *info){
    if (status) {
        printf("Status not 0! %d,%s", status,[info UTF8String]);
        return NO;
    }
    return YES;
}

static OSStatus outputCallback(void *udata, AudioUnitRenderActionFlags *flags, const AudioTimeStamp *ts,  UInt32 busnum, UInt32 nframes, AudioBufferList *buflist) {
    
    @autoreleasepool {
        AudioUnitAecMix *audioUnitTool=(AudioUnitAecMix*)udata;
        NSData *buff=[audioUnitTool getaudiodata:nframes];
        
        if (buff) {
            memcpy(buflist->mBuffers[0].mData,
                   [buff bytes],
                   [buff length] );
        } else {
            for (UInt32 i=0; i < buflist->mNumberBuffers; i++) {
                memset(buflist->mBuffers[i].mData, 0, buflist->mBuffers[i].mDataByteSize);
            }
        }
        return noErr;
    }
}
 
@interface AudioUnitAecMix ()
{
    NSMutableData    *preaudiodata;
}
@end

@implementation AudioUnitAecMix

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
    if(self = [super init]) {
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&m_pMutex, &attr);
        pthread_mutexattr_destroy(&attr);
        
        _mismute=1;
        preaudiodata=[[NSMutableData alloc]init];
        
        _sampleRate = 16000;
        [self setupEncoder];
        [self setupDecoder];
        return self;
    }
    return nil;
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
        
        if(dedata) {
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
    if ([ preaudiodata length]>nframes*2) {
        NSRange range = NSMakeRange(0,nframes*2);
        subFileData =  [ preaudiodata  subdataWithRange:range];
        [preaudiodata replaceBytesInRange:range withBytes:NULL length:0];
    }
    pthread_mutex_unlock(&m_pMutex);
    return subFileData;
}

- (void)startAudio:(bool) mixer
{
    if(preaudiodata) {
        [preaudiodata resetBytesInRange:NSMakeRange(0, [preaudiodata length])];
        [preaudiodata setLength:0];
    }
    
    [self setupDevice:mixer];
}

- (BOOL) setupDevice:(bool) mixer {
    UInt32 len;
    UInt32 val;
    OSStatus err;
    AudioComponent comp;
    AudioComponentDescription desc;
    AudioStreamBasicDescription fmt;
    if (mixer) {
        desc.componentType = kAudioUnitType_Mixer;
        desc.componentSubType = kAudioUnitSubType_MultiChannelMixer;
    } else {
        desc.componentType = kAudioUnitType_Output;
        desc.componentSubType = kAudioUnitSubType_VoiceProcessingIO;
    }
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
    
    if(preaudiodata) {
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
