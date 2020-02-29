#ifndef __Audio_MICCTRL_H__
#define __Audio_MICCTRL_H__

#include "Audio_Export.h"

class Audio_ENGINE_API MicrophoneControl
{
public:
	MicrophoneControl(void);
	virtual ~MicrophoneControl(void);
public:
	typedef enum {
		MIC=0,
		LINE_IN,
		WAVE_OUT_MIXER,
		UNKOWN
	}MIC_INPUT;

	virtual void Attach(void);
	virtual void Detach(void);

	virtual int SetMicrophoneBoost(int nBoost);
	virtual int GetMicrophoneBoost(void);

	virtual int SetMicrophoneInputType(MicrophoneControl::MIC_INPUT nType);
	virtual MicrophoneControl::MIC_INPUT GetMicrophoneInputType(void);

	virtual int SetMicrophoneVolume(int nVolume);
	virtual int GetMicrophoneVolume(void);

	virtual void OnMicrophoneBoostChange(int nBoost);//nBoost: 0|1
	virtual void OnMicrophoneInputChange(MicrophoneControl::MIC_INPUT nType);
	virtual void OnMicrophoneVolumeChange(int nVolume);//nVolume:0-100
};


#endif