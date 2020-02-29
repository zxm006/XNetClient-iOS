//
//  AiYouVoicechatMgr.c
//  VoicechatMicTalk
//
//  Created by zhangxinming on 17/7/11.
//  Copyright © 2016年 zhangxinming. All rights reserved.
//

#include "VoicechatMgr.h"
#include "Voicechat.h"
#include <string.h>
#import <UIKit/UIKit.h> 
#import <AVFoundation/AVFoundation.h>
//#import "VoicechatCommon.h"

id<VoiceChatDelegate> m_VoicechatDelegate = nil;

static Voicechat   *m_Voicechat    =  NULL;

class IVoicechat
:public IVoicechatCallback
{
public:
    virtual void  UserVideoView(UIView* view,NSString*userid);
    
    virtual void  UserMediaStatus(std::string strName,bool isvideo,bool isopen);
    
    virtual void  MtgInfoReg(MTG_CMD mtgCmd,bool result,std::string &info);
    virtual void  MtgInfo(MTG_CMD mtgCmd,std::string username ,std::string &info);
    virtual void  MtgInvite(std::string username ,std::string mtgId);
    virtual void  MtgJoin(CLIENTUSERINFOLIST &userinfo);
    virtual void  MtgExit(CLIENTUSERINFOLIST &userinfo);
    virtual void  MtgUserList(CLIENTUSERINFOLIST_MAP& UserInfoList);
    virtual void IConnectStatusCallback(CONNECT_NET_STATUS cs);
    virtual void INetReceiveUserList(CLIENTUSERINFOLIST_MAP& UserInfoList);
    virtual void INetReceiveUserLogin(unsigned long uPeerUserID, std::string strName,unsigned long useraudioId);
    virtual void INetReceiveUserLogOut(unsigned long uPeerUserID);
    virtual void INetReceiveData(unsigned long uPeerUserID, std::string strName, std::string strData, unsigned long nLen);
    virtual void INetBroadcastData(unsigned long uPeerUserID, const char* pData, unsigned long nLen);
    
};

static IVoicechat * m_IVoicechat =NULL;


void  IVoicechat::UserVideoView(UIView* view,NSString*userid)
{
    if (m_VoicechatDelegate &&[m_VoicechatDelegate respondsToSelector:@selector(UserVideoView:userid:)]) {
        dispatch_async(dispatch_get_main_queue(), ^{
            
            [m_VoicechatDelegate UserVideoView:view userid:userid];
        });
        
    }
}
void IVoicechat::UserMediaStatus(std::string strName,bool isvideo,bool isopen)
{
    if (m_VoicechatDelegate &&[m_VoicechatDelegate respondsToSelector:@selector(UserMediaStatus:isvideo:isopen:)]) {
        dispatch_async(dispatch_get_main_queue(), ^{
            
            [m_VoicechatDelegate UserMediaStatus:strName isvideo:isvideo isopen:isopen];
        });
        
    }
}

void IVoicechat::MtgInfoReg(MTG_CMD mtgCmd,bool result,std::string &info)
{
    if (m_VoicechatDelegate &&[m_VoicechatDelegate respondsToSelector:@selector(VoicechatInfoReg:result:info:)]) {
        
        dispatch_async(dispatch_get_main_queue(), ^{
            
            [m_VoicechatDelegate VoicechatInfoReg:mtgCmd result:result info:info];
        });
    }
    
}

void IVoicechat::MtgInfo(MTG_CMD mtgCmd,std::string username ,std::string &info)
{
    if (m_VoicechatDelegate &&[m_VoicechatDelegate respondsToSelector:@selector(VoicechatInfo:username:info:)]) {
        dispatch_async(dispatch_get_main_queue(), ^{
            
            [m_VoicechatDelegate VoicechatInfo:mtgCmd username:username info:info];
        });
    }
    
}

void IVoicechat::MtgInvite(std::string username ,std::string mtgId)
{
    if (m_VoicechatDelegate &&[m_VoicechatDelegate respondsToSelector:@selector(VoicechatInvite:VoicechatId:)]) {
        dispatch_async(dispatch_get_main_queue(), ^{
            
            [m_VoicechatDelegate VoicechatInvite:username VoicechatId:mtgId];
        });
    }
}

void IVoicechat::MtgJoin(CLIENTUSERINFOLIST &userinfo)
{
    if (m_VoicechatDelegate &&[m_VoicechatDelegate respondsToSelector:@selector(VoicechatJoin:)]) {
        dispatch_async(dispatch_get_main_queue(), ^{
            
            [m_VoicechatDelegate VoicechatJoin:userinfo];
        });
    }
}
void IVoicechat::MtgExit(CLIENTUSERINFOLIST &userinfo)
{
    if (m_VoicechatDelegate &&[m_VoicechatDelegate respondsToSelector:@selector(VoicechatExit:)]) {
        dispatch_async(dispatch_get_main_queue(), ^{
            
            [m_VoicechatDelegate VoicechatExit:userinfo];
        });
    }
    
}

