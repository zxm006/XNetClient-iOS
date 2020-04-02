
#include "CVideoChat.h"
#import <Foundation/Foundation.h>
#import "AudioUnitTool.h"
#include "OpenRemoteUser.h"
#include "ConnectServer.h"
#import "TouchMoveView.h"
#import <dispatch/queue.h>
#import "MicBlow.h"
#import "CameraHelp.h"
#import"AudioUnitManager.h"

CVideoChat::CVideoChat()
:m_uUserID(0),
m_strAddress(""),
m_serverip(""),
m_OpenLocalUser(NULL),
m_userGroupId(""),
m_mtgID (""),
m_pConnectServer(NULL),
m_openCamera(NO),
m_beautifyFace(true),
m_headUrl(""),
m_nickName(""),
m_latitude(0),
m_longitude(0),
m_mtgType(MTG_Common),
m_pINetWorkCallback(NULL)
{
    XNetSetting::SetAudioProtocolType(XNetSetting::PT_TCP);
    XNetSetting::SetVideoProtocolType(XNetSetting::PT_TCP);
    XNetSetting::SetMCUOnly(1);
    m_pINetWorkCallback=NULL;
    m_UserInfoList.clear();
    m_touchMoveView = nil;
    m_peertouchMoveViews =[[NSMutableArray alloc]init];
}

CVideoChat::~CVideoChat(void)
{
    if(m_peertouchMoveViews) {
        [m_peertouchMoveViews release];
        m_peertouchMoveViews =nil;
    }
    
}

void CVideoChat::setAudioSession()
{
    AVAudioSession* session = [AVAudioSession sharedInstance];
    NSError *error = nil;
    
    [session setCategory:AVAudioSessionCategoryPlayAndRecord withOptions:AVAudioSessionCategoryOptionDefaultToSpeaker error:&error];
    //
    //
    //    Float64 preferredSampleRate(16000.0);
    //    [session setPreferredSampleRate:preferredSampleRate error:&error];
    //
    //    [session setMode:AVAudioSessionModeVoiceChat error:&error];
    //    NSTimeInterval bufferDuration = .02;
    //    [session setPreferredIOBufferDuration:bufferDuration error:&error];
    //
    [session setActive:YES error:&error];
}

int CVideoChat::SendData(KCmdPacketEx& pPacket)
{
    return 0;
}

int CVideoChat::ConnectServer(const char* szIP,unsigned int nPort,bool isEncrypt)
{
    if (m_pConnectServer) {
        m_pConnectServer->Stop();
        delete m_pConnectServer;
        printf("========m_pConnectServer != NULL \n\r ===========================");
        m_pConnectServer = NULL;
    }
    m_serverip = szIP;
    m_pConnectServer = new CConnectServer( );
    return m_pConnectServer->Start(szIP,nPort,isEncrypt,this);
}

void CVideoChat::disconnect()
{
    if (m_pConnectServer) {
        m_pConnectServer->Stop();
        delete m_pConnectServer;
        m_pConnectServer = NULL;
    }
}

void CVideoChat::On_SessionConnectStatus(CONNECT_NET_STATUS cs)
{
    if(cs == CS_LOGINED ) {
        DLog(@"登录成功");
        this->getUserList();
        if(m_pINetWorkCallback)
            m_pINetWorkCallback->IConnectStatusCallback(CS_LOGINED);
        
    } else if(cs == CS_DISCONNECTED) {
         std::lock_guard<std::recursive_mutex> lock_guard(m_mutex);
        closeMediaSender();
        StopAllRemoteMedia();
        
    } else if(cs == CS_CONNECTED||cs == CS_RECONNECTED||cs == CS_RESTARTED) {
        this->sendloginServer( m_username.c_str() ,"" );
    }
    
    if(m_pINetWorkCallback) {
        m_pINetWorkCallback->IConnectStatusCallback(cs);
    }
}

void CVideoChat::setNetWorkCallback(IVideoChatCallback*netWorkCallback)
{
    m_pINetWorkCallback = netWorkCallback;
}

void CVideoChat::MtgInfoReg(MTG_CMD mtgCmd,bool result,std::string &info)
{
    if (m_pINetWorkCallback) {
        m_pINetWorkCallback->MtgInfoReg(mtgCmd, result, info);
    }
}

void CVideoChat::MtgInfo(MTG_CMD mtgCmd,std::string username ,std::string &info)
{
    if (m_pINetWorkCallback) {
        m_pINetWorkCallback->MtgInfo(mtgCmd, username, info);
    }
}

