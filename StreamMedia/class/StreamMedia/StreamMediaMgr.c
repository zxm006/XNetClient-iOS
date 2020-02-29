//
//  AiYouStreamMediaMgr.c
//  StreamMediaMicTalk
//
//  Created by zhangxinming on 17/7/11.
//  Copyright © 2016年 zhangxinming. All rights reserved.
//

#include "StreamMediaMgr.h"
#include "CStreamMedia.h"
#include <string.h>
#import <UIKit/UIKit.h> 
#import <AVFoundation/AVFoundation.h>
#define IP_SIZE     16  

static const char *mediaServer_URL = "media.aigamecloud.com";

id<StreamMediaDelegate> m_StreamMediaDelegate = nil;

static CStreamMedia   *m_CStreamMedia    =  NULL;

class IStreamMedia
:public IStreamMediaCallback
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

static IStreamMedia * m_IStreamMedia =NULL;

void  IStreamMedia::UserVideoView(UIView* view,NSString*userid)
{
    if (m_StreamMediaDelegate &&[m_StreamMediaDelegate respondsToSelector:@selector(UserVideoView:userid:)]) {
        dispatch_async(dispatch_get_main_queue(), ^{
            
        [m_StreamMediaDelegate UserVideoView:view userid:userid];
        });
        
    }
}
void IStreamMedia::UserMediaStatus(std::string strName,bool isvideo,bool isopen)
{
    if (m_StreamMediaDelegate &&[m_StreamMediaDelegate respondsToSelector:@selector(UserMediaStatus:isvideo:isopen:)]) {
        dispatch_async(dispatch_get_main_queue(), ^{
            
        [m_StreamMediaDelegate UserMediaStatus:strName isvideo:isvideo isopen:isopen];
        });
        
    }
}

void IStreamMedia::MtgInfoReg(MTG_CMD mtgCmd,bool result,std::string &info)
{
    if (m_StreamMediaDelegate &&[m_StreamMediaDelegate respondsToSelector:@selector(MtgInfoReg:result:info:)]) {
        
        dispatch_async(dispatch_get_main_queue(), ^{
            
        [m_StreamMediaDelegate MtgInfoReg:mtgCmd result:result info:info];
        });
    }

}

void IStreamMedia::MtgInfo(MTG_CMD mtgCmd,std::string username ,std::string &info)
{
    if (m_StreamMediaDelegate &&[m_StreamMediaDelegate respondsToSelector:@selector(MtgInfo:username:info:)]) {
        dispatch_async(dispatch_get_main_queue(), ^{
            
        [m_StreamMediaDelegate MtgInfo:mtgCmd username:username info:info];
        });
    }
    
}

void IStreamMedia::MtgInvite(std::string username ,std::string mtgId)
{
    if (m_StreamMediaDelegate &&[m_StreamMediaDelegate respondsToSelector:@selector(MtgInvite:mtgId:)]) {
        dispatch_async(dispatch_get_main_queue(), ^{
            
        [m_StreamMediaDelegate MtgInvite:username mtgId:mtgId];
        });
    }
}

void IStreamMedia::MtgJoin(CLIENTUSERINFOLIST &userinfo)
{
    if (m_StreamMediaDelegate &&[m_StreamMediaDelegate respondsToSelector:@selector(MtgJoin:)]) {
        dispatch_async(dispatch_get_main_queue(), ^{
            
            [m_StreamMediaDelegate MtgJoin:userinfo];
        });
    }
}
  void IStreamMedia::MtgExit(CLIENTUSERINFOLIST &userinfo)
{
    if (m_StreamMediaDelegate &&[m_StreamMediaDelegate respondsToSelector:@selector(MtgExit:)]) {
        dispatch_async(dispatch_get_main_queue(), ^{
            
            [m_StreamMediaDelegate MtgExit:userinfo];
        });
    }
    
}
void  IStreamMedia::MtgUserList(CLIENTUSERINFOLIST_MAP& UserInfoList)
{
    if (m_StreamMediaDelegate &&[m_StreamMediaDelegate respondsToSelector:@selector(MtgUserList:)]) {
        dispatch_async(dispatch_get_main_queue(), ^{
            
            [m_StreamMediaDelegate MtgUserList:UserInfoList];
        });
    }
}

