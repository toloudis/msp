/****************************************************************************\
**  gltfImageExtract.hpp
**
**      gltfImageExtract writes images embedded in a glTF (a .glb buffer
**	view or a data URI) out to files, since our texture manager loads
**	textures by name. A file is only written when its bytes differ from
**	the file already there, so loading the same model again rewrites
**	nothing.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GLTF_IMAGEEXTRACT_HPP
#error gltfImageExtract.hpp multiply included
#endif
#define GLTF_IMAGEEXTRACT_HPP

#include "tiny_gltf.h"

#include <filesystem>
#include <string>

class fsLocator;

namespace gltfImageExtract
{
	enum WriteResult
	{
		e_Written,		// the file was missing or different, and was written
		e_Unchanged,	// the file already held these bytes
		e_Failed		// the file could not be written
	};

	//--------------------------------------------------------------------
	// WriteFileIfChanged - write i_pBytes to i_Path unless the file there
	// already holds exactly these bytes. The folder must exist.
	//--------------------------------------------------------------------
	WriteResult WriteFileIfChanged(const std::filesystem::path& i_Path, const unsigned char* i_pBytes, size_t i_NumBytes);

	//--------------------------------------------------------------------
	// ExtractEmbeddedImage - write image i_ImageIndex of i_Model (a .glb
	// buffer view, or a data URI tinygltf has decoded) to
	// "<model dir>\<model name>_textures\image<N>.<ext>", creating the
	// folder if needed, and return that full path. Still-encoded bytes are
	// written as they are; decoded 8 bit pixels are encoded as PNG first.
	// Either way the file is left alone if it already holds those bytes.
	// Returns "" (with a warning) if the image has no readable data or
	// can't be written. o_pResult, if given, receives the write result
	// (e_Failed when there is no data).
	//--------------------------------------------------------------------
	std::string ExtractEmbeddedImage(const tinygltf::Model& i_Model, int i_ImageIndex,
									 const fsLocator& i_ContainingFile, WriteResult* o_pResult = NULL);
}