void CVideoChat::OnDispatchMtgCmdReg(KCmdPacketEx& pPacket)
{
    MTG_CMD mtgCmd   = (MTG_CMD)pPacket.GetAttrib("MTGCMDREQ").AsUnsignedLong();
    bool result = pPacket.GetAttrib("RESULT").AsBoolean();
    std::string strinfo = pPacket.GetAttrib("INFO").AsString();
    switch (mtgCmd) {
        case  MTG_JOIN: {
            if(result) {
                
                DLog(@"MTG_JOIN succee");
                CMD_ITEM_LST lstItems = pPacket.GetItemList();
                CLIENTUSERINFOLIST_MAP     userInfoList;
                for(CMD_ITEM_LST::const_iterator it=lstItems.begin();it!=lstItems.end();it++) {
                    KCmdItem item((std::string)*it);
                    CLIENTUSERINFOLIST userinfo;
                    userinfo.ulMtgTime    = item.GetAttrib("MTGTIME").AsUnsignedLong();
                    userinfo.strUserName    =  item.GetAttrib("USERNAME").AsString();
                    userinfo.ulUserId      =  item.GetAttrib("USERID").AsUnsignedLong();
                    userinfo.ulUserAudioID =  item.GetAttrib("USERAUDIOID").AsUnsignedLong();
                    userinfo.strHeadUrl    =  item.GetAttrib("HEADURL").AsString();
                    userinfo.strNickName   =  item.GetAttrib("NICKNAME").AsString();
                    userinfo.ulLatitude    =  item.GetAttrib("LATITUDE").AsDouble();
                    userinfo.ulLongitude   =  item.GetAttrib("LONGITUDE").AsDouble();
                    userInfoList[userinfo.ulUserId ] =userinfo;
                    OpenRemoteUser* m_OpenRemoteUser = new OpenRemoteUser(false);
                    m_OpenRemoteUser->ConnectMediaServer(m_serverip, REMOTE_MCUPort);
                    m_OpenRemoteUser->OpenAudioReceive(userinfo.ulUserAudioID);
                    dispatch_async(dispatch_get_main_queue(), ^{
                        
                        TouchMoveView  *touchview =(TouchMoveView*)creatVideoWindow(0,0,80,80);
                        touchview.userName = [NSString stringWithFormat:@"%s",userinfo.strUserName.c_str()];
                        if(m_pINetWorkCallback)
                            m_pINetWorkCallback->UserVideoView(touchview, touchview.userName);
                        m_OpenRemoteUser->SetVideoWindow(touchview);
                        [m_peertouchMoveViews addObject:touchview];
                    });
                    
                    m_OpenRemoteUser->OpenVideoReceive(userinfo.ulUserAudioID+200);
                    m_pOpenRemoteUser_map[userinfo.ulUserId] = m_OpenRemoteUser;
                }
                
                startAudioUnit();
                
                if(m_pINetWorkCallback)
                    m_pINetWorkCallback->MtgUserList(userInfoList);
                openLocalMedia(NO);
            } else {
                DLog(@"MTG_JOIN fail");
                closeMediaSender();
            }
        }
            break;
        case  MTG_EXIT: {
            closeMediaSender();
            StopAllRemoteMedia();
            m_mtgID ="";
        }
            break;
        case  MTG_DESTROY: {
            closeMediaSender();
            StopAllRemoteMedia();
            m_mtgID ="";
        }
            break;
        default:
            break;
    }
    MtgInfoReg(mtgCmd, result, strinfo);
}