void IStreamMedia::IConnectStatusCallback(CONNECT_NET_STATUS cs)
{
    if(cs == CS_LOGOUT||cs == CS_LOGINFAILED)
    {
       if (m_CStreamMedia)
        {
            dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        
            m_CStreamMedia->disconnect();
            usleep(500000);
            
            delete  m_CStreamMedia;
            m_CStreamMedia = NULL;
                
                
            });
        }
        
        if (m_IStreamMedia)
        {
            delete m_IStreamMedia;
            m_IStreamMedia =NULL;
        }
        
        if (m_StreamMediaDelegate &&[m_StreamMediaDelegate respondsToSelector:@selector(IConnectStatusCallback:)]) {
            dispatch_async(dispatch_get_main_queue(), ^{
                
            [m_StreamMediaDelegate IConnectStatusCallback:cs];
            });
            
            
        }
        return;
        
    }
    
    if (m_StreamMediaDelegate &&[m_StreamMediaDelegate respondsToSelector:@selector(IConnectStatusCallback:)]) {
//        dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        
        dispatch_async(dispatch_get_main_queue(), ^{
            
        [m_StreamMediaDelegate IConnectStatusCallback:cs];
//        });
            });
    }
    

}

void IStreamMedia::INetReceiveUserList(CLIENTUSERINFOLIST_MAP& UserInfoList)
{
    
    if (m_StreamMediaDelegate &&[m_StreamMediaDelegate respondsToSelector:@selector(INetReceiveUserList:)]) {
        dispatch_async(dispatch_get_main_queue(), ^{
            
        [m_StreamMediaDelegate INetReceiveUserList:UserInfoList];
        });
    }
   
}

void IStreamMedia::INetReceiveUserLogin(unsigned long uPeerUserID, std::string strName,unsigned long userAudioID)
{
   
    if (m_StreamMediaDelegate &&[m_StreamMediaDelegate respondsToSelector:@selector(INetReceiveUserLogin:strName:useraudioID:)]) {
        dispatch_async(dispatch_get_main_queue(), ^{
            
        [m_StreamMediaDelegate INetReceiveUserLogin:uPeerUserID strName:strName useraudioID:userAudioID];
        });
    }
    
}

void IStreamMedia::INetReceiveUserLogOut(unsigned long uPeerUserID)
{
    
    if (m_StreamMediaDelegate &&[m_StreamMediaDelegate respondsToSelector:@selector(INetReceiveUserLogOut:)]) {
        dispatch_async(dispatch_get_main_queue(), ^{
            
        [m_StreamMediaDelegate INetReceiveUserLogOut:uPeerUserID ];
        });
        
    }
   
}

void IStreamMedia::INetReceiveData(unsigned long uPeerUserID, std::string strName, std::string strData, unsigned long nLen)
{
    
    if (m_StreamMediaDelegate &&[m_StreamMediaDelegate respondsToSelector:@selector(INetReceiveData:strName:strData:nLen:)]) {
        dispatch_async(dispatch_get_main_queue(), ^{
            
        [m_StreamMediaDelegate INetReceiveData:uPeerUserID strName:strName.c_str() strData:strData.c_str() nLen:nLen];
        });
    }
    
    
}
void IStreamMedia::INetBroadcastData(unsigned long uPeerUserID, const char* pData, unsigned long nLen)
{
   
    
    if (m_StreamMediaDelegate &&[m_StreamMediaDelegate respondsToSelector:@selector(INetBroadcastData:pData:nLen:)]) {
        
        [m_StreamMediaDelegate INetBroadcastData:uPeerUserID pData:pData  nLen:nLen];
        
    }
}

bool checkStreamMediaMicAudio()
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

bool checkStreamMediaMicVideo()
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


void  setMtgNetWorkDelegate( id<StreamMediaDelegate> delegate)
{
    m_StreamMediaDelegate = delegate;
}

