#include "KMutex.h"

#ifndef WIN32 


KMutex::KMutex() {
    pthread_mutex_init(&m_mutex, NULL);
}
KMutex::~KMutex() {
    pthread_mutex_destroy(&m_mutex);
}
void KMutex::Lock() {
    pthread_mutex_lock(&m_mutex); 
}
void  KMutex::Unlock() {
    pthread_mutex_unlock(&m_mutex);
}
#endif
