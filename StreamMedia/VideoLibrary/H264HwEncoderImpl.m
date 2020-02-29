//
//  H264HwEncoderImpl.m
//  h264v1
//
//  Created by Ganvir, Manish on 3/31/15.
//  Copyright (c) 2015 Ganvir, Manish. All rights reserved.
//

#import "H264HwEncoderImpl.h"

#import <VideoToolbox/VideoToolbox.h>
@interface H264HwEncoderImpl ()
{
    NSString * yuvFile;
    VTCompressionSessionRef EncodingSession;
    dispatch_queue_t aQueue;
    CMFormatDescriptionRef  format;
    CMSampleTimingInfo * timingInfo;
    BOOL initialized;
    long long  frameCount;
    NSData *sps;
    NSData *pps;
}
@end

@implementation H264HwEncoderImpl

- (void) initWithConfiguration
{
    EncodingSession = nil;
    initialized = true;
    aQueue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
    frameCount = 0;
    sps = NULL;
    pps = NULL;
}

void didCompressH264(void *outputCallbackRefCon, void *sourceFrameRefCon, OSStatus status, VTEncodeInfoFlags infoFlags,
                     CMSampleBufferRef sampleBuffer )
{
    if (status != 0) return;
    if (!CMSampleBufferDataIsReady(sampleBuffer)) {
        DLog(@"didCompressH264 data is not ready ");
        return;
    }
    H264HwEncoderImpl* encoder = (__bridge H264HwEncoderImpl*)outputCallbackRefCon;
    
    // Check if we have got a key frame first
    bool keyframe = !CFDictionaryContainsKey( (CFDictionaryRef)(CFArrayGetValueAtIndex(CMSampleBufferGetSampleAttachmentsArray(sampleBuffer, true), 0)), (const void *)kCMSampleAttachmentKey_NotSync);
    
    if (keyframe) {
        CMFormatDescriptionRef format = CMSampleBufferGetFormatDescription(sampleBuffer);
        size_t sparameterSetSize, sparameterSetCount;
        const uint8_t *sparameterSet;
        OSStatus statusCode = CMVideoFormatDescriptionGetH264ParameterSetAtIndex(format, 0, &sparameterSet, &sparameterSetSize, &sparameterSetCount, 0 );
        if (statusCode == noErr) {
            // Found sps and now check for pps
            size_t pparameterSetSize, pparameterSetCount;
            const uint8_t *pparameterSet;
            OSStatus statusCode = CMVideoFormatDescriptionGetH264ParameterSetAtIndex(format, 1, &pparameterSet, &pparameterSetSize, &pparameterSetCount, 0 );
            if (statusCode == noErr) {// Found pps
                encoder->sps = [NSData dataWithBytes:sparameterSet length:sparameterSetSize];
                encoder->pps = [NSData dataWithBytes:pparameterSet length:pparameterSetSize];
                if (encoder->_delegate) {
                    [encoder->_delegate gotSpsPps:encoder->sps pps:encoder->pps];
                }
            }
        }
    }
    
    CMBlockBufferRef dataBuffer = CMSampleBufferGetDataBuffer(sampleBuffer);
    size_t length, totalLength;
    char *dataPointer;
    OSStatus statusCodeRet = CMBlockBufferGetDataPointer(dataBuffer, 0, &length, &totalLength, &dataPointer);
    if (statusCodeRet == noErr) {
        size_t bufferOffset = 0;
        static const int AVCCHeaderLength = 4;
        while (bufferOffset < totalLength - AVCCHeaderLength) {
            // Read the NAL unit length
            uint32_t NALUnitLength = 0;
            memcpy(&NALUnitLength, dataPointer + bufferOffset, AVCCHeaderLength);
            // Convert the length value from Big-endian to Little-endian
            NALUnitLength = CFSwapInt32BigToHost(NALUnitLength);
            NSData* data = [[NSData alloc] initWithBytes:(dataPointer + bufferOffset + AVCCHeaderLength) length:NALUnitLength];
            [encoder->_delegate gotEncodedData:data isKeyFrame:keyframe];
            [data release];
            // Move to the next NAL unit in the block buffer
            bufferOffset += AVCCHeaderLength + NALUnitLength;
        }
    }
}

