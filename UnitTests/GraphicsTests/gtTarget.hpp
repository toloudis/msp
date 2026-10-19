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
