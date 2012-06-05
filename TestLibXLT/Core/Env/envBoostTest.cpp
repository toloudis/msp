/*****************************************************************************
**  envBoostTest.cpp
**
**      envBoostTest is a test of envBoost.hpp
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "envBoostTest.hpp"

#include "Core/env/envBoost.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/dbg/dbgLog.hpp"

#include <string>
#include <vector>

//====================================================================
// Anonymous Namespace for local variables and functions
//====================================================================
namespace 
{
	
	class LogClass
	{
		public:
			LogClass()
			{
				DBG_LOG0("In LogClass constructor.");
			}
			~LogClass()
			{
				DBG_LOG0("In LogClass destructor.");
			}
			void DoFunc()
			{
				DBG_LOG0("  In LogClass::DoFunc()");
			}
			void DoConstFunc() const
			{
				DBG_LOG0("  In LogClass::DoConstFunc()");
			}

	};

	void test_shared_pointer()
	{
		DBG_LOG0("Creating new LogClass into a shared pointer.");
		shared_ptr<LogClass> log1(new LogClass);
		
		{
			DBG_LOG0("Sharing the fírst pointer in a local space.");
			shared_ptr<LogClass> log2(log1);
		}

		DBG_LOG0("Out of local space, second shared pointer should be out of scope.");
		DBG_LOG0("Leaving function, should free the original LogClass.");
	}

	class OldStyleClass
	{
	public:
		OldStyleClass()
			: m_pLogger(new LogClass)
		{
		}
		~OldStyleClass()
		{
			delete m_pLogger;
			envSTLHelpers::DeleteContainer(m_LogList);
		}

		void AddToList(LogClass *pLogClass)
		{
			m_LogList.push_back(pLogClass);
		}

		void SetLogger(LogClass *pLogClass)
		{
			// have to delete old one first
			delete m_pLogger;
			m_pLogger = pLogClass;
		}
		const LogClass* GetLogger() const
		{
			return m_pLogger;
		}
		LogClass* Logger()
		{
			return m_pLogger;
		}

	private:
		LogClass *m_pLogger;
		std::vector<LogClass*> m_LogList;
	};

	class InternalClass
	{
	public:
		InternalClass()
			: m_pLogger(new LogClass)
		{
		}
		~InternalClass()
		{
			// no need to delete anything
		}
		void AddToList(LogClass *pLogClass)
		{
			// This won't work because it is explicit contructor
			//m_LogList.push_back(pLogClass);
			// Use of a temporary is not recommended in the boost website reference
			//m_LogList.push_back( shared_ptr<LogClass>(pLogClass) );
			
			shared_ptr<LogClass> temp(pLogClass);
			m_LogList.push_back( temp );
		}
		void SetLogger(LogClass *pLogClass)
		{
			// Could we use this temporary?
			//m_pLogger = shared_ptr<LogClass>(pLogClass);

			shared_ptr<LogClass> temp(pLogClass);
			m_pLogger = temp;
		}
		const LogClass* GetLogger() const
		{
			return m_pLogger.get();
		}
		LogClass* Logger()
		{
			return m_pLogger.get();
		}

	private:
		shared_ptr<LogClass> m_pLogger;
		std::vector< shared_ptr<LogClass> > m_LogList;
	};

	template<class T>
	void test_class_style()
	{
		shared_ptr<T> pPtr(new T());
		pPtr->SetLogger(NULL);
		pPtr->SetLogger(new LogClass());
		pPtr->AddToList(new LogClass());
		pPtr->GetLogger()->DoConstFunc();
		pPtr->Logger()->DoFunc();
	}

	
	class ExternalClass
	{
	public:
		ExternalClass()
			: m_pLogger(new LogClass)
		{
		}
		~ExternalClass()
		{
			// no need to delete anything
		}
		void AddToList(shared_ptr<LogClass> pLogClass)
		{
			m_LogList.push_back( pLogClass );
		}
		void SetLogger(shared_ptr<LogClass> pLogClass)
		{
			m_pLogger = pLogClass;
		}
		const shared_ptr<LogClass>& GetLogger() const
		{
			return m_pLogger;
		}
		shared_ptr<LogClass>& Logger()
		{
			return m_pLogger;
		}

	private:
		shared_ptr<LogClass> m_pLogger;
		std::vector< shared_ptr<LogClass> > m_LogList;
	};

	void test_external_class()
	{
		shared_ptr<ExternalClass> pPtr(new ExternalClass());

		// Can't just set the pointer to NULL to claear it out
		//pPtr->SetLogger(NULL);
		shared_ptr<LogClass> empty;
		pPtr->SetLogger(empty);

		// This won't work because it is explicit contructor
		//pPtr->SetLogger(new LogClass());
		// Use of a temporary is not recommended in the boost website reference
		//pPtr->SetLogger(shared_ptr<LogClass>(new LogClass()));
		// So have to do it this way:
		shared_ptr<LogClass> temp1(new LogClass());
		pPtr->SetLogger(temp1);

		shared_ptr<LogClass> temp2(new LogClass());
		pPtr->AddToList(temp2);

		pPtr->GetLogger()->DoConstFunc();
		pPtr->Logger()->DoFunc();
	}

	void test_weak_pointer()
	{
		shared_ptr<ExternalClass> pPtr(new ExternalClass());

		weak_ptr<LogClass> pWeakPtr(pPtr->GetLogger());

		// Use a working shared pointer in order to make function calls
		if (shared_ptr<LogClass> pWorkingPtr1 = pWeakPtr.lock())
		{
			pWorkingPtr1->DoFunc();

			// Destroy the original shared pointer, the working pointer is
			// still valid because it is one of the shared references
			DBG_LOG0("Resetting original class");
			pPtr.reset();

			pWorkingPtr1->DoFunc();
		}
		else
		{
			DBG_LOG0("Weak ptr is not valid.");
		}

		// Now try to use the weak pointer again, the original
		// shared pointer pPtr is empty now
		
		// This will throw a bad_weak_ptr exception
		//shared_ptr<LogClass> pWorkingPtr(pWeakPtr);

		// This method checks to see if the pointer is 
		// valid before using
		if (shared_ptr<LogClass> pWorkingPtr2 = pWeakPtr.lock())
		{
			pWorkingPtr2->DoFunc();
		}
		else
		{
			DBG_LOG0("Weak ptr is not valid anymore.");
		}
	}
}

//========================================================================
//	RunTest - executes the envSTLHelpers tests
//========================================================================
void envBoostTest::RunTest()
{
	DBG_LOG0("Begin envBoostTest::RunTest()");

	//test_shared_pointer();

	DBG_LOG0("\nTesting old style class");
	test_class_style<OldStyleClass>();

	DBG_LOG0("\nTesting new style internal class");
	test_class_style<InternalClass>();

	DBG_LOG0("\nTesting new style external class");
	test_external_class();

	DBG_LOG0("\nTesting weak pointers");
	test_weak_pointer();

	DBG_LOG0("\nEnd envBoostTest::RunTest()");
}