void  IVoicechat::MtgUserList(CLIENTUSERINFOLIST_MAP& UserInfoList)
{
    if (m_VoicechatDelegate &&[m_VoicechatDelegate respondsToSelector:@selector(VoicechatJoin:)]) {
        [m_VoicechatDelegate VoiceChatUserList:UserInfoList];
        
    }
}

void IVoicechat::IConnectStatusCallback(CONNECT_NET_STATUS cs)
{
    if(cs == CS_LOGOUT||cs == CS_LOGINFAILED||cs == CS_RELOGIN) {
        if (m_Voicechat) {
            dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
                m_Voicechat->disconnect();
                usleep(500000);
                delete  m_Voicechat;
                m_Voicechat = NULL;
            });
        }
        if (m_IVoicechat) {
            delete m_IVoicechat;
            m_IVoicechat =NULL;
        }
        if (m_VoicechatDelegate &&[m_VoicechatDelegate respondsToSelector:@selector(IConnectStatusCallback:)]) {
            dispatch_async(dispatch_get_main_queue(), ^{
            [m_VoicechatDelegate IConnectStatusCallback:cs];
            });
        }
        return;
    }
    
    if (m_VoicechatDelegate &&[m_VoicechatDelegate respondsToSelector:@selector(IConnectStatusCallback:)]) {
        //        dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        
        dispatch_async(dispatch_get_main_queue(), ^{
            [m_VoicechatDelegate IConnectStatusCallback:cs];
            //        });
        });
    }
    
    
}

void IVoicechat::INetReceiveUserList(CLIENTUSERINFOLIST_MAP& UserInfoList)
{
    
    if (m_VoicechatDelegate &&[m_VoicechatDelegate respondsToSelector:@selector(INetReceiveUserList:)]) {
        dispatch_async(dispatch_get_main_queue(), ^{
            
            [m_VoicechatDelegate INetReceiveUserList:UserInfoList];
        });
    }
    
}

void IVoicechat::INetReceiveUserLogin(unsigned long uPeerUserID, std::string strName,unsigned long userAudioID)
{
    
    if (m_VoicechatDelegate &&[m_VoicechatDelegate respondsToSelector:@selector(INetReceiveUserLogin:strName:useraudioID:)]) {
        dispatch_async(dispatch_get_main_queue(), ^{
            
            [m_VoicechatDelegate INetReceiveUserLogin:uPeerUserID strName:strName useraudioID:userAudioID];
        });
    }
    
}

void IVoicechat::INetReceiveUserLogOut(unsigned long uPeerUserID)
{
    if (m_VoicechatDelegate &&[m_VoicechatDelegate respondsToSelector:@selector(INetReceiveUserLogOut:)]) {
        dispatch_async(dispatch_get_main_queue(), ^{
            
            [m_VoicechatDelegate INetReceiveUserLogOut:uPeerUserID ];
        });
    }
}

void IVoicechat::INetReceiveData(unsigned long uPeerUserID, std::string strName, std::string strData, unsigned long nLen)
{
    if (m_VoicechatDelegate &&[m_VoicechatDelegate respondsToSelector:@selector(INetReceiveData:strName:strData:nLen:)]) {
        dispatch_async(dispatch_get_main_queue(), ^{
            
            [m_VoicechatDelegate INetReceiveData:uPeerUserID strName:strName.c_str() strData:strData.c_str() nLen:nLen];
        });
    }
}

void IVoicechat::INetBroadcastData(unsigned long uPeerUserID, const char* pData, unsigned long nLen)
{
    if (m_VoicechatDelegate &&[m_VoicechatDelegate respondsToSelector:@selector(INetBroadcastData:pData:nLen:)]) {
        [m_VoicechatDelegate INetBroadcastData:uPeerUserID pData:pData  nLen:nLen];
        
    }
}

bool checkVoicechatMicAudio()
{
    AVAudioSession *avSession = [AVAudioSession sharedInstance];
    
    __block bool ret=YES;
    
    if ([avSession respondsToSelector:@selector(requestRecordPermission:)]) {
        
        [avSession requestRecordPermission:^(BOOL available) {
         ret= available;
         if (available) {
         }
         else {
         dispatch_async(dispatch_get_main_queue(), ^{
            [[[UIAlertView alloc] initWithTitle:@"无法录音" message:@"请在“设置-隐私-麦克风”选项中允许融企信访问你的麦克风" delegate:nil cancelButtonTitle:@"确定" otherButtonTitles:nil] show];
            //        return available;
        });
         }
         }];
    }
    return ret;
}

