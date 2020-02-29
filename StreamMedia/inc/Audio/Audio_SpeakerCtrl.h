#ifndef __Audio_SPEAKERCTRL_H__
#define __Audio_SPEAKERCTRL_H__

#include "Audio_Export.h"

class Audio_ENGINE_API SpeakControl
{
public:
	SpeakControl(void);
	virtual ~SpeakControl(void);
public:
	virtual void Attach(void);
	virtual void Detach(void);

	virtual int SetSpeechNoVol(int nMute);
	virtual int GetSpeechNoVol(void);

	virtual int SetSpeechSound(int nVolume);
	virtual int GetSpeechSound(void);

	virtual void OnSpeechNoVolchanged(int nMute);//nMute: 0|1
	virtual void OnSpeechSoundChanged(int nVolume);//nVolume:0-100
};


#endif