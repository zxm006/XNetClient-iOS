//#include "StdAfx.h"
#include "CIntercomManage.h"
#import <Foundation/Foundation.h>
#import "AudioUnitTool.h"
#import "AudioUnitAecTool.h"
#include "OpenRemoteUser.h"
#include "ConnectServer.h"
#include "AutoLock.h"
CIntercomManage::CIntercomManage(INTERCOMCALLBACK callback)
:m_uUserID(0),
m_strAddress(""),
m_serverip(""),
m_OpenLocalUser(NULL),
m_userGroupId(""),
m_headUrl(""),
m_nickName(""),
m_latitude(0),
m_longitude(0)
{
    XNetSetting::SetAudioProtocolType(XNetSetting::PT_UDP);
    XNetSetting::SetMCUOnly(1);
    m_pINetWorkCallback=NULL;
    m_callback = callback;
    m_UserInfoList.clear();
    m_ROOMaudioStatus = AUDIO_SEND_ENABLE;
    m_GROUPaudioStatus = AUDIO_SEND_ENABLE;
    
    //   setupSession();
}

CIntercomManage::~CIntercomManage(void)
{
    
}

int CIntercomManage::SendData(KCmdPacketEx& pPacket)
{
    return 0;
}

AUDIO_SEND_STATUS CIntercomManage::getAudioStatus()
{
    return m_ROOMaudioStatus;
}

void CIntercomManage::startSendAudio(BOOL isspeakinroom)
{
    if (isspeakinroom)
    {
        if(!mlistenInRoom)
            return;
        if(m_ROOMaudioStatus==AUDIO_SEND_ENABLE&&m_GROUPaudioStatus!=AUDIO_SEND_SENDING)
        {
            NSString*  smsg=[NSString stringWithFormat:@"AUDIO_SEND_DISABLE"];
            this->SendVoice([smsg UTF8String], [smsg length], isspeakinroom);
            m_isSpeakinRoom = isspeakinroom;
        }
        else
        {
            //IntercomLOG(@"startSendAudio AUDIO_SEND_DISABLE 状态错误!");
        }
    }
    else
    {
        if(m_GROUPaudioStatus==AUDIO_SEND_ENABLE&&m_ROOMaudioStatus!=AUDIO_SEND_SENDING)
        {
            
            NSString*  smsg=[NSString stringWithFormat:@"AUDIO_SEND_DISABLE"];
            this->SendVoice([smsg UTF8String], [smsg length], isspeakinroom);
            m_isSpeakinRoom = isspeakinroom;
        }
        else
        {
            //IntercomLOG(@"startSendAudio AUDIO_SEND_DISABLE 状态错误!");
        }
    }
}

void CIntercomManage::stopSendAudio()
{
    
    if (m_isSpeakinRoom)
    {
        NSString*  smsg=[NSString stringWithFormat:@"AUDIO_SEND_ENABLE"];
        this->SendVoice([smsg UTF8String], [smsg length], YES);
    }
    else
    {
        NSString*  smsg=[NSString stringWithFormat:@"AUDIO_SEND_ENABLE"];
        this->SendVoice([smsg UTF8String], [smsg length], NO);
    }
    
    if(m_ROOMaudioStatus==AUDIO_SEND_SENDING)
    {
        [[AudioUnitTool shareHandle]stop];
        m_ROOMaudioStatus = AUDIO_SEND_ENABLE;
    }
    else if(m_GROUPaudioStatus==AUDIO_SEND_SENDING)
    {
        [[AudioUnitTool shareHandle]stop];
        m_GROUPaudioStatus = AUDIO_SEND_ENABLE;
    }
    this->closeMediaSender();
}

int CIntercomManage::ConnectServer(const char* szIP,unsigned int nPort,bool isEncrypt)
{
    m_serverip = szIP;
    return m_ConnectServer.Start(szIP,nPort,isEncrypt,this);
}

void CIntercomManage::disconnect()
{
    m_ConnectServer.Stop();
}

