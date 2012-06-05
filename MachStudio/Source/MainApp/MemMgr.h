//============================================================================
//	MemMgr.h - Custom memory allocator with leak id logging and stack backtracing.
//
//	StudioGPU
//	Copyright(C) 2008 - All Rights Reserved
//	Author - John Schwab
//============================================================================

#ifndef MEMMGR_H_INCLUDED
#define MEMMGR_H_INCLUDED

// Enable memory manager in debug builds
//bga - Turn on memory manager locally only when debugging memory leaks.
// Don't check it in "on"
#ifdef _DEBUG
//#define USE_MMGR
#endif


#ifdef USE_MMGR

#include <windows.h>

// Comment this out to disable stack trace code (Makes allocator slightly faster)
#define USE_STACKTRACE

// Get the RtlCaptureContext function at runtime so we can support older SDKs
// and machines without it
typedef VOID (WINAPI *LPRtlCaptureContext)(PCONTEXT ContextRecord);

class MemMgr
{
public:
	enum Alloc
	{
		ALLOC_New,
		ALLOC_NewArray,
	};

	MemMgr();
	~MemMgr();

	// Used internally:
	static MemMgr& Get()
	{
		return *ms_pInstance;
	}
	static void Create();
	static void Destroy();

	//========================================================================

	// Dump all active allocations, optionally to memleaks.log
	void DumpAllAllocations( bool bDumpToFile );

	// Return the allocation ID (allocation number) of the next allocation
	size_t GetNextAllocID() const { return m_nNextID; }

	// Return the number of active allocations
	size_t GetNumActiveAllocations() const { return m_nAllocations; }

	// Return the number of bytes of currently allocated memory
	size_t GetAllocatedBytes() const { return m_nAllocatedBytes; }

	// Return the highest number of bytes allocated at one time
	size_t GetHighWatermark() const { return m_nHighWatermark; }

	// Start counting allocations
	void BeginAllocationCount() { m_nAllocationsSinceMarker = 0; }

	// Return the number of allocations since BeginAllocationCount() was called
	size_t GetAllocationCount() const { return m_nAllocationsSinceMarker; }

	// Tag all allocations from now on with an allocation tag. Allocation
	// tags are displayed when allocations are dumped with DumpAllAllocations()
	void SetAllocationTagID( size_t nTag ) { m_nAllocationTag = nTag; }

	// Break on a specified allocation ID
	void SetBreakOnAllocID( size_t nID ) { m_nBreakOnAllocID = nID; }


	//========================================================================
	// Called from operator new
	void* Allocate( size_t nSize, Alloc eType );
	void Deallocate( void* p, Alloc eType );

private:
	struct AllocHeader
	{
		AllocHeader* pNext;
		AllocHeader* pPrev;
		Alloc eType;
		size_t nLength;
		size_t nAllocID;
		size_t nTagID;
		#ifdef USE_STACKTRACE
			static const size_t cnMaxStackFrames = 16;
			DWORD64 nPC[cnMaxStackFrames];
		#endif
	};

	static void RecordStackTrace( AllocHeader* pAllocation );
	static const char* GetCallerForAllocation( AllocHeader* pAllocation );

	static MemMgr*		ms_pInstance;
	CRITICAL_SECTION	m_theMutex;
	AllocHeader*		m_pHead;
	AllocHeader*		m_pTail;
	size_t				m_nAllocations;
	size_t				m_nAllocationsSinceMarker;
	size_t				m_nAllocatedBytes;
	size_t				m_nNextID;
	size_t				m_nBreakOnAllocID;
	size_t				m_nHighWatermark;
	size_t				m_nAllocationTag;
	static LPRtlCaptureContext ms_pfnRtlCaptureContext;
};

#endif // USE_MMGR

#endif // MEMMGR_H_INCLUDED
