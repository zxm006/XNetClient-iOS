//
//  AiYouStreamMediaMgr.h
//  MultiLinkMicTalk
//
//  Created by zhangxinming on 17/7/11.
//  Copyright © 2016年 zhangxinming. All rights reserved.
//

#ifndef AiYouStreamMediaMgr_h
#define AiYouStreamMediaMgr_h
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#include "uuDefine.h"



#import <Foundation/Foundation.h>

#ifdef __cplusplus
extern "C" {
#endif
    //登录状态回调
    void  setVideoChatDelegate( id<VideoChatDelegate> delegate);
    
    //登录服务器
    void loginVideoChatServer(const char* szServerHostURL,const char* szUserName, const char* headUrl,const char* nickName,double latitude,double longitude,const char* gameid,bool isencrypt);
    void  logoutVideoChatServer();
    
   void joinVideoChat(std::string mtgid, bool openCamera);
    void exitVideoChat();
    void destroyVideoChat();
    
    void setVideoChatSession();
    void VideoChatInviteUser(const char* mtgId, const char* iuserId);
    
    void openVideoChatVideoWtihName(const char* username ,int x,int y,int width,int height,bool round);
    void closeVideoChatVideoWtihName(const char *name);
   
    void setVideoChatRemoteUserVideoFrame(const char* username ,int x,int y,int width,int height,bool round);
    
    void openVideoChatLocalVideo();
    void closeVideoChatLocalVideo();
    
    void openVideoChatRemoteUserAudio(const char* username);
    void closeVideoChatRemoteUserAudio(const char* username);
    
    void switchVideoChatCamera();
    void  resetVideoChatOrientation();
    
    void  openVideoChatBeautify();
    void  closeVideoChatBeautify();
    
    void openVideoChatLocalAudio();
    void closeVideoChatLocalAudio();
    
    //设置美颜，默认开启
    void  setVideoChatBeautifyFace(bool beautifyFace);
    void videoChatSnapshot();
#ifdef __cplusplus
}
#endif

#endif /* AiYouStreamMediaMgr_h */
