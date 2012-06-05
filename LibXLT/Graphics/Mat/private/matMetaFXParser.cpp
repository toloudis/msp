/*****************************************************************************
**	matMetaFXParser.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mat/matMetaFXParser.hpp"
#include "Graphics/mat/matMetaFX.hpp"

#include "Core/ch/chBinReader.hpp"
#include "Core/ch/chBinWriter.hpp"
#include "Core/ch/chChunkParserUtil.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/Fs/fsAbsolutePathMgr.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "Core/Gf/gfFileBin.hpp"


//============================================================================
//============================================================================
namespace matMetaFXParser
{
	namespace
	{
		const chDefs::Name c_SHNM = chDefs::MakeName('S', 'H', 'N', 'M');	// SHader NaMe
		const chDefs::Name c_CMFX = chDefs::MakeName('C', 'M', 'F', 'X');	// CoMpiled FX (DX11)
		const chDefs::Name c_HLSL = chDefs::MakeName('H', 'L', 'S', 'L');	// HLSL code without header or footer (DX11)
		const chDefs::Name c_MTSL = chDefs::MakeName('M', 'T', 'S', 'L');	// MetaSL code for recompiling
		const chDefs::Name c_SHDV = chDefs::MakeName('S', 'H', 'D', 'V');	// Shader version
		const chDefs::Name c_DTBK = chDefs::MakeName('D', 'T', 'B', 'K');	// DaTa BlocK of raw data within chunk
		const chDefs::Name c_SRCS = chDefs::MakeName('S', 'R', 'C', 'S');	// SouRCe String in HLSL or MetaSL within chunk

		
		void write_shader_version( chWriter& io_Writer,
								   const envAppVersion &i_Version )
		{
			const int c_SHDV_VERSION = 0;
			io_Writer.WriteChunkHeader( c_SHDV, c_SHDV_VERSION, false );
			io_Writer.Write( i_Version );
			io_Writer.FinishChunk();	
		}
		void write_raw_data( chWriter& io_Writer,
							 const std::vector<envType::UInt8> &i_ShaderData )
		{
			const int c_DTBK_VERSION = 0;
			io_Writer.WriteChunkHeader( c_DTBK, c_DTBK_VERSION, false );
			io_Writer.Write( &i_ShaderData[0], i_ShaderData.size() );
			io_Writer.FinishChunk();	
		}
		void write_string_data( chWriter& io_Writer,
								const std::string &i_ShaderData )
		{
			const int c_SRCS_VERSION = 0;
			io_Writer.WriteChunkHeader( c_SRCS, c_SRCS_VERSION, false );
			io_Writer.Write( i_ShaderData ); // write as string unencrypted
			io_Writer.FinishChunk();	
		}
	
		void read_raw_block(chReader& io_Reader, 
					   matMetaFXBlock< std::vector<envType::UInt8> > &o_DataBlock)
		{
			chDefs::Name name;
			chDefs::Size size;
			chDefs::Version version;

			while( io_Reader.ReadChunkHeader(name, version, size) )
			{
				if (name == c_SHDV)
				{
					io_Reader.Read( o_DataBlock.m_Version );
				}
				else if (name == c_DTBK)
				{
					o_DataBlock.m_Data.resize(size);
					io_Reader.Read( &o_DataBlock.m_Data[0], size );
					o_DataBlock.m_bHasData = (!o_DataBlock.m_Data.empty());
				}
				io_Reader.FinishChunk();
			}
		}
		void read_string_block(chReader& io_Reader, 
					   matMetaFXBlock< std::string > &o_DataBlock)
		{
			chDefs::Name name;
			chDefs::Size size;
			chDefs::Version version;

			while( io_Reader.ReadChunkHeader(name, version, size) )
			{
				if (name == c_SHDV)
				{
					io_Reader.Read( o_DataBlock.m_Version );
				}
				else if (name == c_SRCS)
				{
					// read unencrypted string
					io_Reader.Read( o_DataBlock.m_Data );
					o_DataBlock.m_bHasData = (!o_DataBlock.m_Data.empty());
				}
				io_Reader.FinishChunk();
			}
		}
	}

	//------------------------------------------------------------------------
	// Reads multi-format shader from file. 
	//------------------------------------------------------------------------
	void ReadMetaFX( const fsLocator &i_Locator,
						matMetaFX &o_ShaderData)
	{

		gfFileBin ifile(i_Locator, fsFileStream::e_ReadOnly, gfFileBin::e_LittleEndian);
		ifile.ReadHeader();
		chBinReader reader(ifile);

		chDefs::Name name;
		chDefs::Size size;
		chDefs::Version version;

		while( reader.ReadChunkHeader(name, version, size) )
		{
			if (name == c_SHNM)
			{
				chChunkParserUtil::Read( reader, o_ShaderData.m_ShaderName );
			}
			else if (name == c_CMFX)
			{
				read_raw_block( reader, o_ShaderData.m_CompiledFX );
			}
			else if (name == c_HLSL)
			{
				read_string_block( reader, o_ShaderData.m_HLSLSource );
			}
			else if (name == c_MTSL)
			{
				read_string_block( reader, o_ShaderData.m_MetaSLSource );
			}
			reader.FinishChunk();
		}
	}

	//--------------------------------------------------------------------
	// Writes multi-format shader to file.
	//--------------------------------------------------------------------
	void WriteMetaFX( const fsLocator &i_Locator,
					  const matMetaFX& i_ShaderData )
	{	
		// Create new locator
		if( fsFileUtil::FileExists(i_Locator) )
			fsFileUtil::DeleteFile(i_Locator);
		fsFileUtil::CreateFile(i_Locator);

		gfFileBin ofile(i_Locator, fsFileStream::e_WriteOnly, gfFileBin::e_LittleEndian);
		ofile.WriteHeader();
		chBinWriter writer(ofile);

		if (!i_ShaderData.m_ShaderName.empty())
		{
			const int c_SHNM_VERSION = 0;
			writer.WriteChunkHeader( c_SHNM, c_SHNM_VERSION, false );
			chChunkParserUtil::Write(writer, i_ShaderData.m_ShaderName);
			writer.FinishChunk();	
		}

		// FX compiled in binary code
		if (i_ShaderData.m_CompiledFX.m_bHasData)
		{
			const int c_CMFX_VERSION = 0;
			writer.WriteChunkHeader( c_CMFX, c_CMFX_VERSION, true );

			write_shader_version( writer, i_ShaderData.m_CompiledFX.m_Version );
			write_raw_data( writer, i_ShaderData.m_CompiledFX.m_Data );
	
			writer.FinishChunk();	
		}

		// HLSL source code without header and footer
		if (i_ShaderData.m_HLSLSource.m_bHasData)
		{
			const int c_HLSL_VERSION = 0;
			writer.WriteChunkHeader( c_HLSL, c_HLSL_VERSION, true );

			write_shader_version( writer, i_ShaderData.m_HLSLSource.m_Version );
			write_string_data( writer, i_ShaderData.m_HLSLSource.m_Data );
	
			writer.FinishChunk();	
		}

		// MetaSL source code for recompiling when needed or for use in mental ray export
		if (i_ShaderData.m_MetaSLSource.m_bHasData)
		{
			const int c_MTSL_VERSION = 0;
			writer.WriteChunkHeader( c_MTSL, c_MTSL_VERSION, true );

			write_shader_version( writer, i_ShaderData.m_MetaSLSource.m_Version );
			write_string_data( writer, i_ShaderData.m_MetaSLSource.m_Data );
	
			writer.FinishChunk();	
		}
	}

} // end of namespace