bool checkVoicechatMicVideo()
{
    NSString *mediaType = AVMediaTypeVideo;// Or AVMediaTypeAudio
    AVAuthorizationStatus authStatus = [AVCaptureDevice authorizationStatusForMediaType:mediaType];
    
    // This status is normally not visible—the AVCaptureDevice class methods for discovering devices do not return devices the user is restricted from accessing.
    if(authStatus ==AVAuthorizationStatusAuthorized){
        //        //VoicechatLOG(@"Restricted");
        
        return YES;
        
    }else if(authStatus == AVAuthorizationStatusDenied){
        // The user has explicitly denied permission for media capture.
//        //VoicechatLOG(@"Denied");     //应该是这个，如果不允许的话
        UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"提示"
                                                        message:@"请在设备的\"设置-隐私-相机\"中允许访问相机。"
                                                       delegate:nil
                                              cancelButtonTitle:@"确定"
                                              otherButtonTitles:nil];
        [alert show];
        return NO;
    }
    return YES;
}

int checkVoicechatIsDomain(const char *pAddr)
{
    if(!pAddr || strlen(pAddr) < 1)
    {
        return -1;
    }
    unsigned long  add = inet_addr(pAddr);
    if (add == INADDR_NONE) {
        //printf("");
        return 0;
    }
    else {
        return 1;
    }
}

int get_Voicechat_ip_by_domain(const char *domain, char *ip)
{
    char **pptr;
    struct hostent *hptr;
    
    hptr = gethostbyname(domain);
    if(NULL == hptr) {
        printf("gethostbyname error for host:%s/n", domain);
        return -1;
    }
    
    for(pptr = hptr->h_addr_list ; *pptr != NULL; pptr++) {
        if (NULL != inet_ntop(hptr->h_addrtype, *pptr, ip, IP_SIZE) ) {
            return 0; // 只获取第一个 ip
        }
    }
    
    return -1;
}

void  setVoicechatDelegate( id<VoiceChatDelegate> delegate)
{
    m_VoicechatDelegate = delegate;
}

void loginVoicechatServer(const char* szServerHostURL,const char* szUserName, const char* headUrl, const char* nickName,double latitude,double longitude,const char* gameid,bool isencrypt)
{
    char ip[IP_SIZE];
    if (checkVoicechatIsDomain(szServerHostURL)==1) {
        memcpy(ip, szServerHostURL, IP_SIZE);
    }
    else {
        int  ret=  get_Voicechat_ip_by_domain(mediaServer_URL, ip);
        if (ret!=0) {
            return;
        }
    }
    
    if(!checkVoicechatMicAudio())
        return;
    
    if(m_Voicechat) {
        logoutVoicechatServer();
        usleep(1000000);
    }
    
    m_Voicechat = new Voicechat();
    
    if (m_IVoicechat) {
        delete m_IVoicechat;
        m_IVoicechat =NULL;
    }
    m_IVoicechat = new IVoicechat;
    m_Voicechat->setNetWorkCallback(m_IVoicechat);
    if(m_Voicechat) {
        m_Voicechat->LoginServer(ip, szUserName,headUrl,nickName,latitude, longitude,"Voicechat","Voicechat",gameid,"Voicechat","NULL",isencrypt);
    }
}

void openVoicechatRemoteUserAudio(const char* username)
{
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        if(m_Voicechat) {
            m_Voicechat->OpenRemoteUserAudio(username);
            
        }
    });
}

void closeVoicechatRemoteUserAudio(const char* username)
{
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        if(m_Voicechat) {
            m_Voicechat->closeRemoteUserAudio(username);
        }
    });
}

void  VoicechatInviteUser(const char* mtgId, const char* iuserId)
{
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        if(m_Voicechat) {
            m_Voicechat->mtgInviteUser(mtgId,iuserId);
            
        }
    });
}

void  logoutVoicechatServer()
{
    if(m_Voicechat) {
        m_Voicechat->logoutServer();
    }
}

void setVoicechatSession()
{
    if(m_Voicechat) {
        m_Voicechat->setAudioSession();
    }
}

void creatVoicechat(std::string mtgid)
{
    if(m_Voicechat) {
        m_Voicechat->creatMtg(mtgid,MTG_Common);
    }
}

void joinVoicechat(std::string mtgid)
{
    if(m_Voicechat) {
        m_Voicechat->joinMtg(mtgid);
    }
}

void exitVoicechat()
{
    if(m_Voicechat) {
        m_Voicechat->exitMtg();
    }
}

void destroyVoicechat()
{
    if(m_Voicechat)
    {
        m_Voicechat->destroyMtg();
        
    }
}

void startVoicechatAudio()
{
    if(m_Voicechat) {
        m_Voicechat->openLocalAudio();
    }
}

void stopVoicechatAudio()
{
    if(m_Voicechat) {
        m_Voicechat->closeLocalAudio();
    }
}