void CIntercomManage::On_SessionConnectStatus(CONNECT_NET_STATUS cs)
{
    
    if(cs == CS_LOGINED)
    {
        
        m_callback(0,YES,"Multi//IntercomLOGin","OK");
        //IntercomLOG(@"登录成功");
        this->getUserList();
        if(m_pINetWorkCallback)
            m_pINetWorkCallback->IConnectStatusCallback(CS_LOGINED);
        
        
    }
    else if(cs == CS_DISCONNECTED||cs == CS_LOGINFAILED)
    {
        //IntercomLOG(@"登录失败");
        m_callback(-1,NO,"Multi//IntercomLOGin","FAILE");
        
        this->closeMediaSender();
        this->StopAllRemoteMedia();
    }
    else if(cs == CS_CONNECTED||cs == CS_RECONNECTED||cs == CS_RESTARTED)
    {
        this->LoginServer( m_username.c_str() ,"");
    }
    
    if(m_pINetWorkCallback)
    {
        m_pINetWorkCallback->IConnectStatusCallback(cs);
    }
}

void CIntercomManage::setNetWorkCallback(INetWorkCallback*netWorkCallback)
{
    m_pINetWorkCallback   =netWorkCallback;
}

void CIntercomManage::OnDispatchCmd(KCmdPacketEx& pPacket)
{
    std::string strCMD = pPacket.GetCMD();
    //IntercomLOG(@"strCMD ==>%s",strCMD.c_str());
    
    if(strCMD=="LOGINSERVERED")
    {
        m_uUserID = pPacket.GetAttrib("USERID").AsUnsignedLong();
        unsigned long ulErrorType = pPacket.GetAttrib("ERRORTYPE").AsUnsignedLong();
        unsigned long   ulLoginTime     = pPacket.GetAttrib("LOGINTIME").AsUnsignedLong();
        
        if(ulErrorType == 0)
        {
            NSString *str=@"";
            NSNotification *notification = [NSNotification notificationWithName:@"LOGININ"
                                                                         object:str];
            [[NSNotificationCenter defaultCenter] postNotification:notification];
            m_callback(0,YES,"Multi//IntercomLOGin","OK");
            //IntercomLOG(@"登录成功");
            
            this->getUserList();
            
            if(m_pINetWorkCallback)
                m_pINetWorkCallback->IConnectStatusCallback(CS_LOGINED);
            
            if(m_pINetWorkCallback)
                m_pINetWorkCallback->ILoginTime( ulLoginTime);
            
        }
        else
        {
            m_callback(-1,NO,"Multi//IntercomLOGin","FAILE");
            //IntercomLOG(@"登录失败");
            if(m_pINetWorkCallback)
                m_pINetWorkCallback->IConnectStatusCallback(CS_LOGINFAILED);
        }
    }
    
    else if(strCMD=="REMOTEUSERLOGIN")
    {
        std::string strUserName = pPacket.GetAttrib("USERNAME").AsString();
        unsigned long ulUserID = pPacket.GetAttrib("USERID").AsUnsignedLong();
        
        CLIENTUSERINFOLIST userinfo;
        userinfo.ulMtgTime     = pPacket.GetAttrib("MTGTIME").AsUnsignedLong();
        userinfo.strUserName   =  pPacket.GetAttrib("USERNAME").AsString();
        userinfo.ulUserId      =  pPacket.GetAttrib("USERID").AsUnsignedLong();
        userinfo.ulUserAudioID =  pPacket.GetAttrib("USERAUDIOID").AsUnsignedLong();
        userinfo.strHeadUrl    =  pPacket.GetAttrib("HEADURL").AsString();
        userinfo.strNickName   =  pPacket.GetAttrib("NICKNAME").AsString();
        userinfo.ulLatitude    =  pPacket.GetAttrib("LATITUDE").AsDouble();
        userinfo.ulLongitude   =  pPacket.GetAttrib("LONGITUDE").AsDouble();
        userinfo.ulMtgTime     = pPacket.GetAttrib("MTGTIME").AsUnsignedLong();
        userinfo.ulLoginTime     = pPacket.GetAttrib("LOGINTIME").AsUnsignedLong();
        m_UserInfoList[ulUserID] = userinfo;
        if(m_pINetWorkCallback)
        {
            m_pINetWorkCallback->INetReceiveUserLogin(userinfo);
        }
        
    }
    else if(strCMD=="REMOTEUSERQUIT")
    {
        KAutoLock lock(m_mKCritSec);
        
        unsigned long ulUserID = pPacket.GetAttrib("USERID").AsUnsignedLong();
        std::string strUserName = pPacket.GetAttrib("USERNAME").AsString();
        std::string strExpand = pPacket.GetAttrib("EXPAND").AsString();
        
        CLIENTUSERINFOLIST_MAP::iterator iter = m_UserInfoList.find(ulUserID);
        
        if(m_pINetWorkCallback)
        {
            m_pINetWorkCallback->INetReceiveUserLogOut(iter->second);
        }
        if(iter != m_UserInfoList.end())
        {
            m_UserInfoList.erase(iter);
        }
        
        
        if(strUserName.compare(m_RoomSpeakerName)==0)
        {
            this->m_ROOMaudioStatus = AUDIO_SEND_ENABLE;
            
            NSString *strusername = [NSString stringWithCString:strUserName.c_str() encoding:NSUTF8StringEncoding];
            NSString *strexpand = [NSString stringWithCString:strExpand.c_str()
                                                     encoding:NSUTF8StringEncoding];
            
            NSString * smsg=[NSString stringWithFormat:@"{\"status\":\"%s\",\"username\":\"%@\",\"expand\":\"%@\",\"inRoom\":\"%s\"}","1",strusername,strexpand,"true"];
            
            m_callback(0,YES,"MultiIntercomStatus",[smsg UTF8String]);
            m_RoomSpeakerName = "";
            NSString *str= [NSString stringWithFormat:@"说话者退出房间了 %s",strUserName.c_str()];
            //IntercomLOG(@"%@",str);
            
            if(m_pINetWorkCallback)
                m_pINetWorkCallback->INetAudioStatus(ulUserID,YES,AUDIO_SEND_ENABLE,strUserName);
        }
        
        if(strUserName.compare(m_GroupSpeakerName)==0)
        {
            this->m_GROUPaudioStatus = AUDIO_SEND_ENABLE;
            
            
            NSString *strusername = [NSString stringWithCString:strUserName.c_str() encoding:NSUTF8StringEncoding];
            NSString *strexpand = [NSString stringWithCString:strExpand.c_str()
                                                     encoding:NSUTF8StringEncoding];
            
            NSString * smsg=[NSString stringWithFormat:@"{\"status\":\"%s\",\"username\":\"%@\",\"expand\":\"%@\",\"inRoom\":\"%s\"}","1",strusername,strexpand,"false"];
            
            m_callback(0,YES,"MultiIntercomStatus",[smsg UTF8String]);
            
            m_GroupSpeakerName = "";
            NSString *str= [NSString stringWithFormat:@"说话者退出房间了 %s",strUserName.c_str()];
            //IntercomLOG(@"%@",str);
            
            if(m_pINetWorkCallback)
                m_pINetWorkCallback->INetAudioStatus(ulUserID,NO,AUDIO_SEND_ENABLE,strUserName);
        }
        
        
    }
    
    else if(strCMD=="RELOGIN")
    {
        KAutoLock lock(m_mKCritSec);
        closeMediaSender();
        StopAllRemoteMedia();
        
        if(m_pINetWorkCallback) {
            m_pINetWorkCallback->IConnectStatusCallback(CS_RELOGIN);
        }
    }
    
    else if(strCMD=="UPDATEUSERLIST")
    {
        KAutoLock lock(m_mKCritSec);
        CMD_ITEM_LST lstItems = pPacket.GetItemList();
        for(CMD_ITEM_LST::const_iterator it=lstItems.begin();it!=lstItems.end();it++)
        {
            KCmdItem item((std::string)*it);
            CLIENTUSERINFOLIST userinfo;
            userinfo.ulMtgTime     = item.GetAttrib("MTGTIME").AsUnsignedLong();
            userinfo.strUserName   =  item.GetAttrib("USERNAME").AsString();
            userinfo.ulUserId      =  item.GetAttrib("USERID").AsUnsignedLong();
            userinfo.ulUserAudioID =  item.GetAttrib("USERAUDIOID").AsUnsignedLong();
            userinfo.strHeadUrl    =  item.GetAttrib("HEADURL").AsString();
            userinfo.strNickName   =  item.GetAttrib("NICKNAME").AsString();
            userinfo.ulLatitude    =  item.GetAttrib("LATITUDE").AsDouble();
            userinfo.ulLongitude   =  item.GetAttrib("LONGITUDE").AsDouble();
            userinfo.ulMtgTime     = item.GetAttrib("MTGTIME").AsUnsignedLong();
            userinfo.ulLoginTime   = item.GetAttrib("LOGINTIME").AsUnsignedLong();
            
            m_UserInfoList[userinfo.ulUserId] = userinfo;
        }
        if(m_pINetWorkCallback)
            m_pINetWorkCallback->INetReceiveUserList(m_UserInfoList);
        
    }
    else if(strCMD=="SIGNALINGTRANSFER")
    {
        bool isbroadcast = pPacket.GetAttrib("BROADCAST").AsBoolean();
        std::string strData = pPacket.GetAttrib("DATA").AsString();
        std::string strUserName = pPacket.GetAttrib("USERNAME").AsString();
        printf("strData =%s\n\r",strData.c_str());
        if(isbroadcast)
        {
            unsigned long ulUserID = pPacket.GetAttrib("USERID").AsUnsignedLong();
            if(m_pINetWorkCallback)
                m_pINetWorkCallback->INetBroadcastData(ulUserID, strData.c_str(), strData.size());
        }
        else
        {
            unsigned long ulUserID = pPacket.GetAttrib("PEERUSERID").AsUnsignedLong();
            if(m_pINetWorkCallback)
                m_pINetWorkCallback->INetReceiveData(ulUserID,strUserName, strData.c_str(), strData.size());
        }
    }
    
    else if(strCMD=="USERQUIT")
    {
        KAutoLock lock(m_mKCritSec);
        if(m_pINetWorkCallback)
            m_pINetWorkCallback->IConnectStatusCallback(CS_LOGOUT);
    }
    
    else if(strCMD=="SENDVOICE")
    {
        KAutoLock lock(m_mKCritSec);
        std::string strUserName = pPacket.GetAttrib("USERNAME").AsString();
        std::string strGroupid = pPacket.GetAttrib("GROUPID").AsString();
        bool isinroom = pPacket.GetAttrib("ISINROOM").AsBoolean();
        std::string strAddress = pPacket.GetAttrib("ADDRESS").AsString();
        std::string strData = pPacket.GetAttrib("DATA").AsString();
        std::string strExpand = pPacket.GetAttrib("EXPAND").AsString();
        
        printf("SendVoice cmd = %s\n",strData.c_str());
        NSString *result  =[[[ NSString alloc]  initWithUTF8String: strUserName.c_str()]autorelease];
        //IntercomLOG(@"房间内 AUDIO_SEND_ENABLE speark result = %@",result);
        //IntercomLOG(@"房间内 AUDIO_SEND_ENABLE speark %s",strUserName.c_str());
        
        if(isinroom)
        {
            if (m_strAddress.compare(strAddress) !=0)
            {
                return;
            }
        }
        else
        {
            if (m_strAddress.compare(strAddress) !=0||m_userGroupId.compare(strGroupid)!=0 )
            {
                return;
            }
        }
        unsigned long ulUserID = pPacket.GetAttrib("PEERUSERID").AsUnsignedLong();
        
        if (strData.compare("AUDIO_SEND_ENABLE") == 0)
        {
            if(strUserName.compare(m_username)==0)
            {
                if(isinroom)
                {
                    m_ROOMaudioStatus = AUDIO_SEND_SENDING;
                }
                else
                {
                    m_GROUPaudioStatus = AUDIO_SEND_SENDING;
                }
                this->openMediaSender();
                //IntercomLOG(@"openMediaSender 成功");
                return;
            }
            
            if(isinroom)
            {
                this->m_ROOMaudioStatus = AUDIO_SEND_ENABLE;
                m_RoomSpeakerName="";
                NSString *strusername = [NSString stringWithCString:strUserName.c_str() encoding:NSUTF8StringEncoding];
                NSString *strexpand = [NSString stringWithCString:strExpand.c_str()
                                                         encoding:NSUTF8StringEncoding];
                
                NSString * smsg=[NSString stringWithFormat:@"{\"status\":\"%s\",\"username\":\"%@\",\"expand\":\"%@\",\"inRoom\":\"%s\"}","1",strusername,strexpand,"true"];
                
                m_callback(0,YES,"MultiIntercomStatus",[smsg UTF8String]);
                
            }
            else
            {
                this->m_GROUPaudioStatus = AUDIO_SEND_ENABLE;
                m_GroupSpeakerName="";
                NSString *result  =[[[ NSString alloc]  initWithUTF8String: strUserName.c_str()]autorelease];
                
                //IntercomLOG(@"房间内 AUDIO_SEND_ENABLE speark %@",result);
                //                //IntercomLOG(@"房间内 AUDIO_SEND_ENABLE speark %s",strUserName.c_str());
                NSString *strusername = [NSString stringWithCString:strUserName.c_str() encoding:NSUTF8StringEncoding];
                NSString *strexpand = [NSString stringWithCString:strExpand.c_str()
                                                         encoding:NSUTF8StringEncoding];
                
                NSString * smsg=[NSString stringWithFormat:@"{\"status\":\"%s\",\"username\":\"%@\",\"expand\":\"%@\",\"inRoom\":\"%s\"}","1",strusername,strexpand,"false"];
                m_callback(0,YES,"MultiIntercomStatus",[smsg UTF8String]);
            }
            
            if(m_ROOMaudioStatus !=AUDIO_SEND_DISABLE &&m_GROUPaudioStatus !=AUDIO_SEND_DISABLE&&m_pOpenRemoteUser_map.size()>1)
            {
                StopAllRemoteMedia();
            }
            else
            {
                unsigned long ulUserID = pPacket.GetAttrib("USERID").AsUnsignedLong();
                std::map<unsigned long, class OpenRemoteUser*>::iterator it=m_pOpenRemoteUser_map.find(ulUserID);
                
                if (it!=m_pOpenRemoteUser_map.end())
                {
                    dispatch_sync(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
                        
                        OpenRemoteUser *openRemoteUser=(*it).second;
                        openRemoteUser->CloseAudioReceive();
                        openRemoteUser->ReleaseMediaSever();
                        delete openRemoteUser;
                        openRemoteUser = NULL;
                        
                    });
                    m_pOpenRemoteUser_map.erase(it);
                }
            }
            
            if(m_pINetWorkCallback)
                m_pINetWorkCallback->INetAudioStatus(ulUserID,isinroom,AUDIO_SEND_ENABLE,strUserName);
            return;
        }
        else if(strData.compare("AUDIO_SEND_DISABLE") == 0)
        {
            if(strUserName.compare(m_username)==0)
            {
                //IntercomLOG(@"自己 ..... AUDIO_SEND_DISABLE speark %s",strUserName.c_str());
                
                return;
            }
            if(isinroom)
            {
                if(!mlistenInRoom)
                    return;
                
                //IntercomLOG(@"房间内 AUDIO_SEND_DISABLE speark %s",strUserName.c_str());
                m_RoomSpeakerName=strUserName;
                NSString *strusername = [NSString stringWithCString:strUserName.c_str() encoding:NSUTF8StringEncoding];
                NSString *strexpand = [NSString stringWithCString:strExpand.c_str()
                                                         encoding:NSUTF8StringEncoding];
                
                NSString * smsg=[NSString stringWithFormat:@"{\"status\":\"%s\",\"username\":\"%@\",\"expand\":\"%@\",\"inRoom\":\"%s\"}","0",strusername,strexpand,"true"];
                m_callback(0,YES,"MultiIntercomStatus",[smsg UTF8String]);
                this->m_ROOMaudioStatus = AUDIO_SEND_DISABLE;
            }
            else
            {
                m_GroupSpeakerName=strUserName;
                //IntercomLOG(@"组内 AUDIO_SEND_DISABLE speark %s",strUserName.c_str());
                
                NSString *strusername = [NSString stringWithCString:strUserName.c_str() encoding:NSUTF8StringEncoding];
                NSString *strexpand = [NSString stringWithCString:strExpand.c_str()
                                                         encoding:NSUTF8StringEncoding];
                
                NSString * smsg=[NSString stringWithFormat:@"{\"status\":\"%s\",\"username\":\"%@\",\"expand\":\"%@\",\"inRoom\":\"%s\"}","0",strusername,strexpand,"false"];
                m_callback(0,YES,"MultiIntercomStatus",[smsg UTF8String]);
                
                this->m_GROUPaudioStatus = AUDIO_SEND_DISABLE;
                
            }
            std::map<unsigned long, class OpenRemoteUser*>::iterator it=m_pOpenRemoteUser_map.find(ulUserID);
            
            if (it!=m_pOpenRemoteUser_map.end())
            {
                dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
                    OpenRemoteUser *openRemoteUser=(*it).second;
                    openRemoteUser->CloseAudioReceive();
                    usleep(500);
                    openRemoteUser->ReleaseMediaSever();
                    delete openRemoteUser;
                    openRemoteUser = NULL;
                });
                
                m_pOpenRemoteUser_map.erase(it);
            }
            
            OpenRemoteUser* m_OpenRemoteUser = new OpenRemoteUser(false);
            m_OpenRemoteUser->ConnectMediaServer(m_serverip, 5566);
            
            unsigned int ulUseraudioID = pPacket.GetAttrib("USERAUDIOID").AsUnsignedInt();
            m_OpenRemoteUser->OpenAudioReceive(ulUseraudioID);
            unsigned long ulUserID = pPacket.GetAttrib("USERID").AsUnsignedLong();
            m_pOpenRemoteUser_map[ulUserID] = m_OpenRemoteUser;
            
            if(m_pINetWorkCallback)
                m_pINetWorkCallback->INetAudioStatus(ulUserID,isinroom,AUDIO_SEND_DISABLE,strUserName);
            return;
        }
        
        if(m_pINetWorkCallback)
            m_pINetWorkCallback->INetReceiveData(ulUserID,strUserName, strData.c_str(), strData.size());
    }
}


