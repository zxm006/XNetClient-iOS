//
//  OpusDecoder.m
//  OpusIPhoneTest
//
//  Copyright (c) 2012 Sam Leitch. All rights reserved.
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to
//  deal in the Software without restriction, including without limitation the
//  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
//  sell copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
//  IN THE SOFTWARE.
//

#import "CSIOpusDecoder.h"
#include "CSIDataQueue.h"

extern "C"
{

#include "opus.h"
}

@interface CSIOpusDecoder ()
@property (assign) OpusDecoder *decoder;
@property (assign) double sampleRate;
@property (assign) double frameDuration;
@property (assign) int samplesPerFrame;
@property (assign) size_t bytesPerSample;
@property (assign) CSIDataQueueRef outputBuffer;
@property (assign) opus_int16 *decodeBuffer;
@end


@implementation CSIOpusDecoder

+ (CSIOpusDecoder*)decoderWithSampleRate:(double)sampleRate channels:(int)channels frameDuration:(double)frameDuration
{
    return [[[CSIOpusDecoder alloc] initWithSampleRate:sampleRate channels:channels frameDuration:frameDuration]autorelease];
}

- (id)initWithSampleRate:(double)sampleRate channels:(int)channels frameDuration:(double)frameDuration
{
    self = [super init];
    
    if(self)
    {
        
        DLog(@"Creating an encoder using Opus version %s", opus_get_version_string());
        
        int error;
        _decoder = opus_decoder_create(sampleRate, channels, &error);
        
        if(error != OPUS_OK)
        {
            DLog(@"Opus encoder encountered an error %s", opus_strerror(error));
//            return nil;
        }
        
        _sampleRate = sampleRate;
        _frameDuration = frameDuration;
        _bytesPerSample = sizeof(opus_int16);
        _samplesPerFrame = (int)(sampleRate * frameDuration);
        
        _outputBuffer = CSIDataQueueCreate();
        _decodeBuffer = (opus_int16 *)malloc(_bytesPerSample * _samplesPerFrame);
    }
    
    return self;
}

- (void)dealloc
{
    if (_decodeBuffer) {
            free(_decodeBuffer);
        _decodeBuffer = NULL;
    }

    if (_outputBuffer) {
          CSIDataQueueDestroy(_outputBuffer);
        _outputBuffer =nil;
    }
  
if (_decoder) {
      opus_decoder_destroy(_decoder);
        _decoder =nil;
    }

    [super dealloc];
}

- (NSData*)decode:( const unsigned char *)packet len:(int)len
{
        
    int result = opus_decode(_decoder, ( const unsigned char *)packet,(opus_int32) len, _decodeBuffer, _samplesPerFrame, 0);
    if(result < 0)
    {
        DLog(@"Opus decoder encountered an error %s", opus_strerror(result));
        return nil;
    }
    
    @synchronized(self)
    {
        size_t bytesAvailable = result * self.bytesPerSample;
        NSData *data =[NSData dataWithBytes:self.decodeBuffer length:bytesAvailable];
        return data;
//        CSIDataQueueDequeue(self.outputBuffer, self.decodeBuffer, bytesAvailable);
//       double bufferDuration = CSIDataQueueGetLength(self.outputBuffer) / self.sampleRate;
//        if(bufferDuration > 0.5)
//        {
//            DLog(@"Output queue at %fs. Clearing.", bufferDuration);
//            CSIDataQueueClear(self.outputBuffer);
//        }
    }
  
}

- (int)tryFillBuffer:(AudioBufferList *)audioBufferList
{
    uint totalBytesRequested = 0;
    for (int i=0; i < audioBufferList->mNumberBuffers; ++i)
    {
        AudioBuffer audioBuffer = audioBufferList->mBuffers[i];
        if(audioBuffer.mNumberChannels > 1) return NO;
        totalBytesRequested += audioBuffer.mDataByteSize;
    }
    
    if(totalBytesRequested == 0) return 0;
    
    @synchronized(self)
    {
        size_t bytesAvailable = CSIDataQueueGetLength(self.outputBuffer);
        if(bytesAvailable < totalBytesRequested)
        {
//            DLog(@"Couldnt fill buffer. Needed %d bytes but only have %d", totalBytesRequested, bytesAvailable);
            return 0;
        }
        
        for (int i=0; i < audioBufferList->mNumberBuffers; ++i)
        {
            AudioBuffer audioBuffer = audioBufferList->mBuffers[i];
            CSIDataQueueDequeue(self.outputBuffer, audioBuffer.mData, audioBuffer.mDataByteSize);
        }
        
        return totalBytesRequested;
    }
}


@end
