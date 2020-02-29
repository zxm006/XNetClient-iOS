#include "OpenRemoteUser.h"
#include <XNet/AUDEC_Header.h>
#include <XNet/AUDEC_CodecID.h>
#include <XNet/VIDEC_Header.h>

#import "AudioUnitTool.h"
#include "webrtc/modules/audio_processing/aecm/include/echo_control_mobile.h"
#import"AudioUnitManager.h"

OpenRemoteUser::OpenRemoteUser(bool isVideoCalling)
: m_ulPeerAudioID(0)
, m_pMediaReceiver(NULL)
, m_MediaServerIp("")
, m_peer_locport(0)
, m_peer_mcuport(0)
, m_peer_mcuid("MCU001")/*对方MCUID*/
,m_liveFFmpegdecode(nil)
,m_isVideoCalling(isVideoCalling)
,m_AudioPlay(nil)
,aecmInst(NULL)
,unitaAdioid(0)
{
    if (m_isVideoCalling) {
        
        //       aecmInst =  WebRtcAecm_Create();
//                WebRtcAecm_Init(aecmInst, 16000);
        //
        //        AecmConfig config;
        //        config.echoMode = 3;
        //        WebRtcAecm_set_config(aecmInst, config);
    }
    else {
//            m_AudioPlay = [[AudioPlay alloc]init];
//             [ m_AudioPlay initOpenAL];
//              [m_AudioPlay LoudSpeaker:1];
      
        m_liveFFmpegdecode = [[liveFFmpegdecode alloc] init];
    }
}

void OpenRemoteUser::SetAudioOrder(int order)
{
    unitaAdioid = order;
}

void OpenRemoteUser::SetAudioMode(bool mixer)
{
    m_audioUnitAecMix = [[AudioUnitAecMix alloc]init];
    [m_audioUnitAecMix startAudio:mixer];
}

OpenRemoteUser::~OpenRemoteUser()
{
    if(m_AudioPlay) {
        [m_AudioPlay release];
        m_AudioPlay =nil;
        
    }
    KAutoLock lock(m_mKCritSec);
    
    if (m_liveFFmpegdecode) {
        [m_liveFFmpegdecode Endinmpeg_decode_h264];
        [m_liveFFmpegdecode release];
        m_liveFFmpegdecode =nil;
    }
}

bool OpenRemoteUser::ConnectMediaServer(std::string strServerIp, unsigned short nPort)
{
    m_MediaServerIp = strServerIp;
    m_nPort        = nPort;
    m_peer_mcuid   = "MCU001";
    m_peer_mcuaddr = strServerIp;
    m_peer_nataddr = strServerIp;
    m_peer_locaddr = strServerIp;
    m_peer_mcuport = nPort;
    m_peer_locport = nPort;
    
    if(!CreateScreenReceiver())
        return false;
    return true;
}

void OpenRemoteUser::ReleaseMediaSever()
{
    ReleaseReceiver();
}

bool OpenRemoteUser::CreateScreenReceiver()
{
    m_pMediaReceiver = XNetMediaReceiver::Create(*this);
    if (m_pMediaReceiver!=NULL) {
        m_pMediaReceiver->SetLocalMCUID("MCU001");
        m_pMediaReceiver->SetLocalMCIIP(m_MediaServerIp.c_str());
        m_pMediaReceiver->SetLocalMCUPort(m_nPort);
        
        if (0!=m_pMediaReceiver->Open(m_peer_nodeid.c_str(), m_peer_nataddr.c_str(), m_peer_locaddr.c_str(),m_peer_locport, m_peer_mcuid.c_str(), m_peer_mcuaddr.c_str(), m_peer_mcuport)) {
            m_pMediaReceiver->Close();
            delete m_pMediaReceiver;
            m_pMediaReceiver=NULL;
            return false;
        }
        return true;
    }
    return false;
}

bool OpenRemoteUser::OpenAudioReceive(unsigned long ulPeerUserAudioId)
{
    m_ulPeerAudioID = ulPeerUserAudioId;
    if (m_pMediaReceiver==NULL || m_ulPeerAudioID==0) {
        return false;
    }
    if(0!=m_pMediaReceiver->SetAudioID((unsigned int)m_ulPeerAudioID))
        return false;
    if(0!=m_pMediaReceiver->StartAudio())
        return false;
    return true;
}

bool OpenRemoteUser::CloseAudioReceive()
{
    if (m_pMediaReceiver==NULL || m_ulPeerAudioID==0) {
        return false;
    }
    m_pMediaReceiver->StopAudio();
    return true;
}


bool OpenRemoteUser::OpenVideoReceive(unsigned long ulPeerUserVideoId)
{
    m_ulPeerVidoID = ulPeerUserVideoId;
    if (m_pMediaReceiver==NULL || m_ulPeerVidoID==0) {
        return false;
    }
    if(0!=m_pMediaReceiver->SetVideoID((unsigned int)m_ulPeerVidoID))
        return false;
    if(0!=m_pMediaReceiver->StartVideo())
        return false;
    
    return true;
}

bool OpenRemoteUser::CloseVideoReceive()
{
    if (m_pMediaReceiver==NULL || m_ulPeerVidoID==0) {
        return false;
    }
    m_pMediaReceiver->StopVideo();
    return true;
}

void OpenRemoteUser::ReleaseReceiver()
{
    if (m_pMediaReceiver!=NULL) {
        m_pMediaReceiver->Close();
        delete m_pMediaReceiver;
        m_pMediaReceiver=NULL;
    }
}

void OpenRemoteUser::OnXNetMediaReceiverCallbackAudioPacket(unsigned char*pData,int nLen)
{
    if(pData != NULL && nLen>8 ) {
        int nHeaderSize = AUDEC_HEADER_GET_LEN(pData);
        unsigned char* pAudio = pData + nHeaderSize;
        int len = nLen - nHeaderSize;
        ////保存数据用于混音
        if(m_isVideoCalling) {
//            [[AudioUnitAecTool shareHandle]setaudiodata:(unsigned char*)pAudio length:len];
        }
        else {
        [[AudioUnitManager shareHandle]setaudiodata:pAudio length:len  audioid:unitaAdioid];
//            [m_AudioPlay openAudioFromQueue:pAudio dataSize:len];
        }
    }
}

void OpenRemoteUser::SetVideoWindow(void* pVideoWindow)
{
    if (pVideoWindow&&m_liveFFmpegdecode) {
        [m_liveFFmpegdecode SetLocalVideoWindow:pVideoWindow];
        [m_liveFFmpegdecode Beginmpeg_decode_h264];
    }
    else if(m_liveFFmpegdecode) {
        [m_liveFFmpegdecode Endinmpeg_decode_h264];
    }
}

void OpenRemoteUser::OnXNetMediaReceiverCallbackVideoPacket(unsigned char*pData,int nLen)
{
    KAutoLock lock(m_mKCritSec);
    if(pData != NULL && nLen>16 ) {
        unsigned int timestamp = AUDEC_HEADER_GET_TIMESTAMP(pData);
        //      NSLog(@"Video timestamp = %u\n\r",timestamp);
        int nHeaderLen=VIDEC_HEADER_EXT_GET_LEN(pData);
        unsigned char* pVideo = pData + nHeaderLen;
        int len = nLen - nHeaderLen;
        if(m_liveFFmpegdecode) {
            [m_liveFFmpegdecode ffmpeg_decode_h264:pVideo Len:len iCount:1];
        }
    }
}



