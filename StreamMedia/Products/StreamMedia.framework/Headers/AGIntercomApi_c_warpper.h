//
//  AGIntercomApi_c_warpper.h
//  Intercom
//
//  Created by HeXingang on 2017/10/10.
//  Copyright © 2017年 zhangxinming. All rights reserved.
//

#ifndef AGIntercomApi_c_warpper_h
#define AGIntercomApi_c_warpper_h

#import "uuDefine.h"

#ifdef __cplusplus
extern "C" {
#endif
    
    /**
     Unity 初始化方法
     */
    void AG_initIntercomForUnity(const char* unityObject,const char* UnityFunSucces,const char* UnityFunFail);
    
    
    /**
     Unity 回调
     */
    void AG_setIntercomUnityCallback(UNITYSENDMSG unitymsg);
    
    
    /**
     设置回调 (OC)

     @param callback    回调
     @param params      可选参数，回调时会把这个参数带过去
     */
    void AG_setIntercomCallback(AG_SM_OC_CALLBACK callback, id params);
    
    
    /**
     登录实时对讲的服务器

     @param gameId 游戏id
     @param roomId 房间id
     @param groupId 组id
     @param userId 用户id
     @param nickname 昵称
     @param headUrl 头像图片地址
     */
    void AG_loginIntercomServer(const char* gameId, const char* roomId, const char* groupId, const char* userId, const char* nickname, const char* headUrl);
    
    
    /**
     登出实时对讲服务器
     */
    void AG_logoutIntercomServer();
    
    
    /**
     开启实时对讲

     @param isSpeakInRoom   是否发送语音给所有人，false 只发给所在组用户
     */
    void AG_startIntercom(bool isSpeakInRoom);
    
    
    /**
     结束实时对讲
     */
    void AG_stopIntercom();
    
    
    /**
     SDK配置项方法

     @param params 参数集, json格式
     */
    void AG_setIntercomParams(const char* params);
    
    
#ifdef __cplusplus
}
#endif


#endif /* AGIntercomApi_c_warpper_h */
