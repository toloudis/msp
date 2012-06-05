

/* this ALWAYS GENERATED file contains the proxy stub code */


 /* File created by MIDL compiler version 7.00.0500 */
/* at Tue Dec 08 11:56:56 2009
 */
/* Compiler settings for .\SkpStudioGpuExport.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#if !defined(_M_IA64) && !defined(_M_AMD64)


#pragma warning( disable: 4049 )  /* more than 64k source lines */
#if _MSC_VER >= 1200
#pragma warning(push)
#endif

#pragma warning( disable: 4211 )  /* redefine extern to static */
#pragma warning( disable: 4232 )  /* dllimport identity*/
#pragma warning( disable: 4024 )  /* array to pointer mapping*/
#pragma warning( disable: 4152 )  /* function/data pointer conversion in expression */
#pragma warning( disable: 4100 ) /* unreferenced arguments in x86 call */

#pragma optimize("", off ) 

#define USE_STUBLESS_PROXY


/* verify that the <rpcproxy.h> version is high enough to compile this file*/
#ifndef __REDQ_RPCPROXY_H_VERSION__
#define __REQUIRED_RPCPROXY_H_VERSION__ 440
#endif


#include "rpcproxy.h"
#ifndef __RPCPROXY_H_VERSION__
#error this stub requires an updated version of <rpcproxy.h>
#endif // __RPCPROXY_H_VERSION__


#include "SkpStudioGpuExport.h"

#define TYPE_FORMAT_STRING_SIZE   3                                 
#define PROC_FORMAT_STRING_SIZE   1                                 
#define EXPR_FORMAT_STRING_SIZE   1                                 
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   0            

typedef struct _SkpStudioGpuExport_MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } SkpStudioGpuExport_MIDL_TYPE_FORMAT_STRING;

typedef struct _SkpStudioGpuExport_MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } SkpStudioGpuExport_MIDL_PROC_FORMAT_STRING;

typedef struct _SkpStudioGpuExport_MIDL_EXPR_FORMAT_STRING
    {
    long          Pad;
    unsigned char  Format[ EXPR_FORMAT_STRING_SIZE ];
    } SkpStudioGpuExport_MIDL_EXPR_FORMAT_STRING;


static RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};


extern const SkpStudioGpuExport_MIDL_TYPE_FORMAT_STRING SkpStudioGpuExport__MIDL_TypeFormatString;
extern const SkpStudioGpuExport_MIDL_PROC_FORMAT_STRING SkpStudioGpuExport__MIDL_ProcFormatString;
extern const SkpStudioGpuExport_MIDL_EXPR_FORMAT_STRING SkpStudioGpuExport__MIDL_ExprFormatString;


extern const MIDL_STUB_DESC Object_StubDesc;


extern const MIDL_SERVER_INFO ISkpStudioGpuExporter_ServerInfo;
extern const MIDL_STUBLESS_PROXY_INFO ISkpStudioGpuExporter_ProxyInfo;



#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif

#if !(TARGET_IS_NT40_OR_LATER)
#error You need a Windows NT 4.0 or later to run this stub because it uses these features:
#error   -Oif or -Oicf.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will fail with the RPC_X_WRONG_STUB_VERSION error.
#endif


static const SkpStudioGpuExport_MIDL_PROC_FORMAT_STRING SkpStudioGpuExport__MIDL_ProcFormatString =
    {
        0,
        {

			0x0
        }
    };

static const SkpStudioGpuExport_MIDL_TYPE_FORMAT_STRING SkpStudioGpuExport__MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */

			0x0
        }
    };


/* Object interface: IUnknown, ver. 0.0,
   GUID={0x00000000,0x0000,0x0000,{0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x46}} */


/* Object interface: ISkpStudioGpuExporter, ver. 0.0,
   GUID={0xD2AC12A0,0xBE9B,0x4141,{0x9E,0x54,0x08,0xE4,0x7A,0x43,0x7A,0xB3}} */

#pragma code_seg(".orpc")
static const unsigned short ISkpStudioGpuExporter_FormatStringOffsetTable[] =
    {
    0
    };

static const MIDL_STUBLESS_PROXY_INFO ISkpStudioGpuExporter_ProxyInfo =
    {
    &Object_StubDesc,
    SkpStudioGpuExport__MIDL_ProcFormatString.Format,
    &ISkpStudioGpuExporter_FormatStringOffsetTable[-3],
    0,
    0,
    0
    };


static const MIDL_SERVER_INFO ISkpStudioGpuExporter_ServerInfo = 
    {
    &Object_StubDesc,
    0,
    SkpStudioGpuExport__MIDL_ProcFormatString.Format,
    &ISkpStudioGpuExporter_FormatStringOffsetTable[-3],
    0,
    0,
    0,
    0};
CINTERFACE_PROXY_VTABLE(3) _ISkpStudioGpuExporterProxyVtbl = 
{
    0,
    &IID_ISkpStudioGpuExporter,
    IUnknown_QueryInterface_Proxy,
    IUnknown_AddRef_Proxy,
    IUnknown_Release_Proxy
};

const CInterfaceStubVtbl _ISkpStudioGpuExporterStubVtbl =
{
    &IID_ISkpStudioGpuExporter,
    &ISkpStudioGpuExporter_ServerInfo,
    3,
    0, /* pure interpreted */
    CStdStubBuffer_METHODS
};

static const MIDL_STUB_DESC Object_StubDesc = 
    {
    0,
    NdrOleAllocate,
    NdrOleFree,
    0,
    0,
    0,
    0,
    0,
    SkpStudioGpuExport__MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x20000, /* Ndr library version */
    0,
    0x70001f4, /* MIDL Version 7.0.500 */
    0,
    0,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0
    };

const CInterfaceProxyVtbl * _SkpStudioGpuExport_ProxyVtblList[] = 
{
    ( CInterfaceProxyVtbl *) &_ISkpStudioGpuExporterProxyVtbl,
    0
};

const CInterfaceStubVtbl * _SkpStudioGpuExport_StubVtblList[] = 
{
    ( CInterfaceStubVtbl *) &_ISkpStudioGpuExporterStubVtbl,
    0
};

PCInterfaceName const _SkpStudioGpuExport_InterfaceNamesList[] = 
{
    "ISkpStudioGpuExporter",
    0
};


#define _SkpStudioGpuExport_CHECK_IID(n)	IID_GENERIC_CHECK_IID( _SkpStudioGpuExport, pIID, n)

int __stdcall _SkpStudioGpuExport_IID_Lookup( const IID * pIID, int * pIndex )
{
    
    if(!_SkpStudioGpuExport_CHECK_IID(0))
        {
        *pIndex = 0;
        return 1;
        }

    return 0;
}

const ExtendedProxyFileInfo SkpStudioGpuExport_ProxyFileInfo = 
{
    (PCInterfaceProxyVtblList *) & _SkpStudioGpuExport_ProxyVtblList,
    (PCInterfaceStubVtblList *) & _SkpStudioGpuExport_StubVtblList,
    (const PCInterfaceName * ) & _SkpStudioGpuExport_InterfaceNamesList,
    0, // no delegation
    & _SkpStudioGpuExport_IID_Lookup, 
    1,
    2,
    0, /* table of [async_uuid] interfaces */
    0, /* Filler1 */
    0, /* Filler2 */
    0  /* Filler3 */
};
#pragma optimize("", on )
#if _MSC_VER >= 1200
#pragma warning(pop)
#endif


#endif /* !defined(_M_IA64) && !defined(_M_AMD64)*/

