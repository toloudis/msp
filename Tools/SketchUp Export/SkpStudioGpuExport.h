

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


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

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __SkpStudioGpuExport_h__
#define __SkpStudioGpuExport_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __ISkpStudioGpuExporter_FWD_DEFINED__
#define __ISkpStudioGpuExporter_FWD_DEFINED__
typedef interface ISkpStudioGpuExporter ISkpStudioGpuExporter;
#endif 	/* __ISkpStudioGpuExporter_FWD_DEFINED__ */


#ifndef __SkpStudioGpuExporter_FWD_DEFINED__
#define __SkpStudioGpuExporter_FWD_DEFINED__

#ifdef __cplusplus
typedef class SkpStudioGpuExporter SkpStudioGpuExporter;
#else
typedef struct SkpStudioGpuExporter SkpStudioGpuExporter;
#endif /* __cplusplus */

#endif 	/* __SkpStudioGpuExporter_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __ISkpStudioGpuExporter_INTERFACE_DEFINED__
#define __ISkpStudioGpuExporter_INTERFACE_DEFINED__

/* interface ISkpStudioGpuExporter */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_ISkpStudioGpuExporter;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D2AC12A0-BE9B-4141-9E54-08E47A437AB3")
    ISkpStudioGpuExporter : public IUnknown
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct ISkpStudioGpuExporterVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISkpStudioGpuExporter * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISkpStudioGpuExporter * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISkpStudioGpuExporter * This);
        
        END_INTERFACE
    } ISkpStudioGpuExporterVtbl;

    interface ISkpStudioGpuExporter
    {
        CONST_VTBL struct ISkpStudioGpuExporterVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISkpStudioGpuExporter_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISkpStudioGpuExporter_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISkpStudioGpuExporter_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISkpStudioGpuExporter_INTERFACE_DEFINED__ */



#ifndef __SkpStudioGpuExportLib_LIBRARY_DEFINED__
#define __SkpStudioGpuExportLib_LIBRARY_DEFINED__

/* library SkpStudioGpuExportLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_SkpStudioGpuExportLib;

EXTERN_C const CLSID CLSID_SkpStudioGpuExporter;

#ifdef __cplusplus

class DECLSPEC_UUID("6B828FF4-0676-4E35-A367-5DF780DBBCCA")
SkpStudioGpuExporter;
#endif
#endif /* __SkpStudioGpuExportLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