int CIntercomManage::LoginServer(const char* szName, const char* szPassword)
{
    if(!szName || !szPassword)
        return -1;
    
    m_username = szName;
    m_ROOMaudioStatus=AUDIO_SEND_ENABLE;
    m_GROUPaudioStatus=AUDIO_SEND_ENABLE;
    m_GroupSpeakerName="";
    m_RoomSpeakerName = "";
    
    
    srand((unsigned)time(NULL));
    unsigned int time=[[NSDate date] timeIntervalSince1970]+rand()%1000000;
    this->m_audioID=time-1400000000 ;
    
    std::string strLogin = "LoginServer";
    KCmdPacketEx rPacket(strLogin.c_str(),(int)strLogin.length()+1);
    std::string strCMD = "LOGINSERVER";
    rPacket.SetCMD(strCMD);
    std::string strname = "USERNAME";
    rPacket.SetAttrib(strname,szName);
    std::string strPass = "USERPASS";
    rPacket.SetAttrib(strPass,szPassword);
    std::string strAddress = "ADDRESS";
    rPacket.SetAttrib(strAddress,m_strAddress);
    std::string straudioID = "USERAUDIOID";
    rPacket.SetAttribUL(straudioID, this->m_audioID);
    
    std::string groupid = "GROUPID";
    rPacket.SetAttrib(groupid, m_userGroupId);
    std::string expand = "EXPAND";
    rPacket.SetAttrib(expand, m_szexpand);
    std::string headUrl = "HEADURL";
    rPacket.SetAttrib(headUrl, m_headUrl);
    std::string nickName = "NICKNAME";
    rPacket.SetAttrib(nickName, m_nickName);
    std::string latitude = "LATITUDE";
    rPacket.SetAttribDBL(latitude, m_latitude);
    std::string longitude = "LONGITUDE";
    rPacket.SetAttribDBL(longitude, m_longitude);
    return m_ConnectServer.SendData(rPacket);
    
}
void CIntercomManage::setupSession()
{
    NSError *error = nil;
    AVAudioSession *sessionInstance = [AVAudioSession sharedInstance];
    [sessionInstance setActive:YES error:&error];
    [sessionInstance setCategory:AVAudioSessionCategoryPlayAndRecord withOptions:AVAudioSessionCategoryOptionDefaultToSpeaker error:&error];
    
    AVAudioSession* session = [AVAudioSession sharedInstance];
    Float64 preferredSampleRate(16000.0);
    [session setPreferredSampleRate:preferredSampleRate error:&error];
    [session setMode:AVAudioSessionModeVideoChat error:&error];
    
    NSTimeInterval bufferDuration = .02;
    [sessionInstance setPreferredIOBufferDuration:bufferDuration error:&error];
    
}


