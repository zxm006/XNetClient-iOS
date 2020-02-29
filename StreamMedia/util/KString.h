#ifndef __KSTRING_H__
#define __KSTRING_H__

#include <string>
#include "NETEC/NETEC_Export.h"
#include <algorithm>

//---------------------------------------------------------------------------------------
class NETEC_API KString
{
public:
    KString(void);
	KString(const std::string& str);
	virtual ~KString(void);
	KString& operator=(KString& xstr);
	KString& operator=(const std::string& str);

	virtual std::string AsString(void);
	virtual unsigned long AsUnsignedLong(void);
	virtual unsigned int AsUnsignedInt(void);
	virtual unsigned short AsUnsignedShort(void);
	virtual unsigned char AsUnsignedChar(void);
	virtual bool AsBoolean(void);

protected:
	std::string m_string;
};

//---------------------------------------------------------------------------------------
class NETEC_API KBitString
{
public:
    KBitString(void);
    KBitString(const char* cszPrivilegeString);
	virtual ~KBitString(void);

    virtual void SetPrivilege(int nBit,bool bEnable);
    virtual bool GetPrivilege(int nBit);
    virtual const char* GetString(void);

protected:
    char m_szPrivilegeString[256];
};

#endif
