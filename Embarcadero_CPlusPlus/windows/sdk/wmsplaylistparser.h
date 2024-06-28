#pragma option push -b -a8 -pc -A- /*P_O_Push*/


/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0499 */
/* Compiler settings for wmsplaylistparser.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 500
#endif

/* verify that the <rpcsal.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCSAL_H_VERSION__
#define __REQUIRED_RPCSAL_H_VERSION__ 100
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

#ifndef __wmsplaylistparser_h__
#define __wmsplaylistparser_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IWMSPlaylistParser_FWD_DEFINED__
#define __IWMSPlaylistParser_FWD_DEFINED__
typedef interface IWMSPlaylistParser IWMSPlaylistParser;
#endif 	/* __IWMSPlaylistParser_FWD_DEFINED__ */


#ifndef __IWMSPlaylistParserCallback_FWD_DEFINED__
#define __IWMSPlaylistParserCallback_FWD_DEFINED__
typedef interface IWMSPlaylistParserCallback IWMSPlaylistParserCallback;
#endif 	/* __IWMSPlaylistParserCallback_FWD_DEFINED__ */


#ifndef __IWMSPlaylistParserPlugin_FWD_DEFINED__
#define __IWMSPlaylistParserPlugin_FWD_DEFINED__
typedef interface IWMSPlaylistParserPlugin IWMSPlaylistParserPlugin;
#endif 	/* __IWMSPlaylistParserPlugin_FWD_DEFINED__ */


#ifndef __IWMSPlaylistParserPluginCallback_FWD_DEFINED__
#define __IWMSPlaylistParserPluginCallback_FWD_DEFINED__
typedef interface IWMSPlaylistParserPluginCallback IWMSPlaylistParserPluginCallback;
#endif 	/* __IWMSPlaylistParserPluginCallback_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "xmldom.h"
#include "nsscore.h"
#include "wmsbuffer.h"
#include "dataContainer.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_wmsplaylistparser_0000_0000 */
/* [local] */ 

//*****************************************************************************
//
//  Microsoft Windows Media
//  Copyright (C) Microsoft Corporation. All rights reserved.
//
//  Automatically generated by Midl from WMSPlaylistParser.idl
//
//  DO NOT EDIT THIS FILE.
//
//*****************************************************************************


EXTERN_GUID( IID_IWMSPlaylistParser, 0xee1f2ec, 0x48ef, 0x11d2, 0x9e, 0xff, 0x0, 0x60, 0x97, 0xd2, 0xd7, 0xcf );
EXTERN_GUID( IID_IWMSPlaylistParserCallback, 0xee1f2ed, 0x48ef, 0x11d2, 0x9e, 0xff, 0x0, 0x60, 0x97, 0xd2, 0xd7, 0xcf );
EXTERN_GUID( IID_IWMSPlaylistParserPlugin, 0xfa8764c1, 0x90a2, 0x11d2, 0x9f, 0x22, 0x0, 0x60, 0x97, 0xd2, 0xd7, 0xcf );
EXTERN_GUID( IID_IWMSPlaylistParserPluginCallback, 0xfa8764c2, 0x90a2, 0x11d2, 0x9f, 0x22, 0x0, 0x60, 0x97, 0xd2, 0xd7, 0xcf );


extern RPC_IF_HANDLE __MIDL_itf_wmsplaylistparser_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_wmsplaylistparser_0000_0000_v0_0_s_ifspec;

#ifndef __IWMSPlaylistParser_INTERFACE_DEFINED__
#define __IWMSPlaylistParser_INTERFACE_DEFINED__

/* interface IWMSPlaylistParser */
/* [helpstring][version][uuid][unique][object] */ 


