//
//  CSIOpusAdapter.m
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
#include <stdlib.h>
#import "CSIOpusEncoder.h"
#include "CSIDataQueue.h"
#include "opus.h"
#define BUFFER_LENGTH 4096

@interface CSIOpusEncoder ()
@property (assign) OpusEncoder *encoder;
@property (assign) double frameDuration;
@property (assign) int bytesPerFrame;
@property (assign) int samplesPerFrame;
@property (assign) CSIDataQueueRef inputBuffer;
@property (assign) opus_int16 *frameBuffer;
@property (assign) void *encodeBuffer;
@end

@implementation CSIOpusEncoder

- (id)initWithSampleRate:(double)sampleRate channels:(int)channels frameDuration:(double)frameDuration
{
    self = [super init];
    
    if(self)
    {
    DLog(@"Creating an encoder using Opus version %s", opus_get_version_string());
        int error;
       _encoder = opus_encoder_create(sampleRate, channels, OPUS_APPLICATION_VOIP, &error);
       opus_encoder_ctl(self.encoder, OPUS_SET_BITRATE(10000));//波特率
        opus_encoder_ctl(self.encoder, OPUS_SET_COMPLEXITY(10));//复杂度
        opus_encoder_ctl(self.encoder, OPUS_SET_SIGNAL(OPUS_SIGNAL_VOICE));//信号
        
        if(error != OPUS_OK)
        {
            DLog(@"Opus encoder encountered an error %s", opus_strerror(error));
        }

        self.frameDuration = frameDuration;
        self.samplesPerFrame = (int)(sampleRate * frameDuration);
        int bytesPerSample = sizeof(opus_int16);
        self.bytesPerFrame = self.samplesPerFrame * bytesPerSample;
        
         _inputBuffer = CSIDataQueueCreate();
        _encodeBuffer = malloc(BUFFER_LENGTH);
        _frameBuffer = (opus_int16 *)malloc(self.bytesPerFrame);
    }

    return self;
}

+ (CSIOpusEncoder *)encoderWithSampleRate:(double)sampleRate channels:(int)channels frameDuration:(double)frameDuration
{
    CSIOpusEncoder *encoder = [[[CSIOpusEncoder alloc] initWithSampleRate:sampleRate channels:channels frameDuration:frameDuration]autorelease];
    return encoder;
}

- (NSArray *)encodeSample:(AudioBufferList)bufferList
{
    return [self encodeBufferList:&bufferList];
}

//将此转变为NSData类型的

- (NSArray *)encodeFilePath:(NSString *)filePath
{
    NSMutableArray *output = [NSMutableArray array];
    NSData * data = [NSData dataWithContentsOfFile:filePath];
    DLog(@"%@",data);

    unsigned long len = [data length] / BUFFER_LENGTH;
    for (int i=0; i < len; ++i) {
        void *tempData = (__bridge void *)([data subdataWithRange:NSMakeRange(BUFFER_LENGTH *i, BUFFER_LENGTH)]) ;
        CSIDataQueueEnqueue(self.inputBuffer, tempData, BUFFER_LENGTH);
    }
//    OpusEncoder *st,
//    const opus_int16 *pcm,
//    int frame_size,
//    unsigned char *data,
//    opus_int32 max_data_bytes
    while (CSIDataQueueGetLength(self.inputBuffer) > self.bytesPerFrame) {
        CSIDataQueueDequeue(self.inputBuffer, self.frameBuffer, self.bytesPerFrame);
        opus_int32 result = opus_encode(self.encoder, self.frameBuffer, self.samplesPerFrame,(unsigned char *) self.encodeBuffer, BUFFER_LENGTH);
        
        if(result < 0) {
            DLog(@"Opus encoder encountered an error %s", opus_strerror(result));
            return nil;
        }
        
        NSData *encodedData = [NSData dataWithBytes:self.encodeBuffer length:result];
        [output addObject:encodedData];
    }
    DLog(@"%@",output);
    return output;
}


- (NSArray *)encodeBufferList:(AudioBufferList *)audioBufferList
{
//     @autoreleasepool {
    
    NSMutableArray *output = [NSMutableArray array];
    
    for (int i=0; i < audioBufferList->mNumberBuffers; ++i) {
        AudioBuffer audioBuffer = audioBufferList->mBuffers[i];
        CSIDataQueueEnqueue(self.inputBuffer, audioBuffer.mData, audioBuffer.mDataByteSize);
    }
    
    while (CSIDataQueueGetLength(self.inputBuffer) > self.bytesPerFrame) {
        CSIDataQueueDequeue(self.inputBuffer, self.frameBuffer, self.bytesPerFrame);
        opus_int32 result = opus_encode(self.encoder, self.frameBuffer, self.samplesPerFrame, (unsigned char *)self.encodeBuffer, BUFFER_LENGTH);
        
        if(result < 0) {
            DLog(@"Opus encoder encountered an error %s", opus_strerror(result));
            return nil;
        }
        
        NSData *encodedData = [NSData dataWithBytes:self.encodeBuffer length:result];
        [output addObject:encodedData];
    }
    
    return output;
}


- (NSArray *)encodeaudioNSData:(NSData *)audioData
{
//    @autoreleasepool {
    
        NSMutableArray *output = [NSMutableArray array];
        
    
            CSIDataQueueEnqueue(self.inputBuffer, [audioData bytes], [audioData length]);
    
        
        while (CSIDataQueueGetLength(self.inputBuffer) > self.bytesPerFrame) {
            CSIDataQueueDequeue(self.inputBuffer, self.frameBuffer, self.bytesPerFrame);
            opus_int32 result = opus_encode(self.encoder, self.frameBuffer, self.samplesPerFrame, (unsigned char *)self.encodeBuffer, BUFFER_LENGTH);
            
            if(result < 0) {
                DLog(@"Opus encoder encountered an error %s", opus_strerror(result));
                return nil;
            }
            
            NSData *encodedData = [NSData dataWithBytes:self.encodeBuffer length:result];
            [output addObject:encodedData];
        }
        
        return output;
//    }
}

- (void)dealloc
{
    if(_encodeBuffer)
    {
    free(_encodeBuffer);
        _encodeBuffer = nil;
    }
    if (_frameBuffer) {
            free(_frameBuffer);
        _frameBuffer = nil;
    }

    if (_inputBuffer)
    {
         CSIDataQueueDestroy(_inputBuffer);
        _inputBuffer = nil;
    }
   
    if (_encoder)
    {
            opus_encoder_destroy(_encoder);
        _encoder  = nil;
    }

    [super dealloc];
}

@end
