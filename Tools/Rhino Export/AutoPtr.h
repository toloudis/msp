#pragma once

//AutoPtr to a single object
template <class T>
class AutoPtr
{
private:
	bool owns;
	T *ptr;

public:
	typedef T element_type;
	explicit AutoPtr( const T * p = 0 ) throw(): owns(p != 0), ptr( const_cast<T*>(p)) {}
	AutoPtr(const AutoPtr<T>& y) throw() : owns(y.owns), ptr(y.release()) {}

	~AutoPtr()
	{
		if (owns)
			delete ptr;
	}

	AutoPtr<T>& operator=(const AutoPtr<T>& y) throw()
	{
		if (this != &y)
		{
			if (ptr != y.get())
			{
				if (owns)
					delete ptr;
				owns = y.owns;
			}
			else if (y.owns)
				owns = true;
			ptr = y.release();
		}
		return (*this);
	}

	bool operator == (const AutoPtr<T>& y) const { return ptr == y.ptr; };
	//bool operator == (const T * y) const { return ptr == y; };
	
	T& operator*() const throw() { return (*ptr); }
	T *operator->() const throw() { return (ptr); }
	T *operator()() const throw() { return (ptr); }
	T *get() const throw() { return (ptr); }
	T *release() const throw() { ((AutoPtr<T> *)this)->owns = false; return (ptr); }
	operator T * () const throw() { return (ptr); }
};