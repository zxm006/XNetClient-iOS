#import "AudioPlay.h"
#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>
#include <AudioToolbox/AudioToolbox.h>

 
 


@implementation AudioPlay

#pragma make - openal function
-(void)LoudSpeaker:(int)bOpen
{
//    
//        [[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryPlayAndRecord withOptions:AVAudioSessionCategoryOptionDefaultToSpeaker error:nil];
//        
//    }
//    else
//    {
//        //听筒播放模式
//        [ [AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryPlayAndRecord error:nil];
//        
//    }
    
//    AVAudioSession *sessionInstance = [AVAudioSession sharedInstance];
//    [sessionInstance setActive:YES error:nil];
    
}

//处理监听触发事件
-(void)sensorStateChange:(NSNotificationCenter *)notification
{
//    [[UIDevice currentDevice] setProximityMonitoringEnabled:YES]; //建议在播放之前设置yes，播放结束设置NO，这个功能是开启红外感应
//    //添加监听
//    [[NSNotificationCenter defaultCenter] addObserver:self
//                                             selector:@selector(sensorStateChange:)
//                                                 name:@"UIDeviceProximityStateDidChangeNotification"
//                                               object:nil];
//    
//    //如果此时手机靠近面部放在耳朵旁，那么声音将通过听筒输出，并将屏幕变暗（省电啊）
//    if ([[UIDevice currentDevice] proximityState] == YES)
//    {
//        DLog(@"Device is close to user");
//        [[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryPlayAndRecord error:nil];
//        
//    }
//    else
//    {
//        DLog(@"Device is not close to user");
//        [[AVAudioSession sharedInstance] setCategory:AVAudioSessionCategoryPlayback error:nil];
//    }
}


-(void)initOpenAL
{        //
    //    [[UIDevice currentDevice] setProximityMonitoringEnabled:YES]; //建议在播放之前设置yes，播放结束设置NO，这个功能是开启红外感应
    //
    //    //添加监听
    //    [[NSNotificationCenter defaultCenter] addObserver:self
    //                                             selector:@selector(sensorStateChange:)
    //                                                 name:@"UIDeviceProximityStateDidChangeNotification"
    //                                               object:nil];
//   [self LoudSpeaker:1];
    
    self.m_audiolock = [[[NSLock alloc]init]autorelease];
    
    AVAudioSession* session = [AVAudioSession sharedInstance];
    NSError *error = nil;
    
    [session setCategory:AVAudioSessionCategoryPlayAndRecord withOptions:AVAudioSessionCategoryOptionDefaultToSpeaker error:&error];
    
    
    _decoder = [[CSIOpusDecoder decoderWithSampleRate:16000 channels:1 frameDuration:0.02]retain];
    
    mDevice=alcOpenDevice(NULL);
    if (mDevice) {
        mContext=alcCreateContext(mDevice, NULL);
        alcMakeContextCurrent(mContext);
    }
 
    
    alGenSources(1, &outSourceId);
    alSpeedOfSound(1.0);
    alDopplerVelocity(1.0);
    alDopplerFactor(1.0);
    alSourcef(outSourceId, AL_PITCH, 1.0f);
    alSourcef(outSourceId, AL_GAIN, 1.0f);//控制音量0-1.0
    alSourcei(outSourceId, AL_LOOPING, AL_FALSE);
    alSourcef(outSourceId, AL_SOURCE_TYPE, AL_STREAMING);

    audioSampleRate = 16000;//temp->sampleRates;//采样率
    
    
//    pNS_inst = WebRtcNsx_Create();
//    {
//        printf("Noise_Suppression WebRtcNs_Create err! \n");
//
//    }
//    int nSample =16000;
//    int nMode =2;
//
//    if (0 !=  WebRtcNsx_Init(pNS_inst,nSample))
//    {
//        printf("Noise_Suppression WebRtcNs_Init err! \n");
//
//    }
//
//
//    if (0 !=  WebRtcNsx_set_policy(pNS_inst,nMode))
//    {
//        printf("Noise_Suppression WebRtcNs_set_policy err! \n");
//
//    }
    
    //    audioBit = 16;//temp->aBits;//位数
    //    audioChannel = 1;//temp->Channels; 通道数
    //    audioformat =0 ;
    
    //    if (audioBit == 8) {
    //        if (aChannel == 1)
    //            audioformat = AL_FORMAT_MONO8;
    //        else if(aChannel == 2)
    //            audioformat = AL_FORMAT_STEREO8;
    //        else if( alIsExtensionPresent( "AL_EXT_MCFORMATS" ) )
    //        {
    //            if( aChannel == 4 )
    //            {
    //                audioformat = alGetEnumValue( "AL_FORMAT_QUAD8" );
    //            }
    //            if( aChannel == 6 )
    //            {
    //                audioformat = alGetEnumValue( "AL_FORMAT_51CHN8" );
    //            }
    //        }
    //    }
    //
    //    if( audioBit == 16 )
    //    {
    //        if( audioChannel == 1 )
    //        {
    audioformat = AL_FORMAT_MONO16;
    //        }
    //        if( audioChannel == 2 )
    //        {
    //            // DLog(@"achhenl= 2!!!!!");
    //            audioformat = AL_FORMAT_STEREO16;
    //        }
    //        if( alIsExtensionPresent( "AL_EXT_MCFORMATS" ) )
    //        {
    //            if( audioChannel == 4 )
    //            {
    //                audioformat = alGetEnumValue( "AL_FORMAT_QUAD16" );
    //            }
    //            if( audioChannel == 6 )
    //            {
    //                DLog(@"achannel = 6!!!!!!");
    //                audioformat = alGetEnumValue( "AL_FORMAT_51CHN16" );
    //            }
    //        }
    //    }
    
}

