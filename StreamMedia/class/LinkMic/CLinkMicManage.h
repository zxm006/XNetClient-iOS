#ifndef _UU_CAPP_LinkMicMANAGE_H_
#define _UU_CAPP_LinkMicMANAGE_H_

#include "ConnectServer.h"
#include "CritSec.h"
#include "AutoLock.h"
#include "uuDefine.h"
#include "OpenLocalUser.h"
#include <list>
#include <vector>

#include <pthread/pthread.h>
#include "CritSec.h"
#define SIZE_AUDIO_FRAME (2)
@class TouchMoveView;
@class NSMutableArray;

#include "uuDefine.h"

class ILinkMicCallback
{
public:
    virtual void IConnectStatusCallback(CONNECT_NET_STATUS cs) = 0;
    virtual void INetReceiveUserList(CLIENTUSERINFOLIST_MAP& UserInfoList) = 0;
    virtual void INetReceiveUserLogin(unsigned long uPeerUserID, std::string strName,unsigned long seraudioID) = 0;
    virtual void INetReceiveUserLogOut(unsigned long uPeerUserID) = 0;
    virtual void INetReceiveData(unsigned long uPeerUserID, std::string strName,std::string strData, unsigned long nLen) = 0;
    virtual void INetBroadcastData(unsigned long uPeerUserID, const char* pData, unsigned long nLen) = 0;
};

class CLinkMicManage
:public CmdTCPClientCallback,ISendCmdServer
{
public:
    CLinkMicManage();
    ~CLinkMicManage(void);
public:
    void  loginLinkMicServer(const char* szIP,const char* szName ,const char* szGameId,const char* szGameServerId,const char* szRoomId,const char* szGroupId="",const char* szexpand= "",bool isencrypt = true,bool iscapscreen = NO,bool isAnchor = NO,bool ishaveVideo = YES);

    void setNetWorkCallback(ILinkMicCallback*netWorkCallback);
     int getUserList();
     int logoutServer();
    void disconnect();
    virtual void SendAudioData(char*pData, int nLen);
    
    void  OpenRemoteUserVideo(const char* username);
    void  closeRemoteUserVideo(const char* username);
   
    virtual int SendData(KCmdPacketEx& pPacket);
   
     int SendDataToUser(unsigned long uPeerUserID ,const char* pData, unsigned long nLen);
    void resetLocalVideo();
private:
    
    void openVideo();
    void closeVideo();
    
    virtual void On_SessionConnectStatus(CONNECT_NET_STATUS cs);
    virtual  int SendAllUserData(const char* pData, unsigned long nLen);
    
       int SendLinkMic(const char* pData, unsigned long nLen,bool isinroom);
 
    virtual std::string GetLocalIP();
    virtual std::string GetNATIP();
    
    int LoginServer(const char* szName, const char* szPassword);
    int ConnectServer(const char* szIP,unsigned int nPort ,bool isEncrypt);
    
private:
    virtual void OnDispatchCmd(KCmdPacketEx& pPacket);
private:
    
    std::string m_serverip;
    std::string m_username;
    std::string m_szexpand;
    std::string m_strAddress;
    std::string m_userGroupId;
     TouchMoveView *m_touchMoveView;
    NSMutableArray *m_peertouchMoveViews;
    
    unsigned long  m_audioID;
    unsigned long  m_uUserID;
    bool m_ishasVideo;
    
    OpenLocalUser * m_OpenLocalUser;
    std::map<unsigned long, class OpenRemoteUser*> m_pOpenRemoteUser_map;
     CConnectServer             m_ConnectServer;
    
    ILinkMicCallback*          m_pINetWorkCallback;
    CLIENTUSERINFOLIST_MAP     m_UserInfoList;
    
    KCritSec m_mKCritSec;
    bool m_isAnchor;
    void setupSession();
    void openMediaSender(bool isCapScreen);
    void closeMediaSender();
    
    void openLocalAudio();
    void closeLocalAudio();
  
    void StopAllRemoteMedia();
    bool m_iscapscreen;
    void* creatVideoWindow(int x,int y,int w,int h);
};

#endif
