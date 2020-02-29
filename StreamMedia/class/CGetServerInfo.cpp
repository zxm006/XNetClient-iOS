
#include "CGetServerInfo.h"
#import <Foundation/Foundation.h>

#include "ConnectServer.h"
#include "AutoLock.h"

#import <dispatch/queue.h>

CGetServerInfo::CGetServerInfo()
:m_strAddress(""),
m_serverip(""),
m_userGroupId(""),
m_pConnectServer(NULL)
{
    NETEC_Setting::SetAudioProtocolType(NETEC_Setting::PT_UDP);
    NETEC_Setting::SetMCUOnly(1);
    m_pINetWorkCallback=NULL;
   }

CGetServerInfo::~CGetServerInfo(void)
{
    
}


int CGetServerInfo::ConnectServer(const char* szIP,unsigned int nPort,bool isEncrypt)
{
    if (m_pConnectServer) {
        delete m_pConnectServer;
        m_pConnectServer = NULL;
    }
 
    
    m_serverip = szIP;
    
    m_pConnectServer = new CConnectServer( );
    
    return m_pConnectServer->Start(szIP,nPort,isEncrypt,this);
}

void CGetServerInfo::disconnect()
{
    if (m_pConnectServer)
     {
        m_pConnectServer->Stop();
       
       delete m_pConnectServer;
         
       m_pConnectServer = NULL;
     }
 
    
}


void CGetServerInfo::setNetWorkCallback(IServerInfoCallback*netWorkCallback)
{
    m_pINetWorkCallback   =netWorkCallback;
}



void CGetServerInfo::OnDispatchCmd(KCmdPacketEx& pPacket)
{
    std::string strCMD = pPacket.GetCMD();
    DLog(@"strCMD ==>%s",strCMD.c_str());
    
    if(strCMD=="LOGINSERVERED")
    {
      m_pINetWorkCallback->IConnectStatusCallback(CS_LOGINFAILED);
 
    }
    
   
   
    
}
void CGetServerInfo::On_SessionConnectStatus(LINKMIC_STATUS cs)
{
    
    if(cs == CS_LOGINED||cs == CS_RECONNECTED)
    {
   
      
        if(m_pINetWorkCallback)
            m_pINetWorkCallback->IConnectStatusCallback(CS_LOGINED);
    }
    else if(cs == CS_DISCONNECTED||cs == CS_LOGINFAILED)
    {
   
    }
    else if(cs == CS_CONNECTED||cs == CS_RECONNECTED)
    {
//        this->sendloginServer( m_username.c_str() ,"" );
    }
    
    if(m_pINetWorkCallback)
    {
        m_pINetWorkCallback->IConnectStatusCallback(cs);
    }
}

//int CGetServerInfo::sendloginServer(const char* szName, const char* szPassword)
//{
//    if(!szName || !szPassword)
//        return -1;
//
//
//
//    m_username = szName;
//    srand((unsigned)time(NULL));
//    unsigned int time=[[NSDate date] timeIntervalSince1970]+rand()%1000000;
//
//    std::string strLogin = "LoginServer";
//    KCmdPacketEx rPacket(strLogin.c_str(),(int)strLogin.length()+1);
//    std::string strCMD = "LOGINSERVER";
//    rPacket.SetCMD(strCMD);
//    std::string strname = "USERNAME";
//    rPacket.SetAttrib(strname,szName);
//    std::string strPass = "USERPASS";
//    rPacket.SetAttrib(strPass,szPassword);
//    std::string strAddress = "ADDRESS";
//    rPacket.SetAttrib(strAddress,m_strAddress);
//    std::string straudioID = "USERAUDIOID";
//    std::string groupid = "GROUPID";
//    rPacket.SetAttrib(groupid, m_userGroupId);
//    std::string expand = "EXPAND";
//    rPacket.SetAttrib(expand, m_szexpand);
//    return m_pConnectServer->SendData(rPacket);
//}

int CGetServerInfo::logoutServer()
{
    if(!m_pConnectServer)
        return -1;
    std::string strinfo = "quitserver";
    KCmdPacketEx rPacket(strinfo.c_str(),(int)strinfo.length()+1);
    std::string strCMD = "QUITSERVER";
    rPacket.SetCMD(strCMD);
 
 
   return m_pConnectServer->SendData(rPacket);
}


void CGetServerInfo::LoginServer(const char* szIP,const char* szName ,const char* szGameId,const char* szGameServerId,const char* szRoomId,const char* szGroupId ,const char* szexpand,bool isencrypt )
{
    if(!szIP||!szName||!szGameId||!szGameServerId||!szRoomId)
    {
        return;
    }
    
    m_szexpand = szexpand;
    char sAddress[255];
    sprintf(sAddress,"%s-%s-%s",szGameId,szGameServerId,szRoomId);
    m_strAddress = sAddress;
    m_username = szName;
  
    if(!szGroupId)
    {
        m_userGroupId = "";
    }
    else
    {
        m_userGroupId = szGroupId;
    }
    ConnectServer(szIP,REMOTE_MCUPort,isencrypt);
}



