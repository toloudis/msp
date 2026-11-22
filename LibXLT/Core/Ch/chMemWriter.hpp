/*****************************************************************************
**	chMemWriter.hpp
**
**		chMemWriter is a chWriter which writes chunks into memory, laid out
**	byte for byte as chBinWriter lays them out in a file.
**
**	Extra Large Technology
\****************************************************************************/

#ifdef CH_MEMWRITER_HPP
#error chMemWriter.hpp multiply included
#endif
#define CH_MEMWRITER_HPP

#ifndef CH_WRITER_HPP
#include "Core/ch/chWriter.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/Fs/fsLocator.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
class chMemWriter : public chWriter
{
	public:

		//--------------------------------------------------------------------
		//	i_Locator is what GetLocator answers (parsers that write paths
		//	relative to the document ask for it).
		//--------------------------------------------------------------------
		explicit chMemWriter(const fsLocator& i_Locator = fsLocator());

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~chMemWriter();

		//--------------------------------------------------------------------
		//	GetBytes returns everything written so far.
		//--------------------------------------------------------------------
		const std::vector<uint8_t>& GetBytes() const { return m_Bytes; }

		//--------------------------------------------------------------------
		//	chWriter overrides, written as chBinWriter writes them.
		//--------------------------------------------------------------------
		virtual void WriteChunkHeader(	chDefs::Name i_Name,
										chDefs::Version i_Version,
										bool i_Container);
		virtual void FinishChunk();

		virtual void Write(int8_t i_Val);
		virtual void Write(uint8_t i_Val);
		virtual void Write(int16_t i_Val);
		virtual void Write(uint16_t i_Val);
		virtual void Write(int32_t i_Val);
		virtual void Write(uint32_t i_Val);
		virtual void Write(int64_t i_Val);
		virtual void Write(uint64_t i_Val);
		virtual void Write(float i_Val);
		virtual void Write(double i_Val);
		virtual void Write(bool i_Val);
		virtual void Write(const char* i_Val);
		virtual void Write(const char* i_Val, int i_Length);
		virtual void Write(const envAppVersion& i_Val);
		virtual	void Write(const void* i_Data, int64_t i_NumBytes);

		using chWriter::Write;

		virtual const fsLocator& GetLocator() const;

	private:

		template<class T>
		void WriteValue(T i_Val) { Write(&i_Val, int64_t(sizeof(i_Val))); }

		fsLocator m_Locator;
		std::vector<uint8_t> m_Bytes;
		std::vector<size_t> m_HeaderLocs;
};
