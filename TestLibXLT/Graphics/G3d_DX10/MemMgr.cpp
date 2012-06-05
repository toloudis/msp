//#include "stdafx.h"

//============================================================================
//	MemMgr.cpp - Custom memory allocator with leak id logging and stack backtracing.
//
//	StudioGPU
//	Copyright(C) 2008 - All Rights Reserved
//	Author - John Schwab
//============================================================================


#define _CRT_SECURE_NO_WARNINGS	// Disable unsecured CRT warning
#define _WIN32_WINNT 0x0403 // Required for InitializeCriticalSectionAndSpinCount
#include "MemMgr.h"

#ifdef USE_MMGR
#include <stdio.h>
#include <new>

#ifdef USE_STACKTRACE
	#include <dbghelp.h>
	#pragma comment(lib,"dbghelp.lib")
#endif // USE_STACKTRACE

#ifdef _WIN64
#define _NOMANSLAND_SENTINEL_	0xFDFDFDFDFDFDFDFD
#else
#define _NOMANSLAND_SENTINEL_	0xFDFDFDFD
#endif

static const size_t s_nHeadSentinalFill = _NOMANSLAND_SENTINEL_;
static const size_t s_nTailSentinalFill = _NOMANSLAND_SENTINEL_;

LPRtlCaptureContext MemMgr::ms_pfnRtlCaptureContext = NULL;

MemMgr* MemMgr::ms_pInstance = NULL;

//============================================================================

static void Log( FILE* pFile, const char* psz )
{
	OutputDebugStringA( psz );
	if( pFile )
	{
		fwrite( psz, 1, strlen( psz ), pFile);
	}
}

static void Format( FILE* pFile, const char* szFormat, ... )
{
	char szBuff[1024];
	va_list arg;
	va_start( arg, szFormat );
	_vsnprintf_s( szBuff, sizeof( szBuff ), szFormat, arg );
	va_end( arg );
	Log( pFile, szBuff );
}

inline static void Assert( bool bExpression, const char* szMsg )
{
	if( !bExpression )
	{
		Log( NULL, szMsg );
		__debugbreak();
	}
};

//============================================================================

// Ensure the memory manager is constructed extremely early on
#pragma warning( disable:4074 ) // warning C4074: initializers put in compiler reserved initialization area
#pragma init_seg( compiler )

struct MemoryInitialiser
{
	MemoryInitialiser() { MemMgr::Create(); }
	~MemoryInitialiser() { MemMgr::Destroy(); }
} g_theMemoryInitialiser;

//============================================================================

#ifdef _DEBUG
void* __CRTDECL operator new( size_t nSize,
							 int nBlockUse,
							 const char * szFileName,
							 int nLine
							 )
{
	void* pMemory = MemMgr::Get().Allocate( nSize, MemMgr::ALLOC_New );
	Assert( !!pMemory, "Memory allocator failed!" );
	return pMemory;
}

void *__CRTDECL operator new[]( size_t nSize,
							   int nBlockUse,
							   const char * szFileName,
							   int nLine
							   )
{
	void* pMemory = MemMgr::Get().Allocate( nSize, MemMgr::ALLOC_NewArray );
	Assert( !!pMemory, "Memory allocator failed!" );
	return pMemory;
}

#endif
void* _cdecl operator new( size_t nSize )
{
	void* pMemory = MemMgr::Get().Allocate( nSize, MemMgr::ALLOC_New );
	Assert( !!pMemory, "Memory allocator failed!" );
	return pMemory;
}

void* _cdecl operator new[]( size_t nSize )
{
	void* pMemory = MemMgr::Get().Allocate( nSize, MemMgr::ALLOC_NewArray );
	Assert( !!pMemory, "Memory allocator failed!" );
	return pMemory;
}

void _cdecl operator delete( void* pAddress )
{
	MemMgr::Get().Deallocate( pAddress, MemMgr::ALLOC_New );
}

void _cdecl operator delete[]( void* pAddress )
{
	MemMgr::Get().Deallocate( pAddress, MemMgr::ALLOC_NewArray );
}

//============================================================================

