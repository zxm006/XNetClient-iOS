#ifndef _UU_CAPP_MANAGE_H_
#define _UU_CAPP_MANAGE_H_

#include "ConnectServer.h"
#include "uuDefine.h"
#include "OpenLocalUser.h"
#include <list>
#include <vector>
#import <UIKit/UIKit.h>
#import "TouchMoveView.h"
#include <pthread/pthread.h>
#include <mutex>

#define SIZE_AUDIO_FRAME (2)

typedef int (*INTERCOMCALLBACK)(int msgcode,bool issucceed,const char * funtype,const char *retMessage);


typedef struct _UserInfo{
    _UserInfo()
    :uUserID(0)
    ,strUserName(""){
    }
    ~_UserInfo(){}
    unsigned long uUserID;
    std::string   strUserName;
}UserInfo;

class INetWorkCallback
{
public:
    
    virtual void IConnectStatusCallback(CONNECT_NET_STATUS cs) = 0;
    virtual void INetReceiveUserList(CLIENTUSERINFOLIST_MAP& UserInfoList) = 0;
    virtual void INetReceiveUserLogin(CLIENTUSERINFOLIST & userinfo) = 0;
    virtual void INetReceiveUserLogOut(CLIENTUSERINFOLIST & userinfo) = 0;
    virtual void INetReceiveData(unsigned long uPeerUserID, std::string strName,std::string strData, unsigned long nLen) = 0;
    virtual void INetBroadcastData(unsigned long uPeerUserID, const char* pData, unsigned long nLen) = 0;
    virtual void INetAudioStatus(unsigned long uPeerUserID,bool isRoom,AUDIO_SEND_STATUS AudioStatus, std::string strName) = 0;
    virtual void ILoginTime(unsigned long loginTime) =0;
  

     
};

class CIntercomManage
:public CmdTCPClientCallback,ISendCmdServer
{
public:
    CIntercomManage(INTERCOMCALLBACK  Callback);
    ~CIntercomManage(void);
public:
    void  loginIntercomServer(const char* szIP,const char* szName , const char* headUrl,
                              const char* nickName,double latitude,double longitude ,const char* szGameId,const char* szGameServerId,const char* szRoomId,const char* szGroupId="",BOOL listenInRoom = true,const char* szexpand= "",bool isencrypt = false);
    
    
    void setNetWorkCallback(INetWorkCallback*netWorkCallback);
     int getUserList();
     int logoutServer();
    void disconnect();
    void SendAudioData(char*pData, int nLen);
    
    AUDIO_SEND_STATUS  getAudioStatus();
    
    void startSendAudio(BOOL isspeakinroom);
    void stopSendAudio();

    virtual int SendData(KCmdPacketEx& pPacket);
   
     int SendDataToUser(unsigned long uPeerUserID ,const char* pData, unsigned long nLen);
    
private:
    
    virtual void On_SessionConnectStatus(CONNECT_NET_STATUS cs);
    virtual  int SendAllUserData(const char* pData, unsigned long nLen);
    
    void SetRoomList(const char *roomUserList);
     int SendVoice(const char* pData, unsigned long nLen,bool isinroom);
 
    
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
    std::string m_RoomSpeakerName;
    std::string m_GroupSpeakerName;
    std::string m_strAddress;
    std::string m_userGroupId;
    
    unsigned long  m_audioID;
    unsigned long  m_uUserID; 
    std::string m_headUrl;
    std::string m_nickName;
    double m_latitude;
    double m_longitude;
    //是否在房间内说话，为内部控制
    BOOL m_isSpeakinRoom;
   
    //是否收听房间内的人说话，为外部设置
    BOOL mlistenInRoom ;
    
    AUDIO_SEND_STATUS  m_ROOMaudioStatus;
    AUDIO_SEND_STATUS  m_GROUPaudioStatus;
    
    OpenLocalUser * m_OpenLocalUser;
    std::map<unsigned long, class OpenRemoteUser*> m_pOpenRemoteUser_map;
    INTERCOMCALLBACK m_callback;
    CConnectServer             m_ConnectServer;
    
    INetWorkCallback*          m_pINetWorkCallback;
    CLIENTUSERINFOLIST_MAP     m_UserInfoList;
    
    std::recursive_mutex    m_mutex;
    void  setupSession();
    
    void openMediaSender();
    void closeMediaSender();
    
    void openLocalAudio();
    void closeLocalAudio();
  
    void StopAllRemoteMedia();
 
}; 
#endif
