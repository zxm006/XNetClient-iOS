//
//  AiYouVideoChatMgr.c
//  VideoChatMicTalk
//
//  Created by zhangxinming on 17/7/11.
//  Copyright © 2016年 zhangxinming. All rights reserved.
//

#include "VideoChatMgr.h"
#include "CVideoChat.h"
#include <string.h>
#import <UIKit/UIKit.h> 
#import <AVFoundation/AVFoundation.h>
#define IP_SIZE     16  

static const char *mediaServer_URL = "media.aigamecloud.com";

id<VideoChatDelegate> m_VideoChatDelegate = nil;

static CVideoChat   *m_CVideoChat    =  NULL;

class IVideoChat
:public IVideoChatCallback
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

static IVideoChat * m_IVideoChat =NULL;

void  IVideoChat::UserVideoView(UIView* view,NSString*userid)
{
    if (m_VideoChatDelegate &&[m_VideoChatDelegate respondsToSelector:@selector(UserVideoView:userid:)]) {
        dispatch_async(dispatch_get_main_queue(), ^{
            
        [m_VideoChatDelegate UserVideoView:view userid:userid];
        });
        
    }
}
void IVideoChat::UserMediaStatus(std::string strName,bool isvideo,bool isopen)
{
    if (m_VideoChatDelegate &&[m_VideoChatDelegate respondsToSelector:@selector(UserMediaStatus:isvideo:isopen:)]) {
        dispatch_async(dispatch_get_main_queue(), ^{
            
        [m_VideoChatDelegate UserMediaStatus:strName isvideo:isvideo isopen:isopen];
        });
        
    }
}

void IVideoChat::MtgInfoReg(MTG_CMD mtgCmd,bool result,std::string &info)
{
    if (m_VideoChatDelegate &&[m_VideoChatDelegate respondsToSelector:@selector(MtgInfoReg:result:info:)]) {
        
        dispatch_async(dispatch_get_main_queue(), ^{
            
        [m_VideoChatDelegate MtgInfoReg:mtgCmd result:result info:info];
        });
    }

}

void IVideoChat::MtgInfo(MTG_CMD mtgCmd,std::string username ,std::string &info)
{
    if (m_VideoChatDelegate &&[m_VideoChatDelegate respondsToSelector:@selector(MtgInfo:username:info:)]) {
        dispatch_async(dispatch_get_main_queue(), ^{
            
        [m_VideoChatDelegate MtgInfo:mtgCmd username:username info:info];
        });
    }
    
}

void IVideoChat::MtgInvite(std::string username ,std::string mtgId)
{
    if (m_VideoChatDelegate &&[m_VideoChatDelegate respondsToSelector:@selector(MtgInvite:mtgId:)]) {
        dispatch_async(dispatch_get_main_queue(), ^{
            
        [m_VideoChatDelegate MtgInvite:username mtgId:mtgId];
        });
    }
}

void IVideoChat::MtgJoin(CLIENTUSERINFOLIST &userinfo)
{
    if (m_VideoChatDelegate &&[m_VideoChatDelegate respondsToSelector:@selector(MtgJoin:)]) {
        dispatch_async(dispatch_get_main_queue(), ^{
            
            [m_VideoChatDelegate MtgJoin:userinfo];
        });
    }
}
  void IVideoChat::MtgExit(CLIENTUSERINFOLIST &userinfo)
{
    if (m_VideoChatDelegate &&[m_VideoChatDelegate respondsToSelector:@selector(MtgExit:)]) {
        dispatch_async(dispatch_get_main_queue(), ^{
            
            [m_VideoChatDelegate MtgExit:userinfo];
        });
    }
    
}
void  IVideoChat::MtgUserList(CLIENTUSERINFOLIST_MAP& UserInfoList)
{
    if (m_VideoChatDelegate &&[m_VideoChatDelegate respondsToSelector:@selector(MtgUserList:)]) {
        dispatch_async(dispatch_get_main_queue(), ^{
            
            [m_VideoChatDelegate MtgUserList:UserInfoList];
        });
    }
}

void IVideoChat::IConnectStatusCallback(CONNECT_NET_STATUS cs)
{
    if(cs == CS_LOGOUT||cs == CS_LOGINFAILED)
    {
       if (m_CVideoChat)
        {
            dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        
            m_CVideoChat->disconnect();
            usleep(500000);
            
            delete  m_CVideoChat;
            m_CVideoChat = NULL;
                
                
            });
        }
        
        if (m_IVideoChat)
        {
            delete m_IVideoChat;
            m_IVideoChat =NULL;
        }
        
        if (m_VideoChatDelegate &&[m_VideoChatDelegate respondsToSelector:@selector(IConnectStatusCallback:)]) {
            dispatch_async(dispatch_get_main_queue(), ^{
                
            [m_VideoChatDelegate IConnectStatusCallback:cs];
            });
            
            
        }
        return;
        
    }
    
    if (m_VideoChatDelegate &&[m_VideoChatDelegate respondsToSelector:@selector(IConnectStatusCallback:)]) {
//        dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        
        dispatch_async(dispatch_get_main_queue(), ^{
            
        [m_VideoChatDelegate IConnectStatusCallback:cs];
//        });
            });
    }
    

}

