//
//  audioEng_webrtc.h
//  StreamMedia
//
//  Created by zhangxinming on 2017/12/13.
//  Copyright © 2017年 zhangxinming. All rights reserved.
//

#ifndef audioEng_webrtc_h
#define audioEng_webrtc_h

#include <stdio.h>
#include "webrtc/modules/audio_mixer/audio_mixer_impl.h"
#include "webrtc/modules/audio_processing/aec/include/echo_cancellation.h"
#include "webrtc/modules/audio_processing/aecm/include/echo_control_mobile.h"
#include "webrtc/modules/audio_processing/ns/include/noise_suppression.h"

struct SAudioData
{
    int length;
 
    char* data;
};

using namespace webrtc;


    char* MixAudioFrame(std::vector<SAudioData*> VaudioData);
 
#endif /* audioEng_webrtc_h */