void CVideoChat::OnDispatchMtgCmd( KCmdPacketEx& pPacket)
{
    MTG_CMD mtgCmd   = (MTG_CMD)pPacket.GetAttrib("MTGCMD").AsUnsignedLong();
    long userid = pPacket.GetAttrib("USERID").AsUnsignedLong();
    std::string username = pPacket.GetAttrib("USERNAME").AsString();
    switch (mtgCmd) {
        case  MTG_JOIN: {
            DLog(@"MTG_JOIN   %s",username.c_str());
            
            auto iter = m_UserInfoList.find(userid);
            if(iter != m_UserInfoList.end()) {
                __block CLIENTUSERINFOLIST userinfo=(*iter).second;
                userinfo.ulMtgTime  = pPacket.GetAttrib("MTGTIME").AsUnsignedLong();
                OpenRemoteUser* m_OpenRemoteUser = new OpenRemoteUser(false);
                m_OpenRemoteUser->ConnectMediaServer(m_serverip, REMOTE_MCUPort);
                m_OpenRemoteUser->OpenAudioReceive(userinfo.ulUserAudioID);
                
                dispatch_async(dispatch_get_main_queue(), ^{
                    TouchMoveView  *touchview =nil;
                    if (m_peertouchMoveViews) {
                        for(int i =0;i<m_peertouchMoveViews.count ;i++) {
                            TouchMoveView *moveview = m_peertouchMoveViews[i];
                            if([moveview.userName isEqualToString:[NSString stringWithFormat:@"%s",userinfo.strUserName.c_str()]]) {
                                touchview = moveview ;
                            }
                        }
                    }
                    if (!touchview) {
                        touchview =(TouchMoveView*)creatVideoWindow(0,100,80,80);
                    }
                    
                    touchview.userName = [NSString stringWithFormat:@"%s",userinfo.strUserName.c_str()];
                    if(m_pINetWorkCallback) {
                        m_pINetWorkCallback->MtgJoin(userinfo);
                        m_pINetWorkCallback->UserVideoView(touchview, touchview.userName);
                    }
                    m_OpenRemoteUser->SetVideoWindow(touchview);
                    [m_peertouchMoveViews addObject:touchview];
                    
                });
                
                m_OpenRemoteUser->OpenVideoReceive(userinfo.ulUserAudioID+200);
                m_pOpenRemoteUser_map[userid] = m_OpenRemoteUser;
                startAudioUnit();
            }
        }
            break;
            
        case  MTG_EXIT: {
            DLog(@"MTG_EXIT   %s",username.c_str());
            
            std::map<unsigned long, class OpenRemoteUser*>::iterator it=m_pOpenRemoteUser_map.find(userid);
            if (it!=m_pOpenRemoteUser_map.end()) {
                dispatch_sync(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
                    OpenRemoteUser *openRemoteUser=(*it).second;
                    if(openRemoteUser) {
                        openRemoteUser->CloseAudioReceive();
                        openRemoteUser->CloseVideoReceive();
                        usleep(500);
                        openRemoteUser ->ReleaseMediaSever();
                        printf(" MTG_EXIT ReleaseMediaSever");
                        delete openRemoteUser;
                        openRemoteUser = NULL;
                        m_pOpenRemoteUser_map.erase(it);
                    }
                    
                    CLIENTUSERINFOLIST_MAP::iterator iter = m_UserInfoList.find(userid);
                    if(iter != m_UserInfoList.end()) {
                        CLIENTUSERINFOLIST userinfo=(*iter).second;
                        if(m_pINetWorkCallback) {
                            m_pINetWorkCallback->MtgExit(userinfo);
                        }
                        dispatch_async(dispatch_get_main_queue(), ^{
                            if (m_peertouchMoveViews) {
                                for(int i =0;i<m_peertouchMoveViews.count ;i++) {
                                    TouchMoveView *moveview = m_peertouchMoveViews[i];
                                    if([moveview.userName isEqualToString:[NSString stringWithFormat:@"%s",userinfo.strUserName.c_str()]]) {
                                        [m_peertouchMoveViews removeObject:moveview];
                                        [moveview removeFromSuperview];
                                        moveview =nil;
                                    }
                                }
                            }
                            startAudioUnit();
                        });
                    }
                });
            }
        }
            break;
            
        case  MTG_DESTROY:
        {
            closeMediaSender();
            StopAllRemoteMedia();
        }
            break;
        default:
            break;
    }
}

