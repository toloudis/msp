/****************************************************************************\
**  gltfImageExtract.cpp
**
**      see gltfImageExtract.hpp
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GLTF_IMAGEEXTRACT_HPP
#include "ImportExport/gltf/import/private/gltfImageExtract.hpp"
#endif
// The implementation is compiled in gltfSdkManager.cpp
#include "ImportExport/gltf/import/private/stb_image_write.h"

#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif
#ifndef FS_FILEUTIL_HPP
#include "Core/fs/fsFileUtil.hpp"
#endif

#include <fstream>
#include <string.h>
#include <system_error>
#include <vector>


namespace gltfImageExtract
{
	namespace
	{
		//--------------------------------------------------------------------
		// stb_image_write callback that appends to a std::vector
		//--------------------------------------------------------------------
		void append_to_vector(void* i_pContext, void* i_pData, int i_Size)
		{
			std::vector<unsigned char>& bytes = *(std::vector<unsigned char>*)i_pContext;
			bytes.insert(bytes.end(), (const unsigned char*)i_pData, (const unsigned char*)i_pData + i_Size);
		}
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	std::string ExtractEmbeddedImage(const tinygltf::Model& i_Model, int i_ImageIndex,
									 const fsLocator& i_ContainingFile, WriteResult* o_pResult)
	{
		if (o_pResult)
			*o_pResult = e_Failed;

		const tinygltf::Image& image = i_Model.images[i_ImageIndex];

		// Get the still-encoded (png/jpg) image bytes
		const unsigned char* pBytes = NULL;
		size_t num_bytes = 0;
		const std::string& mime_type = image.mimeType;
		if (image.bufferView >= 0 && image.bufferView < (int)i_Model.bufferViews.size())
		{
			const tinygltf::BufferView& bv = i_Model.bufferViews[image.bufferView];
			if (bv.buffer >= 0 && bv.buffer < (int)i_Model.buffers.size() &&
				bv.byteOffset + bv.byteLength <= i_Model.buffers[bv.buffer].data.size() &&
				bv.byteLength > 0)
			{
				pBytes = &i_Model.buffers[bv.buffer].data[0] + bv.byteOffset;
				num_bytes = bv.byteLength;
			}
		}

		// Data URI images are only kept decoded by tinygltf, so those
		// are re-encoded as png from their 8 bit pixels. stb_image_write's
		// output is the same for the same pixels, so an unchanged image
		// still matches the file written last time.
		const bool bWritePixels = !pBytes && !image.image.empty() && image.bits == 8 &&
			image.width > 0 && image.height > 0 && image.component >= 1 && image.component <= 4 &&
			image.image.size() >= (size_t)image.width * image.height * image.component;
		if (!pBytes && !bWritePixels)
		{
			DBG_WARNING("glTF image " << i_ImageIndex << " (" << image.name << ") has no readable embedded data");
			return "";
		}

		std::vector<unsigned char> png_bytes;
		if (bWritePixels)
		{
			if (!stbi_write_png_to_func(&append_to_vector, &png_bytes, image.width, image.height, image.component,
										&image.image[0], image.width * image.component))
			{
				DBG_WARNING("Could not encode embedded glTF image " << i_ImageIndex << " (" << image.name << ") as png");
				return "";
			}
			pBytes = png_bytes.data();
			num_bytes = png_bytes.size();
		}

		// Our texture loader detects the format from the file header,
		// the extension is just for people browsing the folder.
		const char* ext = "png";
		if (!bWritePixels && (mime_type.find("jpeg") != std::string::npos || mime_type.find("jpg") != std::string::npos))
			ext = "jpg";

		std::string model_path;
		fsFileUtil::LocatorToANSIFilename(i_ContainingFile, model_path);
		const size_t slash = model_path.find_last_of("\\/");
		const std::string model_dir = (slash == std::string::npos) ? std::string() : model_path.substr(0, slash + 1);
		std::string model_name = (slash == std::string::npos) ? model_path : model_path.substr(slash + 1);
		const size_t dot = model_name.find_last_of('.');
		if (dot != std::string::npos)
			model_name = model_name.substr(0, dot);

		const std::string texture_dir = model_dir + model_name + "_textures";
		std::error_code ec;
		std::filesystem::create_directories(std::filesystem::u8path(texture_dir), ec);	// fine if it already exists

		const std::string texture_path = texture_dir + "\\image" + std::to_string(i_ImageIndex) + "." + ext;

		const WriteResult result = WriteFileIfChanged(std::filesystem::u8path(texture_path), pBytes, num_bytes);
		if (o_pResult)
			*o_pResult = result;
		if (result == e_Failed)
		{
			DBG_WARNING("Could not write embedded glTF image to " << texture_path);
			return "";
		}
		return texture_path;
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	WriteResult WriteFileIfChanged(const std::filesystem::path& i_Path, const unsigned char* i_pBytes, size_t i_NumBytes)
	{
		// Compare sizes first, so only a same-sized file is read back
		std::error_code ec;
		if (std::filesystem::file_size(i_Path, ec) == i_NumBytes && !ec)
		{
			std::vector<char> existing(i_NumBytes);
			std::ifstream in(i_Path, std::ios::in | std::ios::binary);
			if (in && (i_NumBytes == 0 || in.read(existing.data(), (std::streamsize)i_NumBytes)) &&
				(i_NumBytes == 0 || memcmp(existing.data(), i_pBytes, i_NumBytes) == 0))
			{
				return e_Unchanged;
			}
		}

		std::ofstream out(i_Path, std::ios::out | std::ios::binary | std::ios::trunc);
		if (!out)
			return e_Failed;
		out.write((const char*)i_pBytes, (std::streamsize)i_NumBytes);
		out.close();
		return out ? e_Written : e_Failed;
	}
}
