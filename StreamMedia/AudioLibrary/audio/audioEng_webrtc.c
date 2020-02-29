//
//  audioEng_webrtc.c
//  StreamMedia
//
//  Created by zhangxinming on 2017/12/13.
//  Copyright © 2017年 zhangxinming. All rights reserved.
//

#include "audioEng_webrtc.h"
using namespace webrtc;

namespace {
 
 class MockMixerAudioSource : public AudioMixer::Source {
        public:
            MockMixerAudioSource()
         : fake_audio_frame_info_(AudioMixer::Source::AudioFrameInfo::kNormal) {
                
            }
     
            AudioFrameInfo GetAudioFrameWithInfo(int sample_rate_hz,
                                                 AudioFrame* audio_frame)
            {
                AudioFrameInfo audioFrameInfo;
                return audioFrameInfo;
                
            }
 
            int Ssrc() const
            {
                return 0;
            }
 
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
        

        char*  MixAudio(std::vector<SAudioData*> VaudioData)
        {
            
            const std::vector<AudioFrame>  frames;
            const std::vector<AudioMixer::Source::AudioFrameInfo>  frame_info;
            const std::vector<bool>  expected_status;
            AudioFrame  frame_for_mixing
            ;
            const size_t num_audio_sources = frames.size();
 
               const auto mixer = AudioMixerImpl::Create();

            std::vector<MockMixerAudioSource> participants(num_audio_sources);
            
            for (size_t i = 0; i < num_audio_sources; ++i)
            {
                participants[i].fake_frame()->CopyFrom(frames[i]);
                participants[i].set_fake_info(frame_info[i]);
            }
            
            for (size_t i = 0; i < num_audio_sources; ++i)
            {
                mixer->AddSource(&participants[i]);
                
            }
            
            mixer->Mix(1, &frame_for_mixing);
            int16_t* fake_frame_data = frame_for_mixing.mutable_data();
            return (char*)fake_frame_data;
        }
}

 char* MixAudioFrame(std::vector<SAudioData*> VaudioData)
{
   return  MixAudio(VaudioData);
}