void CVideoChat::OnDispatchCmd(KCmdPacketEx& pPacket)
{
    std::string strCMD = pPacket.GetCMD();
    DLog(@"strCMD ==>%s",strCMD.c_str());
    if(strCMD=="LOGINSERVERED") {
        m_uUserID = pPacket.GetAttrib("USERID").AsUnsignedLong();
        unsigned long ulErrorType = pPacket.GetAttrib("ERRORTYPE").AsUnsignedLong();
        if(ulErrorType == 0) {
            this->getUserList();
            if(m_pINetWorkCallback)
                m_pINetWorkCallback->IConnectStatusCallback(CS_LOGINED);
        } else {
            DLog(@"登录失败");
            if(m_pINetWorkCallback)
                m_pINetWorkCallback->IConnectStatusCallback(CS_LOGINFAILED);
        }
    }
    
    else if(strCMD=="REMOTEUSERLOGIN")
    {
        std::string strUserName = pPacket.GetAttrib("USERNAME").AsString();
        unsigned long ulUserID = pPacket.GetAttrib("USERID").AsUnsignedLong();
        unsigned long ulUseraudioID = pPacket.GetAttrib("USERAUDIOID").AsUnsignedLong();
        
        CLIENTUSERINFOLIST usrinfo;
        usrinfo.strUserName   =  pPacket.GetAttrib("USERNAME").AsString();
        usrinfo.ulUserId      =  pPacket.GetAttrib("USERID").AsUnsignedLong();
        usrinfo.ulUserAudioID =  pPacket.GetAttrib("USERAUDIOID").AsUnsignedLong();
        usrinfo.strHeadUrl    =  pPacket.GetAttrib("HEADURL").AsString();
        usrinfo.strNickName   =  pPacket.GetAttrib("NICKNAME").AsString();
        usrinfo.ulLatitude    =  pPacket.GetAttrib("LATITUDE").AsDouble();
        usrinfo.ulLongitude   =  pPacket.GetAttrib("LONGITUDE").AsDouble();
        
        m_UserInfoList[ulUserID] = usrinfo;
        
        if(m_pINetWorkCallback)
        {
            m_pINetWorkCallback->INetReceiveUserLogin(ulUserID, strUserName,ulUseraudioID);
        }
    }
    else if(strCMD=="RELOGIN")
    {
         std::lock_guard<std::recursive_mutex> lock_guard(m_mutex);
        
        closeMediaSender();
        
        
        StopAllRemoteMedia();
        
        if(m_pINetWorkCallback) {
            m_pINetWorkCallback->IConnectStatusCallback(CS_RELOGIN);
        }
    }
    
    else if(strCMD=="REMOTEUSERQUIT")
    {
         std::lock_guard<std::recursive_mutex> lock_guard(m_mutex);
        unsigned long ulUserID = pPacket.GetAttrib("USERID").AsUnsignedLong();
        std::string strUserName = pPacket.GetAttrib("USERNAME").AsString();
        std::string strExpand = pPacket.GetAttrib("EXPAND").AsString();
        DLog(@"%s 退出房间",strUserName.c_str());
        CLIENTUSERINFOLIST_MAP::iterator iter = m_UserInfoList.find(ulUserID);
        if(m_pINetWorkCallback) {
            m_pINetWorkCallback->INetReceiveUserLogOut(ulUserID);
        }
        if(iter != m_UserInfoList.end()) {
            m_UserInfoList.erase(iter);
        }
        
        std::map<unsigned long, class OpenRemoteUser*>::iterator it=m_pOpenRemoteUser_map.find(ulUserID);
        if (it!=m_pOpenRemoteUser_map.end()) {
            dispatch_sync(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
                OpenRemoteUser *openRemoteUser=(*it).second;
                if(openRemoteUser) {
                    openRemoteUser->CloseAudioReceive();
                    openRemoteUser->CloseVideoReceive();
                    usleep(500);
                    openRemoteUser->ReleaseMediaSever();
                    delete openRemoteUser;
                    openRemoteUser = NULL;
                }
            });
            
            m_pOpenRemoteUser_map.erase(it++);
        }
        startAudioUnit();
    }
    
    else if(strCMD=="UPDATEUSERLIST") {
         std::lock_guard<std::recursive_mutex> lock_guard(m_mutex);
        CMD_ITEM_LST lstItems = pPacket.GetItemList();
        for(CMD_ITEM_LST::const_iterator it=lstItems.begin();it!=lstItems.end();it++) {
            KCmdItem item((std::string)*it);
            CLIENTUSERINFOLIST usrinfo;
            usrinfo.strUserName   =  item.GetAttrib("USERNAME").AsString();
            usrinfo.ulUserId      =  item.GetAttrib("USERID").AsUnsignedLong();
            usrinfo.ulUserAudioID =  item.GetAttrib("USERAUDIOID").AsUnsignedLong();
            usrinfo.strHeadUrl    =  item.GetAttrib("HEADURL").AsString();
            usrinfo.strNickName   =  item.GetAttrib("NICKNAME").AsString();
            usrinfo.ulLatitude    =  item.GetAttrib("LATITUDE").AsDouble();
            usrinfo.ulLongitude   =  item.GetAttrib("LONGITUDE").AsDouble();
            m_UserInfoList[usrinfo.ulUserId] = usrinfo;
        }
        if(m_pINetWorkCallback)
            m_pINetWorkCallback->INetReceiveUserList(m_UserInfoList);
    }
    else if(strCMD=="SIGNALINGTRANSFER") {
         std::lock_guard<std::recursive_mutex> lock_guard(m_mutex);
        bool isbroadcast = pPacket.GetAttrib("BROADCAST").AsBoolean();
        std::string strData = pPacket.GetAttrib("DATA").AsString();
        std::string strUserName = pPacket.GetAttrib("USERNAME").AsString();
        printf("strData =%s\n\r",strData.c_str());
        if(isbroadcast) {
            unsigned long ulUserID = pPacket.GetAttrib("USERID").AsUnsignedLong();
            if(m_pINetWorkCallback)
                m_pINetWorkCallback->INetBroadcastData(ulUserID, strData.c_str(), strData.size());
        } else {
            unsigned long ulUserID = pPacket.GetAttrib("PEERUSERID").AsUnsignedLong();
            if(m_pINetWorkCallback)
                m_pINetWorkCallback->INetReceiveData(ulUserID,strUserName, strData.c_str(), strData.size());
        }
    }
    else if(strCMD=="USERQUIT") {
        NSLog(@"退出房间" );
        
         std::lock_guard<std::recursive_mutex> lock_guard(m_mutex);
        closeMediaSender();
        StopAllRemoteMedia();
        
        if(m_pINetWorkCallback) {
            m_pINetWorkCallback->IConnectStatusCallback(CS_LOGOUT);
        }
    }
    
    else if(strCMD=="USERMEDIASTATUS") {
         std::lock_guard<std::recursive_mutex> lock_guard(m_mutex);
        bool isvideo = pPacket.GetAttrib("ISVIDEO").AsBoolean();
        bool isopen = pPacket.GetAttrib("ISOPEN").AsBoolean();
        std::string strUserName = pPacket.GetAttrib("USERNAME").AsString();
        
        if(m_pINetWorkCallback)
            m_pINetWorkCallback->UserMediaStatus(strUserName, isvideo, isopen);
    }
    else if(strCMD=="MTGCMDREQ") {
         std::lock_guard<std::recursive_mutex> lock_guard(m_mutex);
        OnDispatchMtgCmdReg(pPacket);
        
    } else if(strCMD=="MTGCMD") {
         std::lock_guard<std::recursive_mutex> lock_guard(m_mutex);
        OnDispatchMtgCmd(pPacket);
    } else if(strCMD=="INVITEMTGUSER") {
        std::string mtgid = pPacket.GetAttrib("MTGID").AsString();
        std::string username = pPacket.GetAttrib("USERNAME").AsString();
        if (m_pINetWorkCallback) {
            m_pINetWorkCallback->MtgInvite(username, mtgid);
        }
    }
}