void IVideoChat::INetReceiveUserList(CLIENTUSERINFOLIST_MAP& UserInfoList)
{
    
    if (m_VideoChatDelegate &&[m_VideoChatDelegate respondsToSelector:@selector(INetReceiveUserList:)]) {
        dispatch_async(dispatch_get_main_queue(), ^{
            
        [m_VideoChatDelegate INetReceiveUserList:UserInfoList];
        });
    }
   
}

void IVideoChat::INetReceiveUserLogin(unsigned long uPeerUserID, std::string strName,unsigned long userAudioID)
{
   
    if (m_VideoChatDelegate &&[m_VideoChatDelegate respondsToSelector:@selector(INetReceiveUserLogin:strName:useraudioID:)]) {
        dispatch_async(dispatch_get_main_queue(), ^{
            
        [m_VideoChatDelegate INetReceiveUserLogin:uPeerUserID strName:strName useraudioID:userAudioID];
        });
    }
    
}

void IVideoChat::INetReceiveUserLogOut(unsigned long uPeerUserID)
{
    
    if (m_VideoChatDelegate &&[m_VideoChatDelegate respondsToSelector:@selector(INetReceiveUserLogOut:)]) {
        dispatch_async(dispatch_get_main_queue(), ^{
            
        [m_VideoChatDelegate INetReceiveUserLogOut:uPeerUserID ];
        });
        
    }
   
}

void IVideoChat::INetReceiveData(unsigned long uPeerUserID, std::string strName, std::string strData, unsigned long nLen)
{
    
    if (m_VideoChatDelegate &&[m_VideoChatDelegate respondsToSelector:@selector(INetReceiveData:strName:strData:nLen:)]) {
        dispatch_async(dispatch_get_main_queue(), ^{
            
        [m_VideoChatDelegate INetReceiveData:uPeerUserID strName:strName.c_str() strData:strData.c_str() nLen:nLen];
        });
    }
    
    
}
void IVideoChat::INetBroadcastData(unsigned long uPeerUserID, const char* pData, unsigned long nLen)
{
   
    
    if (m_VideoChatDelegate &&[m_VideoChatDelegate respondsToSelector:@selector(INetBroadcastData:pData:nLen:)]) {
        
        [m_VideoChatDelegate INetBroadcastData:uPeerUserID pData:pData  nLen:nLen];
        
    }
}

bool checkVideoChatMicAudio()
{
    AVAudioSession *avSession = [AVAudioSession sharedInstance];
    
    __block bool ret=YES;
    
    if ([avSession respondsToSelector:@selector(requestRecordPermission:)]) {
        
        [avSession requestRecordPermission:^(BOOL available) {
         ret= available;
         if (available) {
     
         }
         else
         {
         dispatch_async(dispatch_get_main_queue(), ^{
            [[[UIAlertView alloc] initWithTitle:@"无法录音" message:@"请在“设置-隐私-麦克风”选项中允许融企信访问你的麦克风" delegate:nil cancelButtonTitle:@"确定" otherButtonTitles:nil] show];
            //        return available;
        });
         }
         }];
        
    }
    return ret;
}

