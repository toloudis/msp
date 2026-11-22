/****************************************************************************\
**	prtyFileName.hpp
**
**		FileName property
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef PRTY_FILENAME_HPP
#error prtyFileName.hpp multiply included
#endif
#define PRTY_FILENAME_HPP

#ifndef PRTY_PROPERTYTEMPLATE_HPP
#include "Core/prty/prtyPropertyTemplate.hpp"
#endif 

#include <string>



//============================================================================
//============================================================================
class prtyFileName : public prtyPropertyTemplate<std::string, const std::string&>
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		prtyFileName();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		prtyFileName(const std::string& i_Name);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		prtyFileName(const std::string& i_Name, const std::string& i_InitialValue);

		//--------------------------------------------------------------------
		//	The type of property it is
		//--------------------------------------------------------------------
		virtual const char* GetType();

		//--------------------------------------------------------------------
		//	return a null-terminated std::string
		//--------------------------------------------------------------------
		const std::string GetString() const;

		//--------------------------------------------------------------------
		//	operators											
		//--------------------------------------------------------------------
		prtyFileName& operator =(const prtyFileName& i_Property);
		prtyFileName& operator =(const std::string& i_Value);

		//--------------------------------------------------------------------
		//	comparison operators											
		//--------------------------------------------------------------------
		bool operator ==(const prtyFileName& i_Property) const;
		bool operator !=(const prtyFileName& i_Property) const;
		bool operator ==(const std::string& i_Value) const;
		bool operator !=(const std::string& i_Value) const;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void Read(chReader& io_Reader);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void Write(chWriter& io_Writer) const;

};