int CVideoChat::sendloginServer(const char* szName, const char* szPassword)
{
    if(!szName || !szPassword)
        return -1;
    
    m_mtgID ="";
    
    m_username = szName;
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
    
    return m_pConnectServer->SendData(rPacket);
}

int CVideoChat::logoutServer()
{
    if(!m_pConnectServer)
        return -1;
    [[AudioUnitTool shareHandle]stop];
    
    std::string strinfo = "quitserver";
    KCmdPacketEx rPacket(strinfo.c_str(),(int)strinfo.length()+1);
    std::string strCMD = "QUITSERVER";
    rPacket.SetCMD(strCMD);
    
    m_UserInfoList.clear();
    m_mtgID ="";
    return m_pConnectServer->SendData(rPacket);
}

int CVideoChat::getUserList()
{
    std::string strGetuserlist= "getuserlist";
    KCmdPacketEx rPacket(strGetuserlist.c_str(),(int)strGetuserlist.length()+1);
    rPacket.SetAttrib("ADDRESS", m_strAddress);
    std::string strCMD = "GETUSERLIST";
    rPacket.SetCMD(strCMD);
    return m_pConnectServer->SendData(rPacket);
}

void CVideoChat::LoginServer(const char* szIP,const char* szName, const char* headUrl,
                             const char* nickName,double latitude,double longitude ,const char* szGameId,const char* szGameServerId,const char* szRoomId,const char* szGroupId ,const char* szexpand,bool isencrypt )
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
    m_headUrl  = headUrl;
    m_nickName = nickName;
    m_latitude = latitude;
    m_longitude = longitude;
    
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


int CVideoChat::SendUserMediaStatus(bool isvideo, bool isopen )
{
    if( m_mtgID == "")
    {
        return 0;
    }
    std::string strinfo = "UserMediaStatus";
    KCmdPacketEx rPacket(strinfo.c_str(),(int)strinfo.length()+1);
    std::string strCMD = "USERMEDIASTATUS";
    rPacket.SetCMD(strCMD);
    rPacket.SetAttribBL("ISVIDEO", isvideo);
    rPacket.SetAttribBL("ISOPEN", isopen);
    rPacket.SetAttribUL("USERID", m_uUserID);
    rPacket.SetAttrib("USERNAME", m_username);
    return m_pConnectServer->SendData(rPacket);
}

int CVideoChat::SendMtgCmd(MTG_CMD mtgCmd, std::string mtgid)
{
    
    std::string strinfo = "MTGCMD";
    KCmdPacketEx rPacket(strinfo.c_str(),(int)strinfo.length()+1);
    std::string strCMD = "MTGCMD";
    rPacket.SetCMD(strCMD);
    
    rPacket.SetAttribUL("MTGCMD", mtgCmd);
    rPacket.SetAttrib("MTGID",mtgid);
    if (mtgCmd == MTG_CREAT) {
        rPacket.SetAttribUL("MTGTYPE", m_mtgType);
        
    }
    
    return m_pConnectServer->SendData(rPacket);
    
}

void CVideoChat::creatMtg(std::string mtgid, bool openCamera,MTG_TYPE mtgType)
{
    if(m_mtgID !="")
        return;
    m_openCamera = openCamera;
    m_mtgType = mtgType;
    
    m_mtgID = mtgid;
    openMediaSender();
    SendMtgCmd(MTG_CREAT, mtgid);
}

void CVideoChat::joinMtg(std::string mtgid, bool openCamera)
{
    if(m_mtgID !="")
    {
        
        return;
    }
    m_openCamera = openCamera;
    
    m_mtgID = mtgid;
    
    openMediaSender();
    
    SendMtgCmd(MTG_JOIN, mtgid);
}

void CVideoChat::exitMtg()
{
    if(!m_pConnectServer)
        return ;
    [[AudioUnitTool shareHandle]stop];
    SendMtgCmd(MTG_EXIT, m_mtgID);
    m_mtgID ="";
    
}

void CVideoChat::destroyMtg()
{
    [[AudioUnitTool shareHandle]stop];
    SendMtgCmd(MTG_EXIT, m_mtgID);
    m_mtgID ="";
    
}