- (void) openAudioFromQueue:(unsigned char*)data dataSize:(UInt32)dataSize
{
  
        if (self.m_audiolock) {
        [self.m_audiolock lock];
    }
    else
    {
        return;
    }
        @autoreleasepool{
    int queued;
    alGetSourcei(outSourceId , AL_BUFFERS_QUEUED , &queued);
    if(queued<50&&_decoder)
    {
        NSData *audiodata = [_decoder decode:data len:dataSize ];
        
//        short shOutL[640] = {0} ;
//        for(int i=0;i<2;i++)
//        {
//        WebRtcNsx_Process(pNS_inst,(const short* const*)[audiodata bytes]+320*i,1,(short* const*)shOutL+320*i);
//        }
//
        ALuint bufferID = 0;
        alGenBuffers(1, &bufferID);
        
        
        
        alBufferData(bufferID, audioformat, (const ALvoid*)[audiodata bytes], (ALsizei)[audiodata length],audioSampleRate);
        alSourceQueueBuffers(outSourceId, 1, &bufferID);
    }
    else{
//        DLog(@"音频包多于==================================%d",queued);
    }
    
    [self updataQueueBuffer];
        }
    if (self.m_audiolock) {
        [self.m_audiolock unlock];
    }
 
    
}

- (BOOL)updataQueueBuffer
{
    ALint stateVaue;
    int processed, queued;
    
    alGetSourcei(outSourceId, AL_BUFFERS_PROCESSED, &processed);
    alGetSourcei(outSourceId, AL_BUFFERS_QUEUED, &queued);
    alGetSourcei(outSourceId, AL_SOURCE_STATE, &stateVaue);
    
    if (stateVaue == AL_STOPPED ||
        stateVaue == AL_PAUSED ||
        stateVaue == AL_INITIAL)
    {
        if (queued < processed || queued == 0 ||(queued == 1 && processed ==1)) {
            DLog(@"Audio Stop");
            [self stopSound];
            [self cleanUpOpenAL];
        }
        
        [self playSound];
        return YES;
    }
    alGetSourcei(outSourceId , AL_BUFFERS_PROCESSED ,&processed);
    
    
    while(processed--)
    {
        
        ALuint buffer = 0;
        alSourceUnqueueBuffers(outSourceId ,1 ,&buffer);
        alDeleteBuffers(1 , &buffer);
    }
    
    return YES;
}


#pragma make - play/stop/clean function
- (void)playSound
{
    ALint  state;
    alGetSourcei(outSourceId, AL_SOURCE_STATE, &state);
    if (state != AL_PLAYING)
    {
        alSourcePlay(outSourceId);
    }
    
}

- (void)stopSound
{
     alSourceStop(outSourceId);
    
}

- (void)cleanUpOpenAL
{
 
    int processed =0;
    
    alGetSourcei(outSourceId , AL_BUFFERS_PROCESSED ,&processed);
    
    while(processed--) {
        
        ALuint buffer = 0;
        alSourceUnqueueBuffers(outSourceId ,1 ,&buffer);
        alDeleteBuffers(1 , &buffer);
    }
    
    alDeleteSources(1, &outSourceId);
    
    if(mContext) {
        alcDestroyContext(mContext);
        mContext=NULL;
    }
    
    if(mDevice) {
        alcCloseDevice(mDevice);
        mDevice=NULL;
    }
}


-(void)dealloc
{
    self.m_audiolock =nil;
   
    
  [self cleanUpOpenAL];
  
    if (_decoder) {
        [_decoder release];
        _decoder = nil;
    }
    
    [super dealloc];
}

@end
