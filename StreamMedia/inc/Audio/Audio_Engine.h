//Audio_Engine.h
#ifndef __Audio_ENGINE_H__
#define __Audio_ENGINE_H__

#include <stdio.h>

#include "Audio_Export.h"

typedef struct{
	int		id;
	int		pl_type;
	char	pl_name[32];
	int		pl_freq;
	int		framesize;
	int		bitrate;
	char	desc[128];//g711u 64kbps
}Codecfactor;

class Audio_ENGINE_API OutPutSample
{
public:
	OutPutSample(void){};
	virtual~OutPutSample(void){};
public:
	virtual int GetSampleRate(void)=0;
	// true for ok, false for fail
	virtual bool Write(const short*pSamples,int nSamples)=0;
};

class Audio_ENGINE_API OutPutStream
{
public:
	OutPutStream(void){};
	virtual~OutPutStream(void){};
public:
	// true for ok, false for fail
	virtual bool Write(const void*data,int len)=0; 
};

class Audio_ENGINE_API RecSamples
{
public:
	RecSamples(void){};
	virtual~RecSamples(void){};
public:
	virtual void OnRecordSamples(const short*pSamples,int nSamples,int nSampleRate,int nTimestamp)=0;
};

class Audio_ENGINE_API PlayChannel;
class Audio_ENGINE_API CapturChannel
{
public:
	CapturChannel(void){};
	virtual~CapturChannel(void){};
public:
	virtual int Connect(Codecfactor*pCodecInst)=0;
	virtual void ReleaseConnections(void)=0;

	virtual int SetCodec(Codecfactor*pCodecInst)=0;
	virtual int GetCodec(Codecfactor*pCodecInst)=0;

	virtual int StartCapture(void)=0;
	virtual int StopCapture(void)=0;
	//<0:error 0:false,else true
	virtual int GetCapturing(void)=0;

	//<0:error >=0:level[0-10]
	virtual int GetLevel(void)=0;

	//<0:error else bps
	virtual int GetBitrate(void)=0;

	//<0:error 0:ok nVAD[0:disable,1:enable]
	//virtual int SetVAD(int nVAD)=0;
	//<0:error 0:disable 1:enable
	//virtual int GetVAD(void)=0;

	virtual int AddStreamOut(OutPutStream*pStreamOut)=0;
	virtual int RemoveStreamOut(OutPutStream*pStreamOut)=0;

	//???????5?????
	//volume????¦¶0-20 0:??§³ 20:??????}????10:???????
	virtual int SetVolume(int volume)=0;//0-10-20
	virtual int GetVolume(void)=0;//0-10-20

	//???????;??????????
	virtual int StartMixingWithPlayChan(PlayChannel*pPlayChan)=0;
	virtual int StopMixingWidthPlayChan(PlayChannel*pPlayChan)=0;

	virtual int SetRecordSamples(RecSamples*pRecordSamples)=0;
};

class Audio_ENGINE_API CapturChannelExternal
	: public CapturChannel
{
public:
	CapturChannelExternal(void){};
	virtual~CapturChannelExternal(void){};
public:
	virtual int GetBufferedTime(void)=0;//ms
	virtual int WriteSamples(short*pSamples,int nSamples,int nSampleRate,bool bDiscontinious)=0;
};

class Audio_ENGINE_API PlayChannel
	: public OutPutStream
{
public:
	PlayChannel(void){};
	virtual~PlayChannel(void){};
public:
	virtual int Connect(unsigned long*lpnVidFrameTimestamp=(unsigned long*)NULL,unsigned long*lpnAudFrameTimestamp=(unsigned long*)NULL)=0;
	virtual void ReleaseConnections(void)=0;

	virtual int GetCodec(Codecfactor*codec_inst)=0;

	virtual int SetVolume(int volume)=0;//0-10-20
	virtual int GetVolume(void)=0;//0-10-20

	//<0:error 0:ok mute[0:disable 1:enable]
	virtual int SetMute(int mute)=0;
	//<0:error else mute[0:disable 1:enable]
	virtual int GetMute(void)=0;

	//<0:error level[0-10]
	virtual int GetLevel(void)=0;//0-10

	//<0:error else bps
	virtual int GetBitrate(void)=0;

	//<0:error else 0:100
	virtual int GetLostPackets(void)=0;

	//<0:error else 0:100
	virtual int GetLostPacketsPercent(void)=0;

	//<0:error 0:ok status[0:disable 1:enable]
	virtual int SetJitterBuffer(int value)=0;
	//<0:error else status[0:disable 1:enable]
	virtual int GetJitterBuffer(void)=0;
	//min[60-3000]
	virtual int SetJitterMin(int min)=0;
	virtual int GetJitterMin(void)=0;
	//max[5000-10000]
	virtual int SetJitterMax(int max)=0;
	virtual int GetJitterMax(void)=0;

	virtual int AddStreamOut(OutPutStream*pStreamOut)=0;
	virtual int RemoveStreamOut(OutPutStream*pStreamOut)=0;

	virtual int AddSampleOut(OutPutSample*pSampleOut)=0;
	virtual int RemoveSampleOut(OutPutSample*pSampleOut)=0;

	virtual int GetBufferedTime(void)=0;

	//????PCM?????
	//pSamples,PCM?????
	//nSamples,?????
	//nSampleRate,????,????????8000HZ??16000HZ
	//???????0
	//??????-1
	virtual int PlaySamples(short*pSamples,int nSamples,int nSampleRate,unsigned short usSeqNo)=0;

	//??????????,nFlag:0???????????,nFlag:1?????????
	virtual int SetSynFlag(int nFlag)=0;

	virtual int SetRecordSamples(RecSamples*pRecordSamples)=0;
};