int CVideoChat::SendDataToUser(unsigned long uPeerUserID ,const char* pData, unsigned long nLen)
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
    return m_pConnectServer->SendData(rPacket);
}

int CVideoChat::SendAllUserData(const char* pData, unsigned long nLen)
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
    return m_pConnectServer->SendData(rPacket);
}

int CVideoChat::mtgInviteUser(const char* mtgId, const char* iuserId)
{
    if(!mtgId || !iuserId||m_mtgID ==""  )
        return -1;
    
    std::string strinfo = "INVITEMTGUSER";
    KCmdPacketEx rPacket(strinfo.c_str(),(int)strinfo.length()+1);
    std::string strCMD = "INVITEMTGUSER";
    rPacket.SetCMD(strCMD);
    rPacket.SetAttrib("MTGID", m_mtgID);
    rPacket.SetAttrib("USERNAME", m_username);
    
    CLIENTUSERINFOLIST_MAP::iterator iter =m_UserInfoList.begin();
    while (iter!=m_UserInfoList.end())
    {
        CLIENTUSERINFOLIST userinfo=(*iter).second;
        
        if(userinfo.strUserName  == iuserId)
        {
            rPacket.SetAttribUL("PEERUSERID", userinfo.ulUserId);
            return m_pConnectServer->SendData(rPacket);
            
        }
        iter++;
    }
    return 0;
}

std::string CVideoChat::GetLocalIP()
{
    return m_pConnectServer->GetLocalIP();
}

std::string CVideoChat::GetNATIP()
{
    return m_pConnectServer->GetNATIP();
}

void CVideoChat::SendAudioData(char*pData, int nLen)
{
    if (m_OpenLocalUser) {
        m_OpenLocalUser->SendAudioData(pData, nLen);
    }
}

void* CVideoChat::creatVideoWindow(int x,int y,int w,int h)
{
    CGRect frame = CGRectMake(x, y, w, h);
    TouchMoveView *touchMoveView = [[TouchMoveView alloc]initWithFrame:frame];
    touchMoveView.userInteractionEnabled=YES;
    [touchMoveView setBackgroundColor:[UIColor blackColor]];
  [touchMoveView setHidden:YES];
    return touchMoveView;
}

void CVideoChat::reOrientation()
{
    if (m_OpenLocalUser) {
        m_OpenLocalUser->reOrientation();
    }
}

void CVideoChat::openMediaSender()
{
    if (!m_OpenLocalUser) {
        m_OpenLocalUser = new OpenLocalUser;
        m_OpenLocalUser->ConnectMediaServer(m_serverip, LOCAL_MCUPort);
        m_OpenLocalUser->OpenAudioSend((unsigned int)this->m_audioID);
        m_OpenLocalUser->OpenVideoSend((unsigned int)this->m_audioID+200);
    }
}

void CVideoChat::openLocalMedia(bool isCapScreen)
{
    openLocalAudio();
    if (m_openCamera) {
        dispatch_async(dispatch_get_main_queue(), ^{
            if (m_touchMoveView) {
                [m_touchMoveView removeFromSuperview];
                [m_touchMoveView release];
                m_touchMoveView = nil;
            }
            m_touchMoveView = (TouchMoveView*)creatVideoWindow(0,0,120,160);
            m_touchMoveView.userName = [NSString stringWithFormat:@"%s",m_username.c_str()];
            if(m_pINetWorkCallback)
                m_pINetWorkCallback->UserVideoView(m_touchMoveView, m_touchMoveView.userName);
            if(m_OpenLocalUser)
                m_OpenLocalUser->openLocalVideo(m_touchMoveView,1,isCapScreen);
        });
    }
}

void CVideoChat::switchCamera()
{
    if (m_OpenLocalUser) {
        m_OpenLocalUser->switchCamera();
    }
}

void CVideoChat::closeMediaSender()
{
     std::lock_guard<std::recursive_mutex> lock_guard(m_mutex);
    dispatch_sync(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        if (m_OpenLocalUser) {
            closeLocalAudio();
            m_OpenLocalUser->closeLocalVideo();
            m_OpenLocalUser->CloseAudioSend();
            m_OpenLocalUser->CloseVideoSend();
            m_OpenLocalUser->ReleaseMediaSever();
            dispatch_async(dispatch_get_main_queue(), ^{
                if(m_touchMoveView) {
                    [m_touchMoveView removeFromSuperview];
                    [m_touchMoveView release];
                    m_touchMoveView =nil;
                }
                
            });
            delete m_OpenLocalUser;
            m_OpenLocalUser = NULL;
        }
    });
}

void CVideoChat::closeLocalAudio( )
{
    [[AudioUnitTool shareHandle]setSendCmdManage:NULL];
    [[AudioUnitTool shareHandle]stop];
    [AudioUnitTool closeHandle];
    SendUserMediaStatus(false, false);
}

