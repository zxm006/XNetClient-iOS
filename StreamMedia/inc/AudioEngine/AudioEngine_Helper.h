#ifndef AUDIO_ENGINE_API_H
#define AUDIO_ENGINE_API_H

#include "AudioEngine.h"

#ifdef WIN32
#ifdef NDEBUG
//#pragma comment(lib, "../AudioEngineSDK/Release/AudioEngine.lib")
#else
#pragma comment(lib, "../../AudioEngineSDK/Debug/AudioEngine.lib")
#endif  //debug
#endif


/*说明:
* 该文件不属于AudioEngine 内部文件，用于第三方程序引用AudioEngine时的
* 快速调用文件，第三方程序只需要在其代码中增加以下代码:

 //#include "AudioEngine_Helper.h"

 AudioEngine_Helper the_audio_engine_instance_;

 AudioEngine *audio_engine = AudioEngine_Helper::Instance().getAudioEngine();

 audio_engine->Init();
 //使用语音引擎
 .... 
*
*/


class AudioEngine_Helper;
/*在调用的app 中定义该实例*/
extern	AudioEngine_Helper the_audio_engine_instance_;


class AudioEngine_Helper{

public:
	bool enable_fec;
	static AudioEngine_Helper* Instance()
	{ 
		return &the_audio_engine_instance_; 
	};

public:
	AudioEngine_Helper():
	  audio_engine_(CreateAudioEngine()),
	  enable_fec(false)
	  {
		  audio_engine_->Init();

		  /*开启默认设置*/
		  audio_engine_->VOE_SetAECMode(1,kEcDefault);
		  audio_engine_->VOE_SetAGCMode(1,kAgcDefault);
		  audio_engine_->VOE_SetNSMode(1,kNsHighSuppression);
	  };

	  ~AudioEngine_Helper()
	  {
		  if(audio_engine_->IsInitialized())
				  audio_engine_->Terminate();

		  DeleteAudioEngine(audio_engine_);
	  };

	  AudioEngine *getAudioEngine(){ 
		  return audio_engine_ ;
	  };
private:
	AudioEngine *audio_engine_;
};

#endif