int CIntercomManage::logoutServer()
{
    stopSendAudio();
    std::string strinfo = "quitserver";
    KCmdPacketEx rPacket(strinfo.c_str(),(int)strinfo.length()+1);
    std::string strCMD = "QUITSERVER";
    rPacket.SetCMD(strCMD);
    
    closeMediaSender();
    StopAllRemoteMedia();
    m_UserInfoList.clear();
    
    
    return m_ConnectServer.SendData(rPacket);
}

int CIntercomManage::getUserList()
{
    std::string strGetuserlist= "getuserlist";
    KCmdPacketEx rPacket(strGetuserlist.c_str(),(int)strGetuserlist.length()+1);
    rPacket.SetAttrib("ADDRESS", m_strAddress);
    std::string strCMD = "GETUSERLIST";
    rPacket.SetCMD(strCMD);
    return m_ConnectServer.SendData(rPacket);
}

void CIntercomManage::loginIntercomServer(const char* szIP,const char* szName , const char* headUrl, const char* nickName,double latitude,double longitude ,const char* szGameId,const char* szGameServerId,const char* szRoomId,const char* szGroupId,BOOL listenInRoom,const char* szexpand,bool isencrypt)
{
    if(!szIP||!szName||!szGameId||!szGameServerId||!szRoomId)
    {
        m_callback(-1,NO,"Multi//IntercomLOGin","FAILE");
        return;
    }
    
    m_szexpand = szexpand;
    mlistenInRoom = listenInRoom;
    char sAddress[255];
    sprintf(sAddress,"%s-%s-%s",szGameId,szGameServerId,szRoomId);
    m_strAddress = sAddress;
    m_username = szName;
    m_headUrl = headUrl;
    m_nickName = nickName;
    m_latitude =  latitude;
    m_longitude = longitude;
    
    if(!szGroupId)
    {
        m_userGroupId = "";
    }
    else
    {
        m_userGroupId = szGroupId;
    }
    ConnectServer(szIP,5566,isencrypt);
}