bool checkVideoChatMicVideo()
{
    NSString *mediaType = AVMediaTypeVideo;// Or AVMediaTypeAudio
    AVAuthorizationStatus authStatus = [AVCaptureDevice authorizationStatusForMediaType:mediaType];
    
    // This status is normally not visible—the AVCaptureDevice class methods for discovering devices do not return devices the user is restricted from accessing.
    if(authStatus ==AVAuthorizationStatusAuthorized){
//        DLog(@"Restricted");
        
        return YES;
        
    }else if(authStatus == AVAuthorizationStatusDenied){
        // The user has explicitly denied permission for media capture.
        DLog(@"Denied");     //应该是这个，如果不允许的话
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

int checkIsDomain(const char *pAddr)
{
    if(!pAddr || strlen(pAddr) < 1)
    {
        return -1;
    }
    unsigned long  add = inet_addr(pAddr);
    if (add == INADDR_NONE)
    {
        //printf("");
        return 0;
    }
    else
    {
        return 1;
    }
}

int get_ip_by_domain(const char *domain, char *ip)
  {
      char **pptr;
      struct hostent *hptr;
  
      hptr = gethostbyname(domain);
      if(NULL == hptr)
   {
              printf("gethostbyname error for host:%s/n", domain);
              return -1;
          }
  
      for(pptr = hptr->h_addr_list ; *pptr != NULL; pptr++)
       {
         if (NULL != inet_ntop(hptr->h_addrtype, *pptr, ip, IP_SIZE) )
             {
             return 0; // 只获取第一个 ip
                   }
           }
    
      return -1;  
  }


void  setVideoChatDelegate( id<VideoChatDelegate> delegate)
{
    m_VideoChatDelegate = delegate;
}

void loginVideoChatServer(const char* szServerHostURL,const char* szUserName, const char* headUrl,
                    const char* nickName,long latitude,long longitude,const char* gameid,bool isencrypt)
{
  
    
    char ip[IP_SIZE];
    
    if (checkIsDomain(szServerHostURL)==1)
    {
        memcpy(ip, szServerHostURL, IP_SIZE);
    }
    else
    {
   int  ret=  get_ip_by_domain(mediaServer_URL, ip);
        
    if (ret!=0)
      {
        return;
     
       }
    }
    
     if(!checkVideoChatMicAudio())
            return;
        
        
        if(!checkVideoChatMicVideo())
            return;
    
    
   if(m_CVideoChat)
    {
      logoutVideoChatServer();
      usleep(1000000);
   }
  
    m_CVideoChat = new CVideoChat();
    
    if (m_IVideoChat) {
        delete m_IVideoChat;
        m_IVideoChat =NULL;
    }
    
    m_IVideoChat = new IVideoChat;
    
    m_CVideoChat->setNetWorkCallback(m_IVideoChat);
    
    if(m_CVideoChat)
    {
      m_CVideoChat->LoginServer(ip, szUserName,headUrl,nickName,latitude, longitude,"VideoChat","VideoChat",gameid,"VideoChat","NULL",isencrypt);
    }
}

void  openVideoChatVideoWtihName(const char* username ,int x,int y,int width,int height,bool round)
{
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        
        if(m_CVideoChat)
        {
            m_CVideoChat->OpenRemoteUserVideo(username,x,y,width,height,round);
            
        }
    });
}

void  setVideoChatRemoteUserVideoFrame(const char* username ,int x,int y,int width,int height,bool round)
{
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        
        if(m_CVideoChat)
        {
            m_CVideoChat->setRemoteUserVideoFrame(username,x,y,width,height,round);
            
        }
    });
}

void  closeVideoChatVideoWtihName(const char *name)
{
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
    
        if(m_CVideoChat)
        {
            m_CVideoChat->closeRemoteUserVideo(name);
            
        }
    });
}

void openVideoChatRemoteUserAudio(const char* username)
{
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        
        if(m_CVideoChat)
        {
            m_CVideoChat->OpenRemoteUserAudio(username);
            
        }
    });
}

void closeVideoChatRemoteUserAudio(const char* username)
{
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        
        if(m_CVideoChat)
        {
            m_CVideoChat->closeRemoteUserAudio(username);
            
        }
    });
}

void  openVideoChatLocalVideo()
{
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        
        if(m_CVideoChat)
        {
            m_CVideoChat->openLocalVideo();
            
        }
    });
}

void  closeVideoChatLocalVideo()
{
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        
        if(m_CVideoChat)
        {
            m_CVideoChat->closeLocalVideo();
            
        }
    });
}

void  mtgInviteUser(const char* mtgId, const char* iuserId)
{
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        
        if(m_CVideoChat)
        {
            m_CVideoChat->mtgInviteUser(mtgId,iuserId);
            
        }
    });
}

void  setVideoChatLocalCamera(bool isFront)
{
    if(m_CVideoChat)
    {
        m_CVideoChat->setLocalCamera(isFront);
        
    }
}
void  resetVideoChatPreview()
{
    if(m_CVideoChat)
    {
        m_CVideoChat->resetPreview();
        
    }
}

void  logoutVideoChatServer()
{
    if(m_CVideoChat)
    {
        m_CVideoChat->logoutServer();
 
    }
}

 void setVideoChatSession()
{
    if(m_CVideoChat)
    {
        m_CVideoChat->setAudioSession();
        
    }
}
 
void creatVideoChat(std::string mtgid, bool openCamera)
{
    if(m_CVideoChat)
    {
        m_CVideoChat->creatMtg(mtgid,openCamera,MTG_Common);
    }
}

void joinVideoChat(std::string mtgid, bool openCamera)
{
    if(m_CVideoChat)
    {
        m_CVideoChat->joinMtg(mtgid,openCamera);
        
    }
}

void exitVideoChat()
{
    if(m_CVideoChat)
    {
        m_CVideoChat->exitMtg();
        
    }
}

void destroyVideoChat()
{
    if(m_CVideoChat)
    {
        m_CVideoChat->destroyMtg();
        
    }
}


void openVideoChatLocalAudio()
{
if(m_CVideoChat)
{
    m_CVideoChat->openLocalAudio();
    
}
}

void closeVideoChatLocalAudio()
{
    if(m_CVideoChat)
    {
        m_CVideoChat->closeLocalAudio();
        
    }
}

void  setVideoChatBeautifyFace(bool beautifyFace)
{

    if(m_CVideoChat)
    {
        m_CVideoChat->setbeautifyFace(beautifyFace);
        
    }
}

