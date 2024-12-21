/****************************************************************************\
**  g2dOpenEXROutStream.hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef G2D_OPENEXROSTREAM_HPP
#error g2dOpenEXROutStream.hpp multiply included
#endif
#define G2D_OPENEXROSTREAM_HPP

#ifndef FS_LOCATOR_HPP
#include "Core/Fs/fsLocator.hpp"
#endif 

#include "OpenExr/ImfIO.h"

#include <fstream>
#include <iostream>
#include <ostream>

//============================================================================
//============================================================================
class g2dOpenEXROutStream : public Imf::OStream
{
	public: 

		//--------------------------------------------------------------------
		// g2dOpenEXROutStream()
		//--------------------------------------------------------------------
		g2dOpenEXROutStream(const char fileName[], const fsLocator& i_Locator);

		//--------------------------------------------------------------------
		// ~g2dOpenEXROutStream()
		//--------------------------------------------------------------------
		~g2dOpenEXROutStream();

		//--------------------------------------------------------------------
		// write()
		//--------------------------------------------------------------------
		virtual void write(const char c[/*n*/], int n);

		//--------------------------------------------------------------------
		// tellp()
		//--------------------------------------------------------------------
		virtual uint64_t tellp();

		//--------------------------------------------------------------------
		// seekp()
		//--------------------------------------------------------------------
		virtual void seekp(uint64_t pos);

	private:
		
		std::ofstream * m_OutStream;

};