int CIntercomManage::SendVoice(const char* pData, unsigned long nLen,bool isinroom )
{
    if(!pData || nLen == 0)
        return -1;
    //不在组里却发组的消息。
    if (!isinroom&&m_userGroupId.compare("")==0)
    {
        return -1;
    }
    
    std::string strinfo = "SENDVOICE";
    KCmdPacketEx rPacket(strinfo.c_str(),(int)strinfo.length()+1);
    std::string strCMD = "SENDVOICE";
    rPacket.SetCMD(strCMD);
    
    rPacket.SetAttribBL("ISINROOM", isinroom);
    
    rPacket.SetAttrib("DATA", pData);
    
    rPacket.SetAttribUL("USERID", m_uUserID);
    rPacket.SetAttrib("USERNAME", m_username);
    rPacket.SetAttrib("ADDRESS", m_strAddress);
    rPacket.SetAttrib("GROUPID", m_userGroupId);
    rPacket.SetAttribUL("USERAUDIOID", m_audioID);
    
    rPacket.SetAttrib( "EXPAND", m_szexpand);
    
    return m_ConnectServer.SendData(rPacket);
}

int CIntercomManage::SendDataToUser(unsigned long uPeerUserID ,const char* pData, unsigned long nLen)
{
    if(!pData || nLen == 0)
        return -1;
    std::string strinfo = "Signalingtransfer";
    KCmdPacketEx rPacket(strinfo.c_str(),(int)strinfo.length()+1);
    std::string strCMD = "SIGNALINGTRANSFER";
    rPacket.SetCMD(strCMD);
    rPacket.SetAttribBL("BROADCAST", false);
    rPacket.SetAttribUL("PEERUSERID", uPeerUserID);
    rPacket.SetAttrib("DATA", pData);
    rPacket.SetAttribUL("USERID", m_uUserID);
    rPacket.SetAttrib("USERNAME", m_username);
    return m_ConnectServer.SendData(rPacket);
}

