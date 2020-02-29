//
//  CapScreenEnc.cpp

#include "CapScreenEnc.h"
VideoCapScreenEnc::VideoCapScreenEnc(MediaScreenVideoCallBack* pMediaScreenVideoCallBack)
:m_MediaScreenVideoCallBack(pMediaScreenVideoCallBack),
m_CapScreen(NULL)
{
//    m_CapScreen = [[CapScreen alloc] init];

    
}

VideoCapScreenEnc::~VideoCapScreenEnc()
{
//    if(m_pVideoManagement!=nil)
//    [m_pVideoManagement release];
//    m_CapScreen=nil;
}

bool VideoCapScreenEnc::StartCapture(int resType)
{
    if(m_CapScreen!=nil)
    {
        [m_CapScreen stopCapScreen];
        m_CapScreen=nil;
    }
     m_CapScreen = [[CapScreen alloc] init];
        [m_CapScreen SetCallBack:m_MediaScreenVideoCallBack];
    [m_CapScreen startCapScreen:resType];
    return true;
}
void VideoCapScreenEnc::SetResolution(unsigned int uiWidth, unsigned int uiHeight)
{
    m_uiWidth = uiWidth;
    m_uiHeight = uiHeight;
    if(m_CapScreen!=nil)
        [m_CapScreen SetResolution:uiWidth uiHeight:uiHeight];
}


void VideoCapScreenEnc::StopCapture()
{
    if(m_CapScreen!=nil)
    {
        [m_CapScreen stopCapScreen];
        [m_CapScreen release];
         m_CapScreen=nil;
    }
    
}
