/*****************************************************************************
**  gtTarget.hpp
**
**      Render targets for the scene parity tests: an RGBA8 render-target
**      texture (with a depth buffer) made through matTextureMgr, and its
**      pixels read back on whichever backend is up (gtBackend).
**
**	Extra Large Technology
\****************************************************************************/
#pragma once

#include "gtBackend.hpp"

#include <cstdint>
#include <vector>

class g2dRenderTarget;
class matTexture;
struct ID3D11Texture2D;
struct ID3D11Texture3D;
namespace gvk { class vkdImage; }

struct gtPicture
{
	int						m_Width = 0;
	int						m_Height = 0;
	std::vector<uint8_t>	m_Rgba;		// rows tightly packed, top row first
	bool Valid() const		{ return !m_Rgba.empty() && m_Rgba.size() == (size_t)m_Width * m_Height * 4; }
};

class gtTarget
{
	public:
		//	i_Width x i_Height RGBA8 with a depth buffer; made current and
		//	cleared to i_Clear (and depth 1)
		gtTarget(gtBackend::Kind i_Kind, int i_Width, int i_Height, const float i_Clear[4]);
		~gtTarget();

		g2dRenderTarget*	Target() const;
		matTexture*			Texture() const		{ return m_pTexture; }

		//	Submits what was drawn and reads the pixels
		gtPicture Read() const;

	private:
		gtTarget(const gtTarget&) = delete;
		gtTarget& operator = (const gtTarget&) = delete;

		gtBackend::Kind	m_Kind;
		matTexture*		m_pTexture;
};

//	One mip of one layer (a cube face) of a 2D texture on whichever backend
//	i_Kind is (the one that is up), rows tightly packed (i_BytesPerTexel
//	each), top row first: for targets that aren't RGBA8 (an R32_FLOAT
//	shadow map), and for mips and cube faces. For a 3D texture, every
//	depth slice of the mip, slice after slice (i_Layer unused)
std::vector<uint8_t> gtReadTexture(gtBackend::Kind i_Kind, const matTexture* i_pTexture, int i_BytesPerTexel,
								   int i_Mip = 0, int i_Layer = 0);

//	gtReadTexture's two halves, for textures that aren't a matTexture's
//	main image (the other targets of a reflective shadow map)
std::vector<uint8_t> gtReadD3D11Texture(ID3D11Texture2D* i_pTexture, int i_BytesPerTexel, int i_Mip = 0, int i_Layer = 0);
std::vector<uint8_t> gtReadD3D11Volume(ID3D11Texture3D* i_pTexture, int i_BytesPerTexel, int i_Mip = 0);
std::vector<uint8_t> gtReadVkImage(gvk::vkdImage* i_pImage, int i_Mip = 0, int i_Layer = 0);