void loginMtgServer(const char* szServerHostURL,const char* szUserName, const char* headUrl,
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
    
     if(!checkStreamMediaMicAudio())
            return;
        
        
        if(!checkStreamMediaMicVideo())
            return;
    
    
   if(m_CStreamMedia)
    {
      logoutMtgServer();
      usleep(1000000);
   }
  
    m_CStreamMedia = new CStreamMedia();
    
    if (m_IStreamMedia) {
        delete m_IStreamMedia;
        m_IStreamMedia =NULL;
    }
    
    m_IStreamMedia = new IStreamMedia;
    
    m_CStreamMedia->setNetWorkCallback(m_IStreamMedia);
    
    if(m_CStreamMedia)
    {
      m_CStreamMedia->LoginServer(ip, szUserName,headUrl,nickName,latitude, longitude,"StreamMedia","StreamMedia",gameid,"StreamMedia","NULL",isencrypt);
    }
}

void  openMtgVideoWtihName(const char* username ,int x,int y,int width,int height,bool round)
{
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        
        if(m_CStreamMedia)
        {
            m_CStreamMedia->OpenRemoteUserVideo(username,x,y,width,height,round);
            
        }
    });
}

void  setMtgRemoteUserVideoFrame(const char* username ,int x,int y,int width,int height,bool round)
{
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        
        if(m_CStreamMedia)
        {
            m_CStreamMedia->setRemoteUserVideoFrame(username,x,y,width,height,round);
            
        }
    });
}

void  closeMtgVideoWtihName(const char *name)
{
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
    
        if(m_CStreamMedia)
        {
            m_CStreamMedia->closeRemoteUserVideo(name);
            
        }
    });
}

void openMtgRemoteUserAudio(const char* username)
{
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        
        if(m_CStreamMedia)
        {
            m_CStreamMedia->OpenRemoteUserAudio(username);
            
        }
    });
}

void closeMtgRemoteUserAudio(const char* username)
{
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        
        if(m_CStreamMedia)
        {
            m_CStreamMedia->closeRemoteUserAudio(username);
            
        }
    });
}

void  openMtgLocalVideo()
{
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        
        if(m_CStreamMedia)
        {
            m_CStreamMedia->openLocalVideo();
            
        }
    });
}

void  closeMtgLocalVideo()
{
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        
        if(m_CStreamMedia)
        {
            m_CStreamMedia->closeLocalVideo();
            
        }
    });
}

void  mtgInviteUser(const char* mtgId, const char* iuserId)
{
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        
        if(m_CStreamMedia)
        {
            m_CStreamMedia->mtgInviteUser(mtgId,iuserId);
            
        }
    });
}

void  setLocalCamera(bool isFront)
{
    if(m_CStreamMedia)
    {
        m_CStreamMedia->setLocalCamera(isFront);
        
    }
}
void  resetPreview()
{
    if(m_CStreamMedia)
    {
        m_CStreamMedia->resetPreview();
        
    }
}

void  logoutMtgServer()
{
    if(m_CStreamMedia)
    {
        m_CStreamMedia->logoutServer();
 
    }
}

 void setAudioSession()
{
    if(m_CStreamMedia)
    {
        m_CStreamMedia->setAudioSession();
        
    }
}
 
void creatMtg(std::string mtgid, bool openCamera)
{
    if(m_CStreamMedia)
    {
        m_CStreamMedia->creatMtg(mtgid,openCamera,MTG_Common);
    }
}

void joinMtg(std::string mtgid, bool openCamera)
{
    if(m_CStreamMedia)
    {
        m_CStreamMedia->joinMtg(mtgid,openCamera);
        
    }
}

void exitMtg()
{
    if(m_CStreamMedia)
    {
        m_CStreamMedia->exitMtg();
        
    }
}

void destroyMtg()
{
    if(m_CStreamMedia)
    {
        m_CStreamMedia->destroyMtg();
        
    }
}


void openLocalAudio()
{
if(m_CStreamMedia)
{
    m_CStreamMedia->openLocalAudio();
    
}
}

void closeLocalAudio()
{
    if(m_CStreamMedia)
    {
        m_CStreamMedia->closeLocalAudio();
        
    }
}

void  setbeautifyFace(bool beautifyFace)
{

    if(m_CStreamMedia)
    {
        m_CStreamMedia->setbeautifyFace(beautifyFace);
        
    }
}

