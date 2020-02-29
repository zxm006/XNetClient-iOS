//
//  uuDefine.c
//  StreamMedia
//
//  Created by zhangxinming on 2017/10/24.
//  Copyright © 2017年 zhangxinming. All rights reserved.
//

#include "uuDefine.h"
#import "MicBlow.h"
#import<Foundation/Foundation.h>

void startMicBlow(id<MicBlowDelegate> del)
{
    [[  MicBlow shareHandle]setDelegate:del];

    [[  MicBlow shareHandle]start];
}
 


void stopMicBlow()
{
    [[  MicBlow shareHandle]stop];
}
