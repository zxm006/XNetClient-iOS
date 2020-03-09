#ifndef OPENREMOTEUSER_H
#define OPENREMOTEUSER_H

#include "XNet/XNetMediaReceiver.h"
#include <string>
#import "liveFFmpegdecode.h"
#include <mutex>
// #import "AudioUnitAecTool.h"
 #import "AudioUnitAecMix.h"

class OpenRemoteUser
: public XNetMediaReceiverCallback
{
public:
    OpenRemoteUser(bool isVideoCalling);
    ~OpenRemoteUser();
public:
    bool ConnectMediaServer(std::string strServerIp, unsigned short nPort);
    void ReleaseMediaSever();
 
    void  SetAudioOrder(int order);
    
    bool OpenAudioReceive(unsigned long ulPeerUserAudioId);
    bool CloseAudioReceive();
    
    bool OpenVideoReceive(unsigned long ulPeerUserVideoId);
    bool CloseVideoReceive();
 
       void SetVideoWindow(void* pVideoWindow);
       void SetAudioMode(bool mixer);
      bool      m_isVideoCalling;//对讲
    
public:
	virtual void OnXNetMediaReceiverCallbackAudioPacket(unsigned char*pData,int nLen);
	virtual void OnXNetMediaReceiverCallbackVideoPacket(unsigned char*pData,int nLen);
 private:
 	bool CreateScreenReceiver();
 	void ReleaseReceiver();

    AudioUnitAecMix *m_audioUnitAecMix;
private:
	XNetMediaReceiver*	m_pMediaReceiver;
 
    	unsigned long           m_ulPeerVidoID ;
      liveFFmpegdecode *  m_liveFFmpegdecode;
	unsigned long           m_ulPeerAudioID ;			//AudioID
	std::string             m_peer_nodeid;
	std::string             m_peer_nataddr;
	std::string             m_peer_locaddr;
	int                     m_peer_locport;
	std::string	            m_peer_mcuid;
	std::string	            m_peer_mcuaddr;
	int		                m_peer_mcuport;
private:
    std::string             m_MediaServerIp;
	unsigned short          m_nPort;
    unsigned char*          m_data;
    unsigned long          m_length;
    std::recursive_mutex    m_mutex;
    void *aecmInst;
    int unitaAdioid;
//    AudioEng *m_AudioEng;
};
 
#endif 
