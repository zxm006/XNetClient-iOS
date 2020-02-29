//
//  audioEng.cpp
//  StreamMedia
//
//  Created by zhangxinming on 2017/11/28.
//  Copyright © 2017年 zhangxinming. All rights reserved.
//

#include "audioEng.h"
#include "webrtc/modules/audio_mixer/audio_mixer_impl.h"
#include "webrtc/modules/audio_processing/aec/include/echo_cancellation.h"
#include "webrtc/modules/audio_processing/aecm/include/echo_control_mobile.h"
#include "webrtc/modules/audio_processing/ns/include/noise_suppression.h"
//#include "webrtc/modules/audio_mixer/audio_conference_mixer_defines.h"
namespace webrtc {
    
    namespace {

constexpr int kDefaultSampleRateHz = 16000;

// Utility function that resets the frame member variables with
// sensible defaults.
void ResetFrame(AudioFrame* frame) {
    frame->sample_rate_hz_ = kDefaultSampleRateHz;
    frame->num_channels_ = 1;
    
    // Frame duration 10ms.
    frame->samples_per_channel_ = kDefaultSampleRateHz / 100;
    frame->vad_activity_ = AudioFrame::kVadActive;
    frame->speech_type_ = AudioFrame::kNormalSpeech;
}

class MockMixerAudioSource : public AudioMixer::Source {
public:
    MockMixerAudioSource()
    
    
    : fake_audio_frame_info_(AudioMixer::Source::AudioFrameInfo::kNormal) {
        //        ON_CALL(*this, GetAudioFrameWithInfo(_, _))
        //        .WillByDefault(
        //                       Invoke(this, &MockMixerAudioSource::FakeAudioFrameWithInfo));
        //        ON_CALL(*this, PreferredSampleRate())
        //        .WillByDefault(Return(kDefaultSampleRateHz));
    }
    //
    
    //               void  AudioFrameInfo(int sample_rate_hz, AudioFrame* audio_frame);
    //
    //    MOCK_CONST_METHOD0(PreferredSampleRate, int());
    //    MOCK_CONST_METHOD0(Ssrc, int());
    //
    
    AudioFrameInfo GetAudioFrameWithInfo(int sample_rate_hz,
                                         AudioFrame* audio_frame)
    {
        AudioFrameInfo audioFrameInfo;
        return audioFrameInfo;
        
    }
    
    // A way for a mixer implementation to distinguish participants.
    int Ssrc() const
    {
        return 0;
    }
    
    // A way for this source to say that GetAudioFrameWithInfo called
    // with this sample rate or higher will not cause quality loss.
    int PreferredSampleRate() const
    {
        return 0;
        
    }
    
    AudioFrame* fake_frame() { return &fake_frame_; }
    AudioFrameInfo fake_info() { return fake_audio_frame_info_; }
    void set_fake_info(const AudioFrameInfo audio_frame_info) {
        fake_audio_frame_info_ = audio_frame_info;
    }
    
private:
    AudioFrameInfo FakeAudioFrameWithInfo(int sample_rate_hz,
                                          AudioFrame* audio_frame) {
        audio_frame->CopyFrom(fake_frame_);
        audio_frame->sample_rate_hz_ = sample_rate_hz;
        audio_frame->samples_per_channel_ =
        rtc::CheckedDivExact(sample_rate_hz, 100);
        return fake_info();
    }
    
    AudioFrame fake_frame_;
    AudioFrameInfo fake_audio_frame_info_;
};


class CustomRateCalculator : public OutputRateCalculator {
public:
    explicit CustomRateCalculator(int rate) : rate_(rate) {}
    int CalculateOutputRate(const std::vector<int>& preferred_rates) override {
        return rate_;
    }
    
private:
    const int rate_;
};

//AudioFrame frame_for_mixing;
//        void MixAndCompare(
//                           const std::vector<AudioFrame>& frames,
//                           const std::vector<AudioMixer::Source::AudioFrameInfo>& frame_info,
//                           const std::vector<bool>& expected_status)
//        {
//            const size_t num_audio_sources = frames.size();
//            RTC_DCHECK(frames.size() == frame_info.size());
//            RTC_DCHECK(frame_info.size() == expected_status.size());
//            
//            const auto mixer = AudioMixerImpl::Create();
//            std::vector<MockMixerAudioSource> participants(num_audio_sources);
//            
//            for (size_t i = 0; i < num_audio_sources; ++i) {
//                participants[i].fake_frame()->CopyFrom(frames[i]);
//                participants[i].set_fake_info(frame_info[i]);
//            }
//            
//            for (size_t i = 0; i < num_audio_sources; ++i) {
//                mixer->AddSource(&participants[i]);
//                //               participants[i], GetAudioFrameWithInfo(kDefaultSampleRateHz, _) ;
//            }
//            
//            mixer->Mix(1, &frame_for_mixing);
//            
//            //            for (size_t i = 0; i < num_audio_sources; ++i) {
//            //
//            //                          mixer->GetAudioSourceMixabilityStatusForTest(&participants[i]) ;
//            //            }
//        }}}

        
        
    }}
