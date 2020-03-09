#ifndef _UU_CAPP_LinkMicMANAGE_H_
#define _UU_CAPP_LinkMicMANAGE_H_

#include "ConnectServer.h"
#include "uuDefine.h"
#include "OpenLocalUser.h"
#include <list>
#include <vector>
#include <mutex>
#include <pthread/pthread.h>

#define SIZE_AUDIO_FRAME (2)
@class TouchMoveView;
@class NSMutableArray;
#import "AudioUnitAecMix.h"
#include "uuDefine.h"

class IVideoChatCallback
{
public:
    virtual void  UserVideoView(UIView* view   ,NSString*userid)=0;
    virtual void UserMediaStatus(std::string strName,bool isvideo,bool isopen) = 0;
    virtual void  MtgInfoReg(MTG_CMD mtgCmd,bool result,std::string &info) =0;
    virtual void  MtgInfo(MTG_CMD mtgCmd,std::string username ,std::string &info) =0;
    virtual void  MtgInvite(std::string username ,std::string mtgId) =0;
    virtual void  MtgJoin(CLIENTUSERINFOLIST &userinfo) =0;
    virtual void  MtgExit(CLIENTUSERINFOLIST &userinfo) =0;
    virtual void  MtgUserList(CLIENTUSERINFOLIST_MAP& UserInfoList) =0;
    
    virtual void IConnectStatusCallback(CONNECT_NET_STATUS cs) = 0;
    
    virtual void INetReceiveUserList(CLIENTUSERINFOLIST_MAP& UserInfoList) = 0;
    virtual void INetReceiveUserLogin(unsigned long uPeerUserID, std::string strName,unsigned long seraudioID) = 0;
    virtual void INetReceiveUserLogOut(unsigned long uPeerUserID) = 0;
    virtual void INetReceiveData(unsigned long uPeerUserID, std::string strName,std::string strData, unsigned long nLen) = 0;
    virtual void INetBroadcastData(unsigned long uPeerUserID, const char* pData, unsigned long nLen) = 0;
 
};


class CVideoChat
:public CmdTCPClientCallback,ISendCmdServer
{
public:
    CVideoChat();
    ~CVideoChat(void);
public:
    void LoginServer(const char* szIP,const char* szName , const char* headUrl,
                     const char* nickName,double latitude,double longitude,const char* szGameId,const char* szGameServerId,const char* szRoomId,const char* szGroupId="",const char* szexpand= "",bool isencrypt = true);
    int logoutServer();
    
    void setNetWorkCallback(IVideoChatCallback*netWorkCallback);

    void  OpenRemoteUserVideo(const char* username ,int x,int y,int width,int height,bool round);
    void  setRemoteUserVideoFrame(const char* username ,int x,int y,int width,int height,bool round);
    void  closeRemoteUserVideo(const char* username);
    
    void OpenRemoteUserAudio(const char* username);
    void closeRemoteUserAudio(const char* username);
    
     int SendDataToUser(unsigned long uPeerUserID ,const char* pData, unsigned long nLen);
    void switchCamera();
    
    void  openLocalVideo();
    void  closeLocalVideo();
    void  openBeautify();
   void  closeBeautify();
    
    void openLocalAudio();
    void closeLocalAudio();
    
    void disconnect();
    
    ////////视频会议
    
    void reOrientation();
    
   void creatMtg(std::string mtgid, bool openCamera,MTG_TYPE mtgType);
    void joinMtg(std::string mtgid, bool openCamera);
    void exitMtg();
    void destroyMtg();
    
    void setAudioSession();
    int  mtgInviteUser(const char* mtgId, const char* iuserId);
    
    void  setbeautifyFace(bool beautifyFace);
    
    void  videoChatSnapshot(id<VideoChatDelegate> delegate);
    virtual void SendAudioData(char*pData, int nLen);
 
private:
    virtual void OnDispatchCmd(KCmdPacketEx& pPacket);
  
private:
    int  SendMtgCmd(MTG_CMD mtgCmd, std::string mtgid);
    
    void openMediaSender();
    void closeMediaSender();
    
    void  openLocalMedia(bool isCapScreen);
    void  startAudioUnit();
    
    
    void StopAllRemoteMedia();

    int getUserList();

    virtual int SendData(KCmdPacketEx& pPacket);
    int sendloginServer(const char* szName, const char* szPassword);
    int ConnectServer(const char* szIP,unsigned int nPort ,bool isEncrypt);
    void* creatVideoWindow(int x,int y,int w,int h);
    int  SendUserMediaStatus(bool isvideo, bool isopen );
    virtual void On_SessionConnectStatus(CONNECT_NET_STATUS cs);
    virtual  int SendAllUserData(const char* pData, unsigned long nLen);
   void OnDispatchMtgCmd(KCmdPacketEx& pPacket);
    void OnDispatchMtgCmdReg(KCmdPacketEx& pPacket);
    void MtgInfoReg(MTG_CMD mtgCmd,bool result,std::string &info);
    void MtgInfo(MTG_CMD mtgCmd,std::string username ,std::string &info);
   
private:
    
    virtual std::string GetLocalIP();
    virtual std::string GetNATIP();
    std::string m_serverip;
    std::string m_username;
    std::string m_szexpand;
    std::string m_strAddress;
    std::string m_userGroupId;
    std::string m_mtgID;
    std::string m_headUrl;
    std::string m_nickName;
    double m_latitude;
    double m_longitude;
    MTG_TYPE m_mtgType;
    
   
     TouchMoveView *m_touchMoveView;
    NSMutableArray *m_peertouchMoveViews;
    
    unsigned long  m_audioID;
    unsigned long  m_uUserID;
    bool m_openCamera;
     bool m_beautifyFace;
    
    OpenLocalUser * m_OpenLocalUser;
    std::map<unsigned long, class OpenRemoteUser*> m_pOpenRemoteUser_map;
     CConnectServer             *m_pConnectServer;
    IVideoChatCallback*          m_pINetWorkCallback;
    CLIENTUSERINFOLIST_MAP     m_UserInfoList;
      AudioUnitAecMix *m_audioUnitAecMix;
    std::recursive_mutex    m_mutex;
};
#endif
