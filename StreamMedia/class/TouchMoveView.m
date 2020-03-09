#import "TouchMoveView.h"
#import <AVFoundation/AVFoundation.h>
#define WIDTH self.frame.size.width
#define HEIGHT self.frame.size.height
#define kScreenWidth [[UIScreen mainScreen] bounds].size.width
#define kScreenHeight [[UIScreen mainScreen] bounds].size.height

@interface TouchMoveView ()<AVCaptureVideoDataOutputSampleBufferDelegate>
{
    
} 
@end

@implementation TouchMoveView

- (id)init
{
    if(self = [super init]) {
        }
        return self;
}

- (void)dealloc
{
    self.userName =nil;
    [super dealloc];
}

- (void)touchesEnded:(NSSet<UITouch *> *)touches withEvent:(nullable UIEvent *)event
{
      // 当前触摸点
      CGPoint center = self.center;
  
    if ( center.x<WIDTH/2) {
        center.x =WIDTH/2;
    }
    if ( center.y<HEIGHT/2) {
        center.y =HEIGHT/2;
    }
    
    if ( center.x>kScreenWidth-WIDTH/2) {
        center.x =kScreenWidth-WIDTH/2;
    }
    if ( center.y>kScreenHeight-HEIGHT/2) {
        center.y =kScreenHeight-HEIGHT/2;
    }
    DLog(@"kScreenWidth = %f WIDTH = %f",kScreenWidth,WIDTH);
    DLog(@"kScreenHeight = %f HEIGHT = %f",kScreenHeight,HEIGHT);
    // 修改当前view的中点(中点改变view的位置就会改变)
    self.center = center;
}

- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    UITouch *touch = [touches anyObject];
    // 当前触摸点
    CGPoint currentPoint = [touch locationInView:self.superview];
    // 上一个触摸点
    CGPoint previousPoint = [touch previousLocationInView:self.superview];
    // 当前view的中点
    CGPoint center = self.center;
    center.x += (currentPoint.x - previousPoint.x);
    center.y += (currentPoint.y - previousPoint.y);
  // 修改当前view的中点(中点改变view的位置就会改变)
    self.center = center;
    }
@end