MemMgr::MemMgr() :
	m_pHead( NULL ),
	m_pTail( NULL ),
	m_nAllocations( 0 ),
	m_nAllocationsSinceMarker( 0 ),
	m_nAllocatedBytes( 0 ),
	m_nNextID( 1 ),
	m_nBreakOnAllocID( 0 ),
	m_nHighWatermark( 0 ),
	m_nAllocationTag( 0 )
{
	InitializeCriticalSectionAndSpinCount( &m_theMutex, 4096 | 0x80000000 );
#ifdef USE_STACKTRACE
	SymInitialize( GetCurrentProcess(), NULL, TRUE );
	// See if we can get RtlCaptureContext
	ms_pfnRtlCaptureContext = (LPRtlCaptureContext)GetProcAddress( GetModuleHandleW( L"kernel32.dll" ), "RtlCaptureContext" );
#endif
}

MemMgr::~MemMgr()
{
	Format( NULL, "MEMORY  : High watermark was %d bytes (%dKB, %dMB)\n", m_nHighWatermark,
		m_nHighWatermark/1024, m_nHighWatermark/(1024*1024) );
	if( m_nAllocations != 0 )
	{
		Log( NULL, "MEMORY  : Memory leaks detected!\n" );
		DumpAllAllocations( true );
	}
	else
		Log( NULL, "MEMORY  : No memory leaks detected\n" );

	DeleteCriticalSection( &m_theMutex );
	Assert( m_nAllocations == 0, "Memory leaks were found\n" );
}

void MemMgr::Create()
{
	// Allocate space for memory manager and use placement new to construct
	void* pBuff = malloc( sizeof( MemMgr ) );
	MemMgr* pMemory = new( pBuff ) MemMgr();
	ms_pInstance = pMemory;
}

void MemMgr::Destroy()
{
	if( ms_pInstance )
	{
		// Manually call destructor and free memory
		ms_pInstance->~MemMgr();
		free( ms_pInstance );
		ms_pInstance = 0;
	}
}

//============================================================================

void MemMgr::DumpAllAllocations( bool bDumpToFile )
{
	FILE* pFile = bDumpToFile ? fopen( "memleaks.log", "w" ) : NULL;

	EnterCriticalSection( &m_theMutex );
	Format( pFile, "%d active allocations totaling %d bytes:\n",
		m_nAllocations, m_nAllocatedBytes );
	int count = 0;
	AllocHeader* pAlloc = m_pHead;
	while( pAlloc )
	{
		Format( pFile, "+ ID %08d, tag ID 0x%08x: 0x%p %d bytes [%s]\n",
			pAlloc->nAllocID, pAlloc->nTagID, ((unsigned char*)pAlloc) + sizeof( AllocHeader ) + sizeof( size_t ),
			pAlloc->nLength, GetCallerForAllocation( pAlloc ) );
		pAlloc = pAlloc->pNext;
		count++;
	}
	Format( pFile, "End of allocations, %d leaked\n", count );
	LeaveCriticalSection( &m_theMutex );

	if( pFile )
	{
		fclose( pFile );
	}
}

//============================================================================

void* MemMgr::Allocate( size_t nSize, Alloc eType )
{
	// Break on this alloc? Increase alloc ID first in case we re-enter this func
	++m_nNextID;
	Assert( m_nNextID-1 != m_nBreakOnAllocID, "Break on this allocation requested" );

	// Allocate the memory block and get pointers
	unsigned char* pRawData = (unsigned char*)malloc( nSize + sizeof( AllocHeader ) + sizeof( size_t ) * 2 );
	if( !pRawData )
	{
		Assert( false, "Memory manager out of memory" );
		return NULL;
	}

	AllocHeader* pHeader = (AllocHeader*)pRawData;
	size_t* pHeadSentinal = (size_t*)(pRawData + sizeof( AllocHeader ));
	size_t* pTailSentinal = (size_t*)(pRawData + sizeof( AllocHeader ) + sizeof( size_t ) + nSize);

	// Set the sentinels
	*pHeadSentinal = s_nHeadSentinalFill;
	*pTailSentinal = s_nTailSentinalFill;

	// Get the current context for stack tracing later on
	#ifdef USE_STACKTRACE
		RecordStackTrace( pHeader );
	#endif

	// Enter mutex for insertion to allocation list
	EnterCriticalSection( &m_theMutex );
	{
		// Setup the allocation header
		pHeader->pNext = NULL;
		pHeader->pPrev = m_pTail;
		pHeader->eType = eType;
		pHeader->nLength = nSize;
		pHeader->nAllocID = m_nNextID;
		pHeader->nTagID = m_nAllocationTag;

		// Update stats
		++m_nAllocations;
		++m_nAllocationsSinceMarker;
		m_nAllocatedBytes += nSize;
		if(m_nAllocatedBytes > m_nHighWatermark)
		{
			m_nHighWatermark = m_nAllocatedBytes;
		}

		if(!m_pHead) m_pHead = pHeader;
		if(m_pTail)
		{
			m_pTail->pNext = pHeader;
		}
		m_pTail = pHeader;
	}
	LeaveCriticalSection( &m_theMutex );
	return pRawData + sizeof( AllocHeader ) + sizeof( size_t );
}

