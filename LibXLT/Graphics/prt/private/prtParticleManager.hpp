/*****************************************************************************
**	prtParticleManager.hpp
**
**		prtParticleManager is a private helper for particle generator classes
**	in the SC package.  It provides a template class which holds a group
**	of particles.  The particle class should define the functions
**  Think, IsExpired, GetNext, GetPrev, SetNext, and SetPrev.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef	PRT_PARTICLEMANAGER_HPP
#error	prtParticleManager.hpp multiply included
#endif
#define	PRT_PARTICLEMANAGER_HPP


//============================================================================
//============================================================================
template <class T>
class prtParticleIterator;


//============================================================================
//============================================================================
template <class T>
class prtParticleManager
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		prtParticleManager();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~prtParticleManager();

		//--------------------------------------------------------------------
		//	Think thinks each particle.
		//--------------------------------------------------------------------
		void Think(float i_SimulationTime, float i_SimulationTimeDelta);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void AddParticle(T* iParticle);

		//--------------------------------------------------------------------
		//	Insert one particle in list before given particle.
		//--------------------------------------------------------------------
		void InsertParticle(T* iInsertBefore, T* iParticle);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void RemoveParticle(T* iParticle);
		
		//--------------------------------------------------------------------
		// make sure you delete the particles yourself when calling
		// ReleaseAllParticles()!
		//--------------------------------------------------------------------
		void ReleaseAllParticles();

		//--------------------------------------------------------------------
		// deletes all of the particles, unlike ReleaseAllParticles()
		//--------------------------------------------------------------------
		void DeleteAllParticles();

		//--------------------------------------------------------------------
		// deletes all particles after and including the current particle
		//--------------------------------------------------------------------
		void DeleteParticlesAfter(T* iParticle);

		//--------------------------------------------------------------------
		//	IsEmpty returns true if there are no particles in the 
		//	prtParticleManager.
		//--------------------------------------------------------------------
		bool IsEmpty() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		int GetNumParticles() const;

		//--------------------------------------------------------------------
		//	GetIterator returns an iterator, which can be used to traverse 
		//	(and read or modify) particles in the manager.
		//--------------------------------------------------------------------
		prtParticleIterator<T> GetIterator() const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		T* GetHead();

	private:
		T* m_Head;
		T* m_Tail;

		int m_Number;
};

//============================================================================
//	prtParticleIterator
//============================================================================
template <class T>
class prtParticleIterator
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~prtParticleIterator();

		//--------------------------------------------------------------------
		// advance to the next particle
		//--------------------------------------------------------------------
		void operator ++();

		//--------------------------------------------------------------------
		// get the pointer to the particle
		//--------------------------------------------------------------------
		T* Ptr();

	private:

		friend prtParticleManager<T>;

		//--------------------------------------------------------------------
		// the constructor is private!  You can only get one of these from the
		// prtParticleManager
		//--------------------------------------------------------------------
		prtParticleIterator(T* iBegin);

		T* m_Cur;
};


//----------------------------------------------------------------------------
//	prtParticleManager implementation
//----------------------------------------------------------------------------
template <class T>
prtParticleManager<T>::prtParticleManager()
:	m_Head(NULL),
	m_Tail(NULL),
	m_Number(0)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
template <class T>
prtParticleManager<T>::~prtParticleManager()
{
	this->DeleteAllParticles();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
template <class T>
void prtParticleManager<T>::Think(float i_SimulationTime, float i_SimulationTimeDelta)
{
	if ( m_Head == NULL ) return;				// nothing to do
	if (i_SimulationTimeDelta == 0.0f) return;	// nothing to do

	T* cur = m_Head;
	
	do
	{
		T* temp = cur;
		cur = static_cast<T*>(cur->GetNext());

		// Think() each particle
		//
		temp->Think(i_SimulationTime, i_SimulationTimeDelta);		

		// Get rid of expired particles
		//
		if( temp->IsExpired() )
		{
			this->RemoveParticle(temp);
		}
	}
	while( cur );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
template <class T>
void prtParticleManager<T>::AddParticle(T* iParticle)
{
	if( iParticle )
	{	
		if( m_Tail )
		{
			m_Tail->SetNext(iParticle);
			iParticle->SetPrev(m_Tail);
			iParticle->SetNext(NULL);
			m_Tail = iParticle;
		}
		else
		{
			m_Head = m_Tail = iParticle;
			iParticle->SetNext(NULL);
			iParticle->SetPrev(NULL);
		}

		m_Number++;
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
template <class T>
void prtParticleManager<T>::InsertParticle(T* iInsertBefore, T* iParticle)
{
	if( iParticle )
	{	
		if (iInsertBefore)
		{
			T* prev = iInsertBefore->GetPrev();
			if (prev)
			{
				iParticle->SetPrev(prev);
				prev->SetNext(iParticle);
			}
			else
			{
				m_Head = iParticle;
				iParticle->SetPrev(NULL);
			}

			iParticle->SetNext(iInsertBefore);
			iInsertBefore->SetPrev(iParticle);
			
			m_Number++;
		}
		else
		{
			AddParticle(iParticle);
		}
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
template <class T>
void prtParticleManager<T>::RemoveParticle(T* iParticle)
{
	if( iParticle )
	{
		T* next = static_cast<T*>(iParticle->GetNext());
		T* prev = static_cast<T*>(iParticle->GetPrev());

		if( prev )
			prev->SetNext(next);
		else
			m_Head = next;			// no previous, must be head

		if( next )
			next->SetPrev(prev);
		else
			m_Tail = prev;			// no next, must be tail

		delete iParticle;
		m_Number--;
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
template <class T>
void prtParticleManager<T>::ReleaseAllParticles()
{
	m_Head = m_Tail = NULL;
	m_Number = 0;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
template <class T>
void prtParticleManager<T>::DeleteAllParticles()
{
	if( m_Head == NULL ) return; // nothing to do

	T* next = m_Head;

	do
	{
		T* temp = next;
		next = static_cast<T*>(next->GetNext());
		delete temp;
	}
	while( next );

	m_Head = m_Tail = NULL;
	m_Number = 0;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
template <class T>
void prtParticleManager<T>::DeleteParticlesAfter(T* cur)
{
	if ( cur == NULL ) return; // nothing to do
	if ( m_Head == NULL ) return; // nothing to do
	if ( m_Head == cur )
	{
		DeleteAllParticles();
		return;
	}

	// Cut our list short at this point
	T* prev = cur->GetPrev();
	prev->SetNext(NULL);
	m_Tail = prev;

	int count = 0;
	do
	{
		T* temp = cur;
		cur = static_cast<T*>(cur->GetNext());
		delete temp;
		count++;
	}
	while( cur );

	m_Number -= count;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
template <class T>
inline bool prtParticleManager<T>::IsEmpty() const
{
	return (m_Head == NULL);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
template <class T>
inline int prtParticleManager<T>::GetNumParticles() const
{
	return m_Number;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
template <class T>
prtParticleIterator<T> prtParticleManager<T>::GetIterator() const
{
	return prtParticleIterator<T>(m_Head);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
template <class T>
inline T* prtParticleManager<T>::GetHead()
{
	return m_Head;
}
	
//----------------------------------------------------------------------------
//	prtParticleIterator implementation
//----------------------------------------------------------------------------
template <class T>
prtParticleIterator<T>::~prtParticleIterator()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
template <class T>
inline void prtParticleIterator<T>::operator ++()
{
	if( m_Cur )
		m_Cur = static_cast<T*>(m_Cur->GetNext());
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
template <class T>
inline T* prtParticleIterator<T>::Ptr()
{	
	return m_Cur;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
template <class T>
prtParticleIterator<T>::prtParticleIterator(T* iBegin)
:	m_Cur(iBegin)
{
}