void CVideoChat::openLocalAudio()
{
    [[AudioUnitTool shareHandle]setSendCmdManage:this];
    [[AudioUnitTool shareHandle]start];
    SendUserMediaStatus(false, true);
}

void CVideoChat::setbeautifyFace(bool beautifyFace)
{
    m_beautifyFace = beautifyFace;
}

void CVideoChat::videoChatSnapshot(id<VideoChatDelegate> delegate)
{
    if (m_OpenLocalUser) {
        [[CameraHelp shareCameraHelp] videoChatSnapshot:delegate];
    } else {
        dispatch_async(dispatch_get_main_queue(), ^{
            
            UIWindow * window = [UIApplication sharedApplication].keyWindow;
            UIGraphicsBeginImageContextWithOptions(window.bounds.size, YES, [UIScreen mainScreen].scale);
            
            NSInvocation *invocation = [NSInvocation invocationWithMethodSignature:
                                        [window methodSignatureForSelector:
                                         @selector(drawViewHierarchyInRect:afterScreenUpdates:)]];
            [invocation setTarget:window];
            [invocation setSelector:@selector(drawViewHierarchyInRect:afterScreenUpdates:)];
            CGRect arg2 = window.bounds;
            BOOL arg3 = YES;
            [invocation setArgument:&arg2 atIndex:2];
            [invocation setArgument:&arg3 atIndex:3];
            [invocation invoke];
            UIImage *screenshot = UIGraphicsGetImageFromCurrentImageContext();
            UIGraphicsEndImageContext();
            if (delegate &&[delegate respondsToSelector:@selector(videoChatSnapshot:)]) {
                [delegate videoChatSnapshot:screenshot ];
            }
        });
    }
}

void CVideoChat::openBeautify()
{
    if (m_OpenLocalUser) {
        m_OpenLocalUser->openBeautify();
    }
}

void CVideoChat::closeBeautify()
{
    if (m_OpenLocalUser) {
        m_OpenLocalUser->closeBeautify();
    }
}

void CVideoChat::openLocalVideo()
{
    if (m_OpenLocalUser) {
        if (m_touchMoveView) {
            [m_touchMoveView release];
            m_touchMoveView = nil;
        }
        m_touchMoveView = (TouchMoveView*)creatVideoWindow(0,0,80,80);
        m_touchMoveView.userName = [NSString stringWithFormat:@"%s",m_username.c_str()];
        if(m_pINetWorkCallback)
            m_pINetWorkCallback->UserVideoView(m_touchMoveView, m_touchMoveView.userName);
        m_OpenLocalUser->openLocalVideo(m_touchMoveView,0,NO);
        [m_touchMoveView setHidden:NO];
        SendUserMediaStatus(true, true);
    }
}

void CVideoChat::closeLocalVideo( )
{
    if (m_OpenLocalUser) {
        m_OpenLocalUser->closeLocalVideo();
        [m_touchMoveView setHidden:YES];
        if (m_touchMoveView) {
            [m_touchMoveView release];
            m_touchMoveView = nil;
        }
        SendUserMediaStatus(true, false);
    }
}

void CVideoChat::startAudioUnit()
{
    [[AudioUnitManager shareHandle]stop];
    std::map<unsigned long, class OpenRemoteUser*>::iterator it=m_pOpenRemoteUser_map.begin();
    int i = 0;
    while (it!=m_pOpenRemoteUser_map.end()) {
        OpenRemoteUser *openRemoteUser=(*it).second;
        if(openRemoteUser){
            openRemoteUser->SetAudioOrder(i);
        }
        i++;
        it++;
    }
    [[AudioUnitManager shareHandle]start:(unsigned int)m_pOpenRemoteUser_map.size()];
}

void CVideoChat::StopAllRemoteMedia()
{
     std::lock_guard<std::recursive_mutex> lock_guard(m_mutex);
    std::map<unsigned long, class OpenRemoteUser*>::iterator it=m_pOpenRemoteUser_map.begin();
    while (it!=m_pOpenRemoteUser_map.end()) {
        dispatch_sync(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
            OpenRemoteUser *openRemoteUser=(*it).second;
            if(openRemoteUser) {
                openRemoteUser->CloseAudioReceive();
                openRemoteUser->CloseVideoReceive();
                usleep(500);
                openRemoteUser->ReleaseMediaSever();
                delete  openRemoteUser;
                openRemoteUser =NULL;
            }
        });
        m_pOpenRemoteUser_map.erase(it++);;
    }
    
    [[AudioUnitManager shareHandle]stop];
    if (m_peertouchMoveViews) {
        for(TouchMoveView *moveview in m_peertouchMoveViews) {
            if(moveview) {
                dispatch_async(dispatch_get_main_queue(), ^{
                    [moveview removeFromSuperview];
                    [moveview release];
                } );
            }
        }
        [ m_peertouchMoveViews removeAllObjects];
    }
}