void MemMgr::Deallocate( void* p, Alloc eType )
{
	// Do nothing for null pointers
	if( !p ) return;

	// Grab the header and sentinel positions for this block
	unsigned char* pRawData = (unsigned char*)p;
	size_t* pHeadSentinal = (size_t*)(pRawData - sizeof( size_t ));
	AllocHeader* pHeader = (AllocHeader*)(pRawData - sizeof( size_t ) - sizeof( AllocHeader ));
	size_t* pTailSentinal = (size_t*)(pRawData + pHeader->nLength);

	// Validate sentinels and header
	Assert( *pHeadSentinal == s_nHeadSentinalFill, "Head sentinel damaged!" );
	Assert( pHeader->eType == eType, "Incorrect deallocation used" );
	Assert( *pTailSentinal == s_nTailSentinalFill, "Tail sentinel damaged!" );

	Assert( pHeader->nAllocID != m_nBreakOnAllocID, "Break on this allocation requested" );


	// Unlink block
	EnterCriticalSection( &m_theMutex );
	{
		if( pHeader->pPrev)
		{
			pHeader->pPrev->pNext = pHeader->pNext;
		}
		else
		{
			Assert( pHeader == m_pHead, "Allocator broken!" );
			m_pHead = pHeader->pNext;
		}

		if( pHeader->pNext )
		{
			pHeader->pNext->pPrev = pHeader->pPrev;
		}
		else
		{
			Assert( pHeader == m_pTail, "Allocator broken!" );
			m_pTail = pHeader->pPrev;
		}
	}
	LeaveCriticalSection( &m_theMutex );

	// Update stats
	--m_nAllocations;
	m_nAllocatedBytes -= pHeader->nLength;

	// Deallocate memory
	free( pHeader );
}

//============================================================================

void MemMgr::RecordStackTrace( AllocHeader* pAllocation )
{
#ifdef USE_STACKTRACE
	// See if we can get RtlCaptureContext
/*	LPRtlCaptureContext pfnRtlCaptureContext = (LPRtlCaptureContext)
		GetProcAddress( GetModuleHandleW( L"kernel32.dll" ), "RtlCaptureContext" );
	if( !pfnRtlCaptureContext )
	{
		// RtlCaptureContext not supported
		return;
	}
*/
	if( !ms_pfnRtlCaptureContext ) return;

	// Capture context
	static CONTEXT ctx;
	ms_pfnRtlCaptureContext( &ctx );

	// Init the stack frame for this function
	static STACKFRAME64 theStackFrame;
	memset( &theStackFrame, 0, sizeof( theStackFrame ));
	#ifdef _M_IX86
		DWORD dwMachineType = IMAGE_FILE_MACHINE_I386;
		theStackFrame.AddrPC.Offset = ctx.Eip;
		theStackFrame.AddrPC.Mode = AddrModeFlat;
		theStackFrame.AddrFrame.Offset = ctx.Ebp;
		theStackFrame.AddrFrame.Mode = AddrModeFlat;
		theStackFrame.AddrStack.Offset = ctx.Esp;
		theStackFrame.AddrStack.Mode = AddrModeFlat;
	#elif _M_X64
		DWORD dwMachineType = IMAGE_FILE_MACHINE_AMD64;
		theStackFrame.AddrPC.Offset = ctx.Rip;
		theStackFrame.AddrPC.Mode = AddrModeFlat;
		theStackFrame.AddrFrame.Offset = ctx.Rsp;
		theStackFrame.AddrFrame.Mode = AddrModeFlat;
		theStackFrame.AddrStack.Offset = ctx.Rsp;
		theStackFrame.AddrStack.Mode = AddrModeFlat;
	#elif _M_IA64
		DWORD dwMachineType = IMAGE_FILE_MACHINE_IA64;
		theStackFrame.AddrPC.Offset = ctx.StIIP;
		theStackFrame.AddrPC.Mode = AddrModeFlat;
		theStackFrame.AddrFrame.Offset = ctx.IntSp;
		theStackFrame.AddrFrame.Mode = AddrModeFlat;
		theStackFrame.AddrBStore.Offset = ctx.RsBSP;
		theStackFrame.AddrBStore.Mode = AddrModeFlat;
		theStackFrame.AddrStack.Offset = ctx.IntSp;
		theStackFrame.AddrStack.Mode = AddrModeFlat;
	#else
	#	error "Platform not supported!"
	#endif

	// Walk up the stack
	memset( pAllocation->nPC, 0, sizeof( pAllocation->nPC ));
	for( int i = 0; i < AllocHeader::cnMaxStackFrames; ++i )
	{
		pAllocation->nPC[ i ] = theStackFrame.AddrPC.Offset;
		if( !StackWalk64( dwMachineType, GetCurrentProcess(), GetCurrentThread(), &theStackFrame,
			&ctx, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL ))
		{
			break;
		}
	}
#endif
	UNREFERENCED_PARAMETER( pAllocation );
}