EXTERN_C const IID IID_IWMSPlaylistParser;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0EE1F2EC-48EF-11d2-9EFF-006097D2D7CF")
    IWMSPlaylistParser : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ReadPlaylist( 
            /* [in] */ __RPC__in_opt INSSBuffer *pBuffer,
            /* [in] */ __RPC__in_opt IXMLDOMDocument *pPlaylist,
            /* [in] */ __RPC__in_opt IWMSPlaylistParserCallback *pCallback,
            /* [in] */ QWORD qwContext) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE WritePlaylist( 
            /* [in] */ __RPC__in_opt IXMLDOMDocument *pPlaylist,
            /* [in] */ __RPC__in_opt IWMSPlaylistParserCallback *pCallback,
            /* [in] */ QWORD qwContext) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ReadPlaylistFromDirectory( 
            /* [in] */ __RPC__in_opt IWMSDirectory *pDirectory,
            /* [in] */ __RPC__in LPWSTR pszwFilePattern,
            /* [in] */ __RPC__in_opt IXMLDOMDocument *pPlaylist,
            /* [in] */ __RPC__in_opt IWMSPlaylistParserCallback *pCallback,
            /* [in] */ QWORD qwContext) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWMSPlaylistParserVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWMSPlaylistParser * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWMSPlaylistParser * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWMSPlaylistParser * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ReadPlaylist )( 
            IWMSPlaylistParser * This,
            /* [in] */ __RPC__in_opt INSSBuffer *pBuffer,
            /* [in] */ __RPC__in_opt IXMLDOMDocument *pPlaylist,
            /* [in] */ __RPC__in_opt IWMSPlaylistParserCallback *pCallback,
            /* [in] */ QWORD qwContext);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *WritePlaylist )( 
            IWMSPlaylistParser * This,
            /* [in] */ __RPC__in_opt IXMLDOMDocument *pPlaylist,
            /* [in] */ __RPC__in_opt IWMSPlaylistParserCallback *pCallback,
            /* [in] */ QWORD qwContext);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *ReadPlaylistFromDirectory )( 
            IWMSPlaylistParser * This,
            /* [in] */ __RPC__in_opt IWMSDirectory *pDirectory,
            /* [in] */ __RPC__in LPWSTR pszwFilePattern,
            /* [in] */ __RPC__in_opt IXMLDOMDocument *pPlaylist,
            /* [in] */ __RPC__in_opt IWMSPlaylistParserCallback *pCallback,
            /* [in] */ QWORD qwContext);
        
        END_INTERFACE
    } IWMSPlaylistParserVtbl;

    interface IWMSPlaylistParser
    {
        CONST_VTBL struct IWMSPlaylistParserVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWMSPlaylistParser_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWMSPlaylistParser_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWMSPlaylistParser_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWMSPlaylistParser_ReadPlaylist(This,pBuffer,pPlaylist,pCallback,qwContext)	\
    ( (This)->lpVtbl -> ReadPlaylist(This,pBuffer,pPlaylist,pCallback,qwContext) ) 

#define IWMSPlaylistParser_WritePlaylist(This,pPlaylist,pCallback,qwContext)	\
    ( (This)->lpVtbl -> WritePlaylist(This,pPlaylist,pCallback,qwContext) ) 

#define IWMSPlaylistParser_ReadPlaylistFromDirectory(This,pDirectory,pszwFilePattern,pPlaylist,pCallback,qwContext)	\
    ( (This)->lpVtbl -> ReadPlaylistFromDirectory(This,pDirectory,pszwFilePattern,pPlaylist,pCallback,qwContext) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWMSPlaylistParser_INTERFACE_DEFINED__ */


#ifndef __IWMSPlaylistParserCallback_INTERFACE_DEFINED__
#define __IWMSPlaylistParserCallback_INTERFACE_DEFINED__

/* interface IWMSPlaylistParserCallback */
/* [helpstring][version][uuid][unique][object] */ 


EXTERN_C const IID IID_IWMSPlaylistParserCallback;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0EE1F2ED-48EF-11d2-9EFF-006097D2D7CF")
    IWMSPlaylistParserCallback : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE OnReadPlaylist( 
            /* [in] */ HRESULT hr,
            /* [in] */ QWORD qwContext) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE OnWritePlaylist( 
            /* [in] */ HRESULT hr,
            /* [in] */ __RPC__in_opt INSSBuffer *pBuffer,
            /* [in] */ QWORD qwContext) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWMSPlaylistParserCallbackVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWMSPlaylistParserCallback * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWMSPlaylistParserCallback * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWMSPlaylistParserCallback * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *OnReadPlaylist )( 
            IWMSPlaylistParserCallback * This,
            /* [in] */ HRESULT hr,
            /* [in] */ QWORD qwContext);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *OnWritePlaylist )( 
            IWMSPlaylistParserCallback * This,
            /* [in] */ HRESULT hr,
            /* [in] */ __RPC__in_opt INSSBuffer *pBuffer,
            /* [in] */ QWORD qwContext);
        
        END_INTERFACE
    } IWMSPlaylistParserCallbackVtbl;

    interface IWMSPlaylistParserCallback
    {
        CONST_VTBL struct IWMSPlaylistParserCallbackVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWMSPlaylistParserCallback_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWMSPlaylistParserCallback_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWMSPlaylistParserCallback_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWMSPlaylistParserCallback_OnReadPlaylist(This,hr,qwContext)	\
    ( (This)->lpVtbl -> OnReadPlaylist(This,hr,qwContext) ) 

#define IWMSPlaylistParserCallback_OnWritePlaylist(This,hr,pBuffer,qwContext)	\
    ( (This)->lpVtbl -> OnWritePlaylist(This,hr,pBuffer,qwContext) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWMSPlaylistParserCallback_INTERFACE_DEFINED__ */


#ifndef __IWMSPlaylistParserPlugin_INTERFACE_DEFINED__
#define __IWMSPlaylistParserPlugin_INTERFACE_DEFINED__

/* interface IWMSPlaylistParserPlugin */
/* [helpstring][version][uuid][unique][object] */ 


EXTERN_C const IID IID_IWMSPlaylistParserPlugin;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("FA8764C1-90A2-11d2-9F22-006097D2D7CF")
    IWMSPlaylistParserPlugin : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE CreatePlaylistParser( 
            /* [in] */ __RPC__in_opt IWMSCommandContext *pCommandContext,
            /* [in] */ __RPC__in_opt IWMSContext *pUser,
            /* [in] */ __RPC__in_opt IWMSContext *pPresentation,
            /* [in] */ DWORD dwFlags,
            /* [in] */ __RPC__in_opt IWMSClassObject *pFactory,
            /* [in] */ __RPC__in_opt IWMSBufferAllocator *pBufferAllocator,
            /* [in] */ __RPC__in_opt IWMSPlaylistParserPluginCallback *pCallback,
            /* [in] */ QWORD qwContext) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWMSPlaylistParserPluginVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWMSPlaylistParserPlugin * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWMSPlaylistParserPlugin * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWMSPlaylistParserPlugin * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *CreatePlaylistParser )( 
            IWMSPlaylistParserPlugin * This,
            /* [in] */ __RPC__in_opt IWMSCommandContext *pCommandContext,
            /* [in] */ __RPC__in_opt IWMSContext *pUser,
            /* [in] */ __RPC__in_opt IWMSContext *pPresentation,
            /* [in] */ DWORD dwFlags,
            /* [in] */ __RPC__in_opt IWMSClassObject *pFactory,
            /* [in] */ __RPC__in_opt IWMSBufferAllocator *pBufferAllocator,
            /* [in] */ __RPC__in_opt IWMSPlaylistParserPluginCallback *pCallback,
            /* [in] */ QWORD qwContext);
        
        END_INTERFACE
    } IWMSPlaylistParserPluginVtbl;

    interface IWMSPlaylistParserPlugin
    {
        CONST_VTBL struct IWMSPlaylistParserPluginVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWMSPlaylistParserPlugin_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWMSPlaylistParserPlugin_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWMSPlaylistParserPlugin_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWMSPlaylistParserPlugin_CreatePlaylistParser(This,pCommandContext,pUser,pPresentation,dwFlags,pFactory,pBufferAllocator,pCallback,qwContext)	\
    ( (This)->lpVtbl -> CreatePlaylistParser(This,pCommandContext,pUser,pPresentation,dwFlags,pFactory,pBufferAllocator,pCallback,qwContext) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWMSPlaylistParserPlugin_INTERFACE_DEFINED__ */


#ifndef __IWMSPlaylistParserPluginCallback_INTERFACE_DEFINED__
#define __IWMSPlaylistParserPluginCallback_INTERFACE_DEFINED__

/* interface IWMSPlaylistParserPluginCallback */
/* [helpstring][version][uuid][unique][object] */ 


EXTERN_C const IID IID_IWMSPlaylistParserPluginCallback;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("FA8764C2-90A2-11d2-9F22-006097D2D7CF")
    IWMSPlaylistParserPluginCallback : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE OnCreatePlaylistParser( 
            /* [in] */ HRESULT hr,
            /* [in] */ __RPC__in_opt IWMSPlaylistParser *pParser,
            /* [in] */ QWORD qwContext) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWMSPlaylistParserPluginCallbackVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWMSPlaylistParserPluginCallback * This,
            /* [in] */ __RPC__in REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWMSPlaylistParserPluginCallback * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWMSPlaylistParserPluginCallback * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *OnCreatePlaylistParser )( 
            IWMSPlaylistParserPluginCallback * This,
            /* [in] */ HRESULT hr,
            /* [in] */ __RPC__in_opt IWMSPlaylistParser *pParser,
            /* [in] */ QWORD qwContext);
        
        END_INTERFACE
    } IWMSPlaylistParserPluginCallbackVtbl;

    interface IWMSPlaylistParserPluginCallback
    {
        CONST_VTBL struct IWMSPlaylistParserPluginCallbackVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWMSPlaylistParserPluginCallback_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IWMSPlaylistParserPluginCallback_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IWMSPlaylistParserPluginCallback_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IWMSPlaylistParserPluginCallback_OnCreatePlaylistParser(This,hr,pParser,qwContext)	\
    ( (This)->lpVtbl -> OnCreatePlaylistParser(This,hr,pParser,qwContext) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IWMSPlaylistParserPluginCallback_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif




#pragma option pop /*P_O_Pop*/