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




class IServerInfoCallback
{
public:
    virtual void  UserVideoView(UIView* view   ,NSString*userid)=0;
    virtual void UserMediaStatus(std::string strName,bool isvideo,bool isopen) = 0;
    virtual void  MtgInfoReg(MTG_CMD mtgCmd,bool result,std::string &info) =0;
    virtual void  MtgInfo(MTG_CMD mtgCmd,std::string username ,std::string &info) =0;
    virtual void  MtgInvite(std::string username ,std::string mtgId) =0;
    
    virtual void IConnectStatusCallback(LINKMIC_STATUS cs) = 0;
    virtual void INetReceiveUserList(CLIENTUSERINFOLIST_MAP& UserInfoList) = 0;
    virtual void INetReceiveUserLogin(unsigned long uPeerUserID, std::string strName,unsigned long seraudioID) = 0;
    virtual void INetReceiveUserLogOut(unsigned long uPeerUserID) = 0;
    virtual void INetReceiveData(unsigned long uPeerUserID, std::string strName,std::string strData, unsigned long nLen) = 0;
    virtual void INetBroadcastData(unsigned long uPeerUserID, const char* pData, unsigned long nLen) = 0;
 
};


class CGetServerInfo
:public CmdTCPClientCallback,ISendCmdServer
{
public:
    CGetServerInfo();
    ~CGetServerInfo(void);
public:
    void LoginServer(const char* szIP,const char* szName ,const char* szGameId,const char* szGameServerId,const char* szRoomId,const char* szGroupId="",const char* szexpand= "",bool isencrypt = true);
    int logoutServer();
    
    void setNetWorkCallback(IServerInfoCallback*netWorkCallback);

 
    virtual void On_SessionConnectStatus(LINKMIC_STATUS cs);
   void disconnect();
    void OnDispatchCmd(KCmdPacketEx& pPacket);
  
    int ConnectServer(const char* szIP,unsigned int nPort ,bool isEncrypt);
 
private:
    
    virtual std::string GetLocalIP();
    virtual std::string GetNATIP();
    std::string m_serverip;
    std::string m_username;
    std::string m_szexpand;
    std::string m_strAddress;
    std::string m_userGroupId;
 
     CConnectServer             *m_pConnectServer;
    IServerInfoCallback*          m_pINetWorkCallback;
 
    KCritSec m_mKCritSec;
 
};
#endif
