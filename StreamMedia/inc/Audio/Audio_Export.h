#ifndef __Audio_EXPORT_H__
#define __Audio_EXPORT_H__

#ifdef Audio_EXPORT
#define Audio_ENGINE_API _declspec(dllexport)
#elif Audio_DLL
#define Audio_ENGINE_API _declspec(dllimport)
#else
#define Audio_ENGINE_API
#endif

#endif


