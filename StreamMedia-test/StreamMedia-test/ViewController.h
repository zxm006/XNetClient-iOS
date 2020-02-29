//
//  ViewController.h
//  client-XNet
//
//  Created by zhangxinming on 16/6/14.
//  Copyright © 2016年 zhangxinming. All rights reserved.
//

#import <UIKit/UIKit.h>
#include <string>
#include <map>



@interface UserInfo: NSObject
{
    
}
@property(nonatomic,strong)NSString *strUserName;
@property(nonatomic,assign)unsigned long ulUserId;
@property(nonatomic,assign)unsigned long ulUserAudioID;



@end



@interface ViewController : UIViewController 

{}
@property(nonatomic,assign)IBOutlet UIView *mlocalview;
@property(nonatomic,assign)IBOutlet UIImageView *imageview;
@end