int CIntercomManage::SendAllUserData(const char* pData, unsigned long nLen)
{
    if(!pData || nLen == 0)
        return -1;
    
    std::string strinfo = "Signalingtransfer";
    KCmdPacketEx rPacket(strinfo.c_str(),(int)strinfo.length()+1);
    std::string strCMD = "SIGNALINGTRANSFER";
    rPacket.SetCMD(strCMD);
    rPacket.SetAttribBL("BROADCAST", true);
    rPacket.SetAttribUL("USERID", m_uUserID);
    rPacket.SetAttribUL("USERAUDIOID", this->m_audioID);
    rPacket.SetAttrib("DATA", pData);
    return m_ConnectServer.SendData(rPacket);
}

std::string CIntercomManage::GetLocalIP()
{
    return m_ConnectServer.GetLocalIP();
}

std::string CIntercomManage::GetNATIP()
{
    return m_ConnectServer.GetNATIP();
}

void CIntercomManage::SendAudioData(char*pData, int nLen)
{
    if (m_OpenLocalUser&&( m_ROOMaudioStatus==AUDIO_SEND_SENDING||m_GROUPaudioStatus == AUDIO_SEND_SENDING))
    {
        m_OpenLocalUser->SendAudioData(pData, nLen);
    }
    else if(m_OpenLocalUser)
    {
        m_OpenLocalUser->SendAudioData(pData, nLen);
    }
}