const char* MemMgr::GetCallerForAllocation( AllocHeader* pAllocation )
{
#ifdef USE_STACKTRACE
	const size_t cnBufferSize = 512;
	char szFile[ cnBufferSize ];
	char szFunc[ cnBufferSize ];
	unsigned int nLine;
	static char szBuff[ cnBufferSize * 3 ];

	// Initialise allocation source
	strcpy_s( szFile, "??" );
	nLine = 0;

	// Resolve PC to function names
	DWORD64 nPC;
	for( int i = 0; i < AllocHeader::cnMaxStackFrames; ++i )
	{
		// Check for end of stack walk
		nPC = pAllocation->nPC[ i ];
		if( nPC == 0 ) break;

		// Get function name
		unsigned char byBuffer[ sizeof( IMAGEHLP_SYMBOL64 ) + cnBufferSize ];
		IMAGEHLP_SYMBOL64* pSymbol = (IMAGEHLP_SYMBOL64*)byBuffer;
		DWORD64 dwDisplacement;
		memset( pSymbol, 0, sizeof( IMAGEHLP_SYMBOL64 ) + cnBufferSize );
		pSymbol->SizeOfStruct = sizeof( IMAGEHLP_SYMBOL64 );
		pSymbol->MaxNameLength = cnBufferSize;
		if( !SymGetSymFromAddr64( GetCurrentProcess(), nPC, &dwDisplacement, pSymbol) )
		{
			strcpy_s( szFunc, "??" );
		}
		else
		{
			pSymbol->Name[ cnBufferSize - 1 ] = '\0';
			// See if we need to go further up the stack
			if( strncmp( pSymbol->Name, "MemMgr::", 8 ) == 0 )
			{
				// In MemMgr, keep going...
			}
			else if( strncmp( pSymbol->Name, "operator new", 12 ) == 0 )
			{
				// In operator new or new[], keep going...
			}
			else if( strncmp( pSymbol->Name, "std::", 5 ) == 0 )
			{
				// In STL code, keep going...
			}
			else
			{
				// Found the allocator (Or near to it)
				strcpy_s( szFunc, pSymbol->Name );
				break;
			}
		}
	}

	// Get file/line number
	if( nPC != 0 )
	{
		IMAGEHLP_LINE64 theLine;
		DWORD dwDisplacement;
		memset( &theLine, 0, sizeof( theLine ));
		theLine.SizeOfStruct = sizeof( theLine) ;
		if( !SymGetLineFromAddr64( GetCurrentProcess(), nPC, &dwDisplacement, &theLine ))
		{
			strcpy_s( szFile, "??" );
			nLine = 0;
		}
		else
		{
			const char* pszFile = strrchr( theLine.FileName, '\\ ');
			if( !pszFile ) pszFile = theLine.FileName;
			else ++pszFile;
			strncpy_s( szFile, pszFile, cnBufferSize );
			nLine = theLine.LineNumber;
		}
	}

	// Format into buffer and return
	sprintf_s( szBuff, "%s:%d (%s)", szFile, nLine, szFunc );
	return szBuff;
#else
	UNREFERENCED_PARAMETER( pAllocation );
	return "Stack trace unavailable";
#endif // USE_STACKTRACE
}

//============================================================================

#endif // USE_MMGR
