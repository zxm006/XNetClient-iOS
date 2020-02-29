#ifndef AUDIO_ENGINE_H
#define AUDIO_ENGINE_H

#include "me_common_types.h"

using namespace me;

class AudioEngine
{
public:
	virtual int Init() = 0;
	virtual bool IsInitialized() = 0;
	virtual int Terminate() = 0;

	/*语音通道调用接口*/
	virtual int VOE_CreateChannel() = 0;
	virtual int VOE_SetConnectInfo(int channel,int recv_port,const char *remote_ip, int remote_port,int payload,bool use_external_transport) = 0;
	virtual int VOE_SetChannelSendCodec(int channel,int codec_payload) = 0;
	virtual int VOE_Connect(int channel) = 0;
	virtual int VOE_Disconnect(int channel) = 0;
	virtual int VOE_DeleteChannel(int channel) = 0;
	virtual int VOE_GetCurChannelId() = 0;    //v2
    
	/*音效: 回声消除/自动增益控制/降噪*/
	virtual int VOE_SetAECMode(bool enabled,EcModes mode) = 0;
	virtual int VOE_SetAGCMode(bool enabled,AgcModes mode) = 0;
	virtual int VOE_SetNSMode(bool enabled,NsModes mode) = 0;

	/*音频编解码信息*/
	virtual int VOE_NumOfCodecs() = 0;
	virtual int VOE_GetCodecOfIndex(int index, CodecInst &codec) = 0;
	
	/*设置mic和spk音量*/
	virtual int VOE_SetSpeakerVolume(int volume) = 0;
	virtual int VOE_SetMicVolume(int volume) = 0;
	virtual unsigned int VOE_GetSpeakerVolume() = 0;   //v2
	virtual unsigned int VOE_GetMicVolume() = 0;       //v2
	virtual unsigned int VOE_GetSpeechInputLevel() = 0;   //v2
	virtual unsigned int VOE_GetSpeechOutputLevel(int channel) = 0;  //v2

	/*声卡操作接口*/
	virtual int VOE_GetNumOfPlayoutDevices() = 0;
	virtual int VOE_GetNumOfRecordingDevices() = 0;
	virtual int VOE_GetRecordingDeviceName(int index, char strNameUTF8[128], char strGuidUTF8[128]) = 0;
	virtual int VOE_GetPlayoutDeviceName(int index, char strNameUTF8[128], char strGuidUTF8[128]) = 0;
	virtual int VOE_GetPlayoutDeviceStatus(bool& isAvailable ) = 0;
	virtual int VOE_GetRecordingDeviceStatus(bool& isAvailable ) = 0;
	virtual int VOE_SetPlayoutDevice(int index) = 0;
	virtual int VOE_SetRecordingDevice(int index) = 0;

	/*收到rtp/rtcp数据发送给VoE，仅限外部传输接口模式*/
	virtual int VOE_ReceivedRTPPacket(int channel, const char *buf,int len) = 0;
	virtual int VOE_ReceivedRTCPPacket(int channel, const char *buf,int len) = 0;
	/*开启传输保护*/
    virtual int VOE_SetProtectionFEC(int channel, bool yesno) = 0;

	/*发送带外/带内 dtmf信号*/
	virtual int VOE_SendTelephoneEvent(int channel, unsigned char eventCode,bool outBand, bool play_dtmf_tone_locally) = 0;
	
	/*注册外部传输接口*/
	virtual int VOE_RegisterExternalTransport(int channel, Transport *external_transport) = 0;
	virtual int VOE_DeRegisterExternalTransport(int channel) = 0;

	/*手机扬声器 for mobile version*/
	virtual int VOE_SetLoudspeakerStatus(bool yesno) = 0;
	virtual int VOE_GetCallStatistics(int channel,me::NetworkStatistics &stats)=0;    //v2

protected:
	AudioEngine(){};
	virtual ~AudioEngine(){};

};

#ifdef __cplusplus
extern "C" {
#endif

#ifdef ANDROID
	/*设置 Jave VM 环境*/
	AUDIO_ENGINE_DLLEXPORT void VoE_SetAndroidObjects(void* javaVM, void *env, void* context);
#endif // ANDROID

	//创建/销毁引擎
	AUDIO_ENGINE_DLLEXPORT AudioEngine* CreateAudioEngine();
	AUDIO_ENGINE_DLLEXPORT bool DeleteAudioEngine(AudioEngine*& AudioEngine);
#ifdef __cplusplus
};
#endif

#endif //AUDIO_ENGINE_H