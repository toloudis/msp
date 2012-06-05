/*****************************************************************************
**  HashSetPoint3
**
**	HashSetPoint3 class is a wrapper class around stext::hash_set
** for storing quantized 3d points. This also serves the additional
** functionality of hashing a 3d point( which cannot be done through
** stdext::hash_set)
**	
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/


#ifdef MAXEXP_HASHSETPOINT3_HPP
#error MAXEXP_HASHSETPOINT3_HPP multiply defined!!
#endif
#define MAXEXP_HASHSETPOINT3_HPP

#include "HashSetPoint3Aux.hpp"

#include <algorithm>
#include <hash_set>
#include <cmath>


namespace MaxExp
{
	//========================================================================
	// HashComparePoint3 is the equivalent of hash_compare class in 
	// stdext::hash_set
	//
	// 
	// We have reproduced the interface of stdext::hash_compare here
	// with some modifications in template parameters
	
	// stdext::hash_compare accepts two template paramters , 
	// template<class Key, class Pred = _STD less<Key> > class hash_compare { ... }
	
	// HashComparePoint3 accepts the following template parameters
 //
	//	1)a point class P, (P serves the same role as Key). 
	//	2)a PTraits class to access the components of P.(the default is PointTraits<P>
	//		which is defined in HashSetPoint3Aux.hpp
	//	3)Unlike in stdext::hash_compare, the exact algorithm
	//		used to compute the hash value of an instance of P, is
	//		also, a template parameter. The default is StdLibHashAlgorithm,
	//		defined in HashSetPoint3Aux
	//Unlike in hash_comapre, the predicate function is hard wired to be LessPoint3,
	//So we dont need to pass that as a template parameter.
	
	
	// We choose to privately derive the HashComparePoint3 from H, the hashing algorithm
	// Reason:
	// If 'class B' derives privately from 'class A', then it is syntactically
	// equal to 'class B has a class A'. 
	// http://www.parashift.com/c++-faq-lite/private-inheritance.html
	//========================================================================
	template< 
		class P, 
		class PTraits = PointTraits<P>, 
		class H = StdLibHashAlgorithm 
	    >
	class HashComparePoint3 : private H
	{
	public:
		//The following values are from the hash_ompare function
		//defined in xhash

		// parameters for hash table
		enum {
			bucket_size = 4,// 0 < bucket_size
			min_buckets = 8 // min_buckets = 2 ^^ N, 0 < N
		};

		HashComparePoint3():
			m_Comp() // construct with default comparator
		{
		}

		HashComparePoint3(LessPoint3<P, PTraits> i_LessP):
			m_Comp(i_LessP) // construct with LessPoint3 comparator
			{
			}

		size_t operator()(const P& i_Point3) const
		{	
			float f[4];
			f[0] = PTraits::X(i_Point3); 
			f[1] = PTraits::Y(i_Point3); 
			f[2] = PTraits::Z(i_Point3);
			unsigned char * fAsUCharPtr = reinterpret_cast<unsigned char *>(f);
			//hash_value is from H, the hashing algorithm
			size_t hash = hash_value( fAsUCharPtr );
			return hash;	
		}

		bool operator()(const P &i_First, const P &i_Second) const
		{	
			//comp is defined in hash_compare 
			return m_Comp(i_First, i_Second);
		}

		LessPoint3<P, PTraits> m_Comp;
	};

	


	
	//========================================================================
	// HashPointSet a wrapper around stdext::hash_set

	// HashPointSet is specialized to accomodate the hashing of
	// 3d points in two ways
	// 1) it quantizes the 3d points into a grid of fixed 3d points
	//	  All the 3dpoints stored will be the quantized values.
	//   So once the 3d point is inserted, it is automatically converted and stored as
	//	  the quantized value. Thiss is lossy, but may be acceptable in
	//	  several applications. 
	//	  The quantizer is provided as a template parameter, enabling 
	//	  fine tuning from the user. The default value of this template
	//   paramter is the 'Quantizer' class defined in HashSetPoint3Aux.hpp
	//	  

	// 2) it provides an appropriate way to compute the hash value of a 3d point.
	//    This is also done through a template class H.
	//	   The default template value for H is 'StdLibHashAlgorithm' defined in 
	//	   HashSetPoint3Aux.hpp

	// We choose to privately derive the HashSetPoint3 from stdext::hash_set
	// If 'class B' derives privately from 'class A', then it is syntactically
	// equal to 'class B has a class A'. 
	// http://www.parashift.com/c++-faq-lite/private-inheritance.html
	// Also this private inheritance prevents users from up-casting a HashSetPoint3 as a stdext::hash_set,
	// (which would have been possible if this was a public derivation).
 // Since the parent class stdext::hash_set doesnt have any virtual destrictor,
 // such an up-cast could have resulted in memorey leaks.
	// Also, since all the use full interface functions of hash_set are
	// re-wrapped or exposed by the HashSetPoint3, there is no loss of
	// functionality.


	// Also, quantization of the same point is called several times,
	// we cache the quantized value.
	//========================================================================
	template< 
			class P, 
			class PTraits= PointTraits< P >, 
			class H = StdLibHashAlgorithm , 
			class Q = Quantizer
		>

	class HashSetPoint3 : private stdext::hash_set< P, HashComparePoint3<P, PTraits, H>  >
	{
	private:
		typedef HashComparePoint3<P, PTraits, H> HCompare;
		typedef hash_set< P, HCompare > ParentType;
	public:
		HashSetPoint3():
		  ParentType()
		  {
		  }
		explicit HashSetPoint3(const HashComparePoint3<P, PTraits, H>  &i_HCompare ): 
			ParentType( i_HCompare )
			{
			}


		template<class I>
		HashSetPoint3(I i_First, I i_Last):
			ParentType (HCompare()())
			{	
			// construct set from sequence, defaults
			_DEBUG_RANGE(i_First, i_Last);
			for (; i_First != i_Last; ++i_First)
				insert(*i_First);
			}
	
		template<class I>
		HashSetPoint3(I i_First, I i_Last, const HCompare& i_HCompare )
		: ParentType( i_HCompare )
		{	
			// construct set from sequence, comparator
			_DEBUG_RANGE( i_First, i_Last );
			for (; i_First != i_Last; ++i_First)
				insert(*i_First);
		}

		//use the parent's implementation
		//of the following
		using ParentType::iterator;
		using ParentType::const_iterator;
		using ParentType::size;
		using ParentType::begin;
		using ParentType::end;
		using ParentType::erase;

		const P &Quantize( const P& i_Point3) const
		{
			P ret;
			PTraits::X(ret) = m_Quantizer(PTraits::X(i_Point3));
			PTraits::Y(ret) = m_Quantizer(PTraits::Y(i_Point3));
			PTraits::Z(ret) = m_Quantizer(PTraits::Z(i_Point3));
			const iterator cit = m_QuantizedCache.insert( ret ).first;
			return *cit;
		}
		

		std::pair<typename ParentType::iterator, bool> insert(const P& i_Point3)
		{	
			const P &val_quantized = Quantize( i_Point3 );
			return (ParentType::insert(val_quantized));
		}

		typename ParentType::iterator insert(const_iterator i_It, const P&  i_Point3)
		{	
			const P &val_quantized = Quantize( i_Point3 );
			return (ParentType::insert( i_It, val));
		}

	
		template<class I>
		void insert(I i_First, I i_Last)
		{	
			list< I::value_type > l;
			for( ; i_First != i_Last; ++i_First )
			{
				const P & v = *i_First;
				const P &v_quantize = Quantize( v );
				l.push_back( v_quantize);
			}
			ParentType::insert(l.begin(), l.end());
		}

		typename ParentType::iterator find( const P &i_Point3 )
		{
				const P  &p_quantize = Quantize( i_Point3 );
				return ParentType::find( p_quantize);
		}

		typename ParentType::const_iterator find ( const P &i_Point3 ) const
		{
			const P &p_quantize = Quantize( i_Point3 );			
			return ParentType::find( p_quantize);
		}

		typename ParentType::iterator lower_bound( const P &i_Point3 )
		{
			const P  &p_quantize = Quantize( i_Point3 );			
			return ParentType::lower_bound( p_quantize);
		}

		typename ParentType::const_iterator lower_bound( const P &i_Point3 ) const 
		{
			const P  &p_quantize = Quantize( i_Point3 );			
			return ParentType::lower_bound( p_quantize);
		}

		typename ParentType::iterator upper_bound( const P &i_Point3 )
		{
			const P  &p_quantize = Quantize( i_Point3 );			
			return ParentType::upper_bound( p_quantize);
		}

		typename ParentType::const_iterator upper_bound( const P &i_Point3 ) const 
		{
			const P  &p_quantize = Quantize( i_Point3 );			
			return ParentType::upper_bound( p_quantize);
		}

		std::pair< typename ParentType::iterator , typename ParentType::iterator > equal_range( const P & i_Point3)
		{
			const P  &p_quantize = Quantize( i_Point3 );			
			return ParentType::equal_range( p_quantize );
		}

		std::pair< typename ParentType::const_iterator , typename ParentType::const_iterator > equal_range( const P & i_Point3) const
		{
			const P  &p_quantize = Quantize( i_Point3 );			
			return ParentType::equal_range( p_quantize );
		}

		void clear()
		{	
			ParentType::clear();
			m_QuantizedCache.clear();
		}

		private:
			mutable ParentType m_QuantizedCache;
			Q m_Quantizer;
	};



} //namespace MaxExp