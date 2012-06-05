

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Fri Feb 27 15:12:56 2009
 */
/* Compiler settings for .\mtlThumbnailViewer.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_IComponentRegistrar,0xa817e7a2,0x43fa,0x11d0,0x9e,0x44,0x00,0xaa,0x00,0xb6,0x77,0x0a);


MIDL_DEFINE_GUID(IID, IID_ImtlThumbnailViewerCOM,0xAF0253A9,0x0281,0x4638,0x8B,0xDC,0x3C,0x8F,0x85,0x0B,0xC6,0x77);


MIDL_DEFINE_GUID(IID, LIBID_mtlThumbnailViewerLib,0x700D73A8,0x2F74,0x496B,0xB2,0xDA,0x55,0xBF,0xDE,0x96,0xE2,0xBE);


MIDL_DEFINE_GUID(CLSID, CLSID_CompReg,0x78B61071,0x0490,0x45AF,0xAC,0x14,0x29,0x7B,0xF4,0x31,0xE5,0x38);


MIDL_DEFINE_GUID(CLSID, CLSID_mtlThumbnailViewerCOM,0x2BB80AFB,0x3520,0x465C,0x9D,0xED,0x85,0xE5,0x18,0xCD,0xB2,0xD6);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