void CIntercomManage::openMediaSender()
{
    KAutoLock lock(m_mKCritSec);
    if (!m_OpenLocalUser)
    {
        m_OpenLocalUser = new OpenLocalUser;
        m_OpenLocalUser->ConnectMediaServer(m_serverip, 5566);
        m_OpenLocalUser->OpenAudioSend((unsigned int)this->m_audioID);
        
        openLocalAudio();
    }
}

void CIntercomManage::closeMediaSender()
{
    KAutoLock lock(m_mKCritSec);
    if (m_OpenLocalUser != NULL) {
        closeLocalAudio();
        m_OpenLocalUser->CloseAudioSend();
        m_OpenLocalUser->ReleaseMediaSever();
        delete m_OpenLocalUser;
        m_OpenLocalUser = NULL;
    }
}

void CIntercomManage::closeLocalAudio()
{
    [[AudioUnitTool shareHandle]setSendCmdManage:NULL];
    [[AudioUnitTool shareHandle]stop];
    [AudioUnitTool closeHandle];
    
}

void CIntercomManage::openLocalAudio()
{
    [[AudioUnitTool shareHandle]setSendCmdManage:this];
    [[AudioUnitTool shareHandle]start];
}

void CIntercomManage::StopAllRemoteMedia()
{
    KAutoLock lock(m_mKCritSec);
    std::map<unsigned long, class OpenRemoteUser*>::iterator it=m_pOpenRemoteUser_map.begin();
    while (it!=m_pOpenRemoteUser_map.end()) {
        dispatch_sync(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
            OpenRemoteUser *openRemoteUser=(*it).second;
            openRemoteUser->CloseAudioReceive();
            usleep(500);
            openRemoteUser->ReleaseMediaSever();
            
            delete openRemoteUser;
            openRemoteUser = NULL;
        });
        m_pOpenRemoteUser_map.erase(it++);
    }
}