- (void) initEncode:(int)width  height:(int)height  framerate:(int)fps bitrate:(int)bt
{
    _m_width = width;
    _m_height = height;
    _m_fps = fps;
    _m_bt= bt;
    dispatch_sync(aQueue, ^{
        OSStatus status = VTCompressionSessionCreate(NULL, width, height, kCMVideoCodecType_H264, NULL, NULL, NULL, didCompressH264, (__bridge void *)(self),  &EncodingSession);
        if (status != 0) {
            DLog(@"H264: Unable to create a H264 session");
            _error = @"H264: Unable to create a H264 session";
            return ;
        }
        // 设置实时编码输出，降低编码延迟
        VTSessionSetProperty(EncodingSession, kVTCompressionPropertyKey_RealTime, kCFBooleanTrue);
        //        DLog(@"set realtime  return: %d", (int)status);
        
        // h264 profile, 直播一般使用baseline，可减少由于b帧带来的延时
        VTSessionSetProperty(EncodingSession, kVTCompressionPropertyKey_ProfileLevel, kVTProfileLevel_H264_High_AutoLevel);
        //        DLog(@"set profile   return: %d", (int)status);
        
        // 设置编码码率(比特率)，如果不设置，默认将会以很低的码率编码，导致编码出来的视频很模糊
        VTSessionSetProperty(EncodingSession, kVTCompressionPropertyKey_AverageBitRate, (__bridge CFTypeRef)@(bt)); // bps
        VTSessionSetProperty(EncodingSession, kVTCompressionPropertyKey_DataRateLimits, (__bridge CFArrayRef)@[@(bt*2/8), @1]); // Bps
        //        DLog(@"set bitrate   return: %d", (int)status);
        
        // 设置关键帧间隔，即gop size
        VTSessionSetProperty(EncodingSession, kVTCompressionPropertyKey_MaxKeyFrameInterval, (__bridge CFTypeRef)@(3));
        //        DLog(@"set MaxKeyFrame  return: %d", (int)status);
        
        // 设置帧率，只用于初始化session，不是实际FPS
        VTSessionSetProperty(EncodingSession, kVTCompressionPropertyKey_ExpectedFrameRate, (__bridge CFTypeRef)@(fps));
        //        DLog(@"set framerate return: %d", (int)status);
        
        VTSessionSetProperty(EncodingSession, kVTCompressionPropertyKey_AllowFrameReordering, kCFBooleanFalse);
        
        //        // 开始编码
        status = VTCompressionSessionPrepareToEncodeFrames(EncodingSession);
        DLog(@" VTCompressionSessionPrepareToEncodeFrames  return: %d", (int)status);
    });
}

- (void) setEncodebt:(long)bitrate
{
    dispatch_sync(aQueue, ^{
        if (EncodingSession) {
            VTCompressionSessionCompleteFrames(EncodingSession, kCMTimeInvalid);
            VTSessionSetProperty(EncodingSession, kVTCompressionPropertyKey_DataRateLimits, (__bridge CFArrayRef)@[@(bitrate*2/8), @1]);
            VTCompressionSessionPrepareToEncodeFrames(EncodingSession);
        }
    });
}

- (void) encode:(CVImageBufferRef )imageBuffer
{
    if (!EncodingSession) {
        [self initEncode:_m_width height:_m_height framerate:_m_fps bitrate:_m_bt];
    }
    dispatch_sync(aQueue, ^{
        frameCount++;
        CMTime duration = CMTimeMake(frameCount, 1000);
        VTEncodeInfoFlags flags;
        
        OSStatus statusCode = VTCompressionSessionEncodeFrame(EncodingSession,
                                                              imageBuffer,
                                                              duration,
                                                              kCMTimeInvalid,
                                                              NULL, NULL, &flags);
        if (statusCode != noErr) {
            DLog(@"H264: VTCompressionSessionEncodeFrame failed with %d", (int)statusCode);
            _error = @"H264: VTCompressionSessionEncodeFrame failed ";
            [self unint];
            return;
        }
    });
}

- (void)dealloc
{
    dispatch_sync(aQueue, ^{
        [self unint];
    });
    
    [super dealloc];
}

- (void) unint
{
    if (EncodingSession) {
        VTCompressionSessionCompleteFrames(EncodingSession, kCMTimeInvalid);
        
        VTCompressionSessionInvalidate(EncodingSession);
        CFRelease(EncodingSession);
        EncodingSession = NULL;
    }
    _error = NULL;
}

@end
