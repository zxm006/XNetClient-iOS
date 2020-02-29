//
//  AGVoicechatApi_c_warpper.h
//  StreamMedia
//
//  Created by HeXingang on 2017/10/12.
//  Copyright © 2017年 zhangxinming. All rights reserved.
//

#ifndef AGVoicechatApi_c_warpper_h
#define AGVoicechatApi_c_warpper_h

#import "uuDefine.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    
    /**
     Unity 初始化方法
     */
    void AG_initVoicechatForUnity(const char* unityObject,const char* UnityFunSucces,const char* UnityFunFail);
    
    /**
     Unity 设置回调方法
     */
    void AG_setVoiceUnityCallback(UNITYSENDMSG unitymsg);
    
    /**
     设置回调方法 （OC）

     @param callback    回调方法
     @param params      可选参数，回调时会把该参数带过去
     */
    void AG_setVoiceCallback(AG_SM_OC_CALLBACK callback, id params);
    
    
    /**
     登录语音聊天服务器
     */
    void AG_loginVoicechat(const char* gameId, const char* userId, const char* nickname, const char* headImageURL);
    
    /**
     登出语音聊天服务器
     */
    void AG_logoutVoicechat();
    
    
    /**
     加入语音聊天室

     @param chatId 聊天室id
     */
    void AG_joinVoicechat(const char* chatId);
    
    
    /**
     退出当前聊天室，进入新的聊天室
     
     @param chatId 聊天室id
     */
    void AG_exitAndJoinVoicechat(const char* chatId);
    
    
    /**
     退出语音聊天室
     */
    void AG_exitVoicechat();
    
    
    /**
     邀请用户进入聊天室
     */
    void AG_inviteVoicechat(const char* chatId, const char* userId);
    
    
    /**
     发送语音消息
     */
    void AG_startVoicechatAudio();
    
    
    /**
     说完了
     */
    void AG_stopVoicechatAudio();
    
    
    /**
     SDK配置项方法
     
     @param params 参数集, json格式
     */
    void AG_setVoicechatParams(const char* params);
    
#ifdef __cplusplus
}
#endif


#endif /* AGVoicechatApi_c_warpper_h */