void CVideoChat::setRemoteUserVideoFrame(const char* username ,int x,int y,int width,int height,bool round)
{
    dispatch_async(dispatch_get_main_queue(), ^{
        NSString * strusername = [NSString stringWithFormat:@"%s",username];
        for (  TouchMoveView  *touchview in  m_peertouchMoveViews) {
            if([touchview.userName isEqualToString:strusername]) {
                [touchview setFrame:CGRectMake(x,y,width,height)];
            }
        }
    });
}

void CVideoChat::OpenRemoteUserVideo(const char* username ,int x,int y,int width,int height,bool round)
{
    std::string strusername = username;
    CLIENTUSERINFOLIST_MAP::iterator iter =m_UserInfoList.begin();
    while (iter!=m_UserInfoList.end()) {
        CLIENTUSERINFOLIST userinfo=(*iter).second;
        if(userinfo.strUserName  == strusername) {
            std::map<unsigned long, class OpenRemoteUser*>::iterator it=m_pOpenRemoteUser_map.find(userinfo.ulUserId);
            if (it!=m_pOpenRemoteUser_map.end()) {
                OpenRemoteUser *openRemoteUser=(*it).second;
                TouchMoveView  *touchview =(TouchMoveView*)creatVideoWindow(x,y,width,height);
                [touchview setHidden:NO];
                touchview.userName = [NSString stringWithFormat:@"%s",userinfo.strUserName.c_str()];
                if(m_pINetWorkCallback)
                    m_pINetWorkCallback->UserVideoView(touchview, touchview.userName);
                openRemoteUser->SetVideoWindow(touchview);
                openRemoteUser->OpenVideoReceive( userinfo.ulUserAudioID+200);
                [m_peertouchMoveViews addObject:touchview];
            }
            break;
        }
        iter++;
    }
}

void CVideoChat::closeRemoteUserVideo(const char* username)
{
     std::lock_guard<std::recursive_mutex> lock_guard(m_mutex);
    std::string strusername = username;
    CLIENTUSERINFOLIST_MAP::iterator iter =m_UserInfoList.begin();
    while (iter!=m_UserInfoList.end()) {
        CLIENTUSERINFOLIST userinfo=(*iter).second;
        if(userinfo.strUserName  == strusername) {
            std::map<unsigned long, class OpenRemoteUser*>::iterator it=m_pOpenRemoteUser_map.find(userinfo.ulUserId);
            if (it!=m_pOpenRemoteUser_map.end()) {
                dispatch_async(dispatch_get_main_queue(), ^{
                    OpenRemoteUser *openRemoteUser=(*it).second;
                    openRemoteUser->CloseVideoReceive();
                    openRemoteUser->SetVideoWindow(NULL);
                    if (m_peertouchMoveViews) {
                        [m_peertouchMoveViews enumerateObjectsUsingBlock:^(id obj, NSUInteger idx, BOOL *stop) {
                         
                         TouchMoveView *moveView =(TouchMoveView*)obj;
                         
                         if ([moveView.userName isEqualToString:[NSString stringWithFormat:@"%s",username]]) {
                         *stop = YES;
                         if (*stop == YES) {
                         [moveView removeFromSuperview];
                         [m_peertouchMoveViews removeObject:obj];
                         }
                         }
                         }];
                    }
                });
            }
            break;
        }
        iter++;
    }
}

void CVideoChat::OpenRemoteUserAudio(const char* username)
{
    std::string strusername = username;
    
    CLIENTUSERINFOLIST_MAP::iterator iter =m_UserInfoList.begin();
    while (iter!=m_UserInfoList.end()) {
        CLIENTUSERINFOLIST userinfo=(*iter).second;
        if(userinfo.strUserName  == strusername) {
            std::map<unsigned long, class OpenRemoteUser*>::iterator it=m_pOpenRemoteUser_map.find(userinfo.ulUserId);
            if (it!=m_pOpenRemoteUser_map.end()){
                OpenRemoteUser *openRemoteUser=(*it).second;
                openRemoteUser->OpenAudioReceive(userinfo.ulUserAudioID);
            }
            break;
        }
        iter++;
    }
    startAudioUnit();
}

void CVideoChat::closeRemoteUserAudio(const char* username)
{
    std::string strusername = username;
    CLIENTUSERINFOLIST_MAP::iterator iter =m_UserInfoList.begin();
    while (iter!=m_UserInfoList.end()) {
        CLIENTUSERINFOLIST userinfo=(*iter).second;
        if(userinfo.strUserName  == strusername) {
            std::map<unsigned long, class OpenRemoteUser*>::iterator it=m_pOpenRemoteUser_map.find(userinfo.ulUserId);
            if (it!=m_pOpenRemoteUser_map.end()) {
                OpenRemoteUser *openRemoteUser=(*it).second;
                openRemoteUser->CloseAudioReceive();
            }
            break;
        }
        iter++;
    }
    startAudioUnit();
}
