#ifndef __KMUTEX_H__
#define __KMUTEX_H__

#ifndef WIN32
#include <pthread.h>
#endif

class KMutex
{
public:
    KMutex();
    ~KMutex();
    void Lock();
    void Unlock();
private:
#ifndef WIN32
	pthread_mutex_t m_mutex;
#endif
};

#endif