//
//  main.m
//  client-XNet
//
//  Created by zhangxinming on 16/6/14.
//  Copyright © 2016年 zhangxinming. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "AppDelegate.h"
void MySignalHandler(int signal)

{
    printf("MySignalHandler %d \n",signal);
           }

#include <signal.h>

//void HandleException(int signo)
//{
//    printf("Lanou's sig is:%d",signo);
//}

int main(int argc, char * argv[]) {
    
//    signal(SIGPIPE,SIG_IGN);
    signal(SIGABRT, MySignalHandler);
    
    signal(SIGILL, MySignalHandler);
    
    signal(SIGSEGV, MySignalHandler);
    
    signal(SIGFPE, MySignalHandler);
    
    signal(SIGBUS, MySignalHandler);
    
    signal(SIGPIPE, MySignalHandler);
    @autoreleasepool {
        return UIApplicationMain(argc, argv, nil, NSStringFromClass([AppDelegate class]));
    }
}
