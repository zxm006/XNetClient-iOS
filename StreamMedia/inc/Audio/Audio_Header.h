//Audio_Header.h
#ifndef __Audio_HEADER_H__
#define __Audio_HEADER_H__


#ifdef _WIN32
#include <winsock2.h>
#define __INLINE__	_inline
#else
#define __INLINE__	inline

#ifndef _WIN32_WCE
#include <netinet/in.h>
#endif

#endif





#define Audio_HEADER_VER_01					0x01
#define Audio_HEADER_VER_01_LEN				12


typedef struct tagAudio_HEADER
{
	unsigned short ver:2;
	unsigned short tcp:1;
	unsigned short dual:1;	
	unsigned short fec:1;
	unsigned short duration:3;	//10ms
	unsigned short codecid:8;

	unsigned short seq;
	unsigned int   ts;
	unsigned int   istrp;
}Audio_HEADER,*PAudio_HEADER;

__INLINE__ void Audio_HEADER_RESET(void*p)
{
	PAudio_HEADER pHeader=(PAudio_HEADER)p;
	pHeader->ver=Audio_HEADER_VER_01;
	pHeader->tcp=0;
	pHeader->dual=0;
	pHeader->fec=0;
	pHeader->codecid=0;
	pHeader->duration=0;
	pHeader->seq=0;
	pHeader->ts=0;
	pHeader->istrp = 0;
}

__INLINE__ unsigned short Audio_HEADER_IS_VALID(const void*p)
{
	PAudio_HEADER pHeader=(PAudio_HEADER)p;
	if (pHeader->ver==Audio_HEADER_VER_01)
	{
		return 1;
	}
	return 0;
}

__INLINE__ unsigned short Audio_HEADER_GET_TCP(const void*p)
{
	PAudio_HEADER pHeader=(PAudio_HEADER)p;
	return pHeader->tcp;
}

__INLINE__ void Audio_HEADER_SET_TCP(void*p,unsigned short v)
{
	PAudio_HEADER pHeader=(PAudio_HEADER)p;
	pHeader->tcp=v;
}

__INLINE__ unsigned short Audio_HEADER_GET_DUAL(const void*p)
{
	PAudio_HEADER pHeader=(PAudio_HEADER)p;
	return pHeader->dual;
}

__INLINE__ void Audio_HEADER_SET_DUAL(void*p,unsigned short v)
{
	PAudio_HEADER pHeader=(PAudio_HEADER)p;
	pHeader->dual=v;
}

__INLINE__ unsigned short Audio_HEADER_GET_FEC(const void*p)
{
	PAudio_HEADER pHeader=(PAudio_HEADER)p;
	return pHeader->fec;
}

__INLINE__ void Audio_HEADER_SET_FEC(void*p,unsigned short v)
{
	PAudio_HEADER pHeader=(PAudio_HEADER)p;
	pHeader->fec=v;
}

__INLINE__ unsigned short Audio_HEADER_GET_CODEC_ID(const void*p)
{
	PAudio_HEADER pHeader=(PAudio_HEADER)p;
	return pHeader->codecid;
}

__INLINE__ void Audio_HEADER_SET_CODEC_ID(void*p,unsigned short v)
{
	PAudio_HEADER pHeader=(PAudio_HEADER)p;
	pHeader->codecid=v;
}


__INLINE__ unsigned short Audio_HEADER_GET_DURATION(const void*p)
{
	PAudio_HEADER pHeader=(PAudio_HEADER)p;
	return pHeader->duration;
}

__INLINE__ void Audio_HEADER_SET_DURATION(void*p,unsigned short v)
{
	PAudio_HEADER pHeader=(PAudio_HEADER)p;
	pHeader->duration=v;
}

__INLINE__ unsigned short Audio_HEADER_GET_SEQUENCE(const void*p)
{
	PAudio_HEADER pHeader=(PAudio_HEADER)p;
	return ntohs(pHeader->seq);
}

__INLINE__ void Audio_HEADER_SET_SEQUENCE(void*p,unsigned short v)
{
	PAudio_HEADER pHeader=(PAudio_HEADER)p;
	pHeader->seq=htons(v);
}

__INLINE__ unsigned int Audio_HEADER_GET_TIMESTAMP(const void*p)
{
	PAudio_HEADER pHeader=(PAudio_HEADER)p;
	return ntohl(pHeader->ts);
}

__INLINE__ void Audio_HEADER_SET_TIMESTAMP(void*p,unsigned int v)
{
	PAudio_HEADER pHeader=(PAudio_HEADER)p;
	pHeader->ts=htonl(v);
}



__INLINE__ unsigned int Audio_HEADER_GET_ISRTP(const void*p)
{
	PAudio_HEADER pHeader=(PAudio_HEADER)p;
	return ntohl(pHeader->istrp);
}

__INLINE__ void Audio_HEADER_SET_ISRTP(void*p,unsigned int v)
{
	PAudio_HEADER pHeader=(PAudio_HEADER)p;
	pHeader->istrp=htonl(v);
}

__INLINE__ unsigned short Audio_HEADER_GET_LEN(const void*p)
{
	return Audio_HEADER_VER_01_LEN;
}

#endif