class Audio_ENGINE_API RtpCall
{
public:
	RtpCall(void){}
	virtual~RtpCall(void){};
public:
	virtual int Connect(const char*cszLocalIP,unsigned short usLocalPort)=0;
	virtual void ReleaseConnections(void)=0;

	virtual int SetRemoteSDP(int nPayloadType,const char*cszPayloadName,const char*cszIP,unsigned short usPort)=0;

	virtual CapturChannel*GetCapChan(void)=0;
	virtual PlayChannel*GetPlayChan(void)=0;

	virtual int SendDTMF(int nPayloadType,char dtmf)=0;
};


class Audio_ENGINE_API Engine
{
public:
	Engine(void){};
	virtual~Engine(void){};
public:
	//0:ok,<0:error,nAECType:0-4,0:????4:???
	static int Initialize(int nAECType=4,int nUsedByRS=0);
	//0:ok,<0:error
	static int Terminate(void);

	//0:no dev,>0:dev count <0:error
	static int GetMicrophoneNum(void);
	//0:ok,<0:error
	static int GetMicrophoneDev(int dev,char desc[]);

	//0:no dev,>0:dev count <0:error
	static int GetSpeakerNum(void);
	//0:ok,<0:error
	static int GetSpeakerDev(int dev,char desc[]);

	//0:no codec,>0:codec count,<0:error
	static int	GetCodecID(void);
	//0:ok,<0:error
	static int	GetCodecByIndex(int index,Codecfactor*pCodecInst);
	//0:ok,<0:error
	static int	GetAudioCodecByID(int id,Codecfactor*pCodecInst);


	virtual int Audio_SetAEC(int nAEC)=0;//0:diable aec,1:enable aec
	virtual int Audio_GetAEC(void)=0;
	virtual int Audio_GetAECQuality(void)=0;//0:very good,1:good,2:poor,3:very poor

	//??????Mic????????;nVolume[0-100]0:??§³,1:???
	virtual int Audio_SetDefaultMicVolume(int nVolume)=0;
	virtual int Audio_GetDefaultMicVolume(void)=0;

	//?????????????,?????????????;nAGC????¦¶(0-1),0:????,1:??;????1
	virtual int Audio_SetPreAGC(int nAGC)=0;
	virtual int Audio_GetPreAGC(void)=0;

	//??????????????,??????????????????;nAGC????¦¶(0-12),0:????,12:???12??;????4
	virtual int Audio_SetPostAGC(int nAGC)=0;
	virtual int Audio_GetPostAGC(void)=0;

	//???????????;nNS????¦¶(0-6),0:????,6:???;????6
	virtual int Audio_SetPreNS(int nNS)=0;
	virtual int Audio_GetPreNS(void)=0;

	//??????;nVAD????¦¶(0-1),0:????,1:??
	virtual int Audio_SetVAD(int nVAD)=0;
	virtual int Audio_GetVAD(void)=0;

	//???4?;nFEC????¦¶(0-1),0:????,1:??
	virtual int Audio_SetFEC(int nFEC)=0;
	virtual int Audio_GetFEC(void)=0;

	virtual int Audio_SetSpeakerDev(int nDev,int nSteroPlay=0)=0;
	virtual int Audio_SetMicDev(int nDev,int nMicBoost=0)=0;

	virtual int Audio_CreateCapChan(CapturChannel**ppCapChan)=0;
	virtual int Audio_CreateCapChanExternal(CapturChannelExternal**ppCapChanExternal)=0;
	virtual int Audio_CreatePlayChan(PlayChannel**ppPlayChan)=0;


	//dtmf['0'-'9','#','*']
	virtual int Audio_PlayDTMF(char dtmf)=0;

	virtual int Audio_CreateRTPCall(RtpCall**ppRTPCall)=0;

	//for debug only
	//record pcm raw 16khz mono
	virtual int Audio_StartRecordPreAEC(FILE*hFile)=0;
	virtual int Audio_StopRecordPreAEC(void)=0;
	virtual int Audio_StartRecordPostAEC(FILE*hFile)=0;
	virtual int Audio_StopRecordPostAEC(void)=0;

	//????DTMF??????????????RFC2833??INBAND??}??
#define	Audio_DTMF_RFC2833			0
#define Audio_DTMF_INBAND			1
	virtual int Audio_SetDTMFType(int nType)=0;
	virtual int Audio_GetDTMFType(void)=0;

	//????????;nCNG????¦¶(0-1),0:????,1:??
	virtual int Audio_SetCNG(int nCNG)=0;
	virtual int Audio_GetCNG(void)=0;
};

Audio_ENGINE_API Engine&GetAlgorithmicEngine(void);

#endif


