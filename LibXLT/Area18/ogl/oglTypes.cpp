#pragma once

#include "oglTypes.hpp"

//#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"OpenCL.lib")

#include "Graphics/G2d/g2dPFD.hpp"

std::string GetGLError(GLenum err)
{
	switch(err) {
	case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
	case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
	case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
	case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
	}

	return "unknown error code";
}
std::string GetGLFramebufferStatus(GLenum err)
{
	switch(err) {
	case GL_FRAMEBUFFER_UNDEFINED: return "GL_FRAMEBUFFER_UNDEFINED";
	case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
	case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
	case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER: return "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";
	case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER: return "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
	case GL_FRAMEBUFFER_UNSUPPORTED: return "GL_FRAMEBUFFER_UNSUPPORTED";
	case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: return "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
	case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS: return "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";
	}

	return "unknown framebuffer status code";
}

int ogl::bitsPerPixel(GLenum gltype)
{
	switch (gltype) 
	{
	case GL_RGBA32F: return 32*4;
	};
	return 0;
}

void ogl::PFDFromGLFormat(GLenum i_Format, g2dPFD& o_PFD)
{
	// set up some basic guess first
	o_PFD.SetPixelFormat(g2dPFD::e_Color);
	o_PFD.SetBitsPerPixel(ogl::bitsPerPixel( i_Format ));

	// now look for formats we want to specifically recognize
	switch( i_Format )
	{
		case GL_RGBA:
		case GL_RGBA8:	//GL_RGBA	8	8	8	8	 
		case GL_RGBA8_SNORM:	//GL_RGBA	s8	s8	s8	s8	 
			o_PFD.Set(	0,	8,
						8,	8,
						16,	8,
						24,	8,
						32);
		break;
		//// TODO: this is a hack, the best approximation we've come up w/ for this
		//// NOTE: this means that all DXT3 files are assumed to have alpha in them,
		////	whether they really do or not
		//// DXT2 and DXT4 are the same as DXT3 and DXT5, except that they are 
		//// interpreted as having premultiplied alpha in the rgb colors. 
		//// We don't really support premultiplied alpha in our shading system.
		//case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:  // fall through
		//case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:  // fall through
		//	o_PFD.Set(	16,	8,
		//				8,	8,
		//				0,	8,
		//				0,	0,
		//	// DXT compression results in 4 bpp storage.
		//				4);
		//	break;
		//case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:  // fall through
		//case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
		//	o_PFD.Set(	16,	8,
		//				8,	8,
		//				0,	8,
		//				24,	8,
		//	// DXT compression results in 8 bpp storage.
		//				8);
		//	break;
		case GL_RG16_SNORM:	//GL_RG	s16	s16	 	 	 
			o_PFD.SetPixelFormat( g2dPFD::e_BumpMapV16U16 );
			o_PFD.SetBitsPerPixel(32);
			break;
		case GL_R8:	//GL_RED	8	 	 	 	 
			o_PFD.SetPixelFormat( g2dPFD::e_Luminance8 );
			o_PFD.SetBitsPerPixel(8);
			break;
		case GL_DEPTH_COMPONENT16:
			o_PFD.SetPixelFormat( g2dPFD::e_Depth16 );
			o_PFD.SetBitsPerPixel(16);
			break;
		case GL_R32F:	//GL_RED	f32	 	 	 	 
			o_PFD.SetPixelFormat( g2dPFD::e_Float32 );
			o_PFD.SetBitsPerPixel(32);
			break;
		case GL_R16F:	//GL_RED	f16	 	 	 	 
			o_PFD.SetPixelFormat( g2dPFD::e_Float16 );
			o_PFD.SetBitsPerPixel(16);
			break;
		case GL_RGBA16F:	//GL_RGBA	f16	f16	f16	f16	 
			o_PFD.SetPixelFormat( g2dPFD::e_RGBA16f );
			o_PFD.SetBitsPerPixel(64);
			o_PFD.Set(	0,	16,
						16,	16,
						32,	16,
						48,	16,
						64);
			break;
		case GL_RGBA16:	//GL_RGBA	16	16	16	16	 
			o_PFD.SetPixelFormat( g2dPFD::e_RGBA16UInt );
			o_PFD.SetBitsPerPixel(64);
			o_PFD.Set(	0,	16,
						16,	16,
						32,	16,
						48,	16,
						64);
			break;
		case GL_RGBA32F:	//GL_RGBA	f32	f32	f32	f32	 
			o_PFD.SetPixelFormat( g2dPFD::e_RGBA32f );
			o_PFD.SetBitsPerPixel(128);
			break;

		case GL_RG32F:	//GL_RG	f32	f32	 	 	 
			o_PFD.SetPixelFormat( g2dPFD::e_GR32f );
			o_PFD.SetBitsPerPixel(64);
			break;

		case GL_DEPTH24_STENCIL8:
			o_PFD.SetPixelFormat( g2dPFD::e_Depth24Stencil8 );
			o_PFD.SetBitsPerPixel(32);
			break;

		case GL_DEPTH_COMPONENT32F:
			o_PFD.SetPixelFormat(g2dPFD::e_Depth32f);
			o_PFD.SetBitsPerPixel(32);
			break;

		case GL_RGB8:	//GL_RGB	8	8	8	 	 
			o_PFD.SetPixelFormat( g2dPFD::e_RColor );
			o_PFD.SetBitsPerPixel(32);
			break;


		case GL_DEPTH_COMPONENT:	//Depth	D
		case GL_DEPTH_STENCIL:	//Depth, Stencil	D, S
		case GL_RED:	//Red	R
		case GL_RG:	//Red, Green	R, G
		case GL_RGB:	//Red, Green, Blue	R, G, B
		case GL_R8_SNORM:	//GL_RED	s8	 	 	 	 
		case GL_R16:	//GL_RED	16	 	 	 	 
		case GL_R16_SNORM:	//GL_RED	s16	 	 	 	 
		case GL_RG8:	//GL_RG	8	8	 	 	 
		case GL_RG8_SNORM:	//GL_RG	s8	s8	 	 	 
		case GL_RG16:	//GL_RG	16	16	 	 	 
		case GL_R3_G3_B2:	//GL_RGB	3	3	2	 	 
		case GL_RGB4:	//GL_RGB	4	4	4	 	 
		case GL_RGB5:	//GL_RGB	5	5	5	 	 
		case GL_RGB8_SNORM:	//GL_RGB	s8	s8	s8	 	 
		case GL_RGB10:	//GL_RGB	10	10	10	 	 
		case GL_RGB12:	//GL_RGB	12	12	12	 	 
		case GL_RGB16_SNORM:	//GL_RGB	16	16	16	 	 
		case GL_RGBA2:	//GL_RGB	2	2	2	2	 
		case GL_RGBA4:	//GL_RGB	4	4	4	4	 
		case GL_RGB5_A1:	//GL_RGBA	5	5	5	1	 
		case GL_RGB10_A2:	//GL_RGBA	10	10	10	2	 
		case GL_RGB10_A2UI:	//GL_RGBA	ui10	ui10	ui10	ui2	 
		case GL_RGBA12:	//GL_RGBA	12	12	12	12	 
		case GL_SRGB8:	//GL_RGB	8	8	8	 	 
		case GL_SRGB8_ALPHA8:	//GL_RGBA	8	8	8	8	 
		case GL_RG16F:	//GL_RG	f16	f16	 	 	 
		case GL_RGB16F:	//GL_RGB	f16	f16	f16	 	 
		case GL_RGB32F:	//GL_RGB	f32	f32	f32	 	 
		case GL_R11F_G11F_B10F:	//GL_RGB	f11	f11	f10	 	 
		case GL_RGB9_E5:	//GL_RGB	9	9	9	 	5
		case GL_R8I:	//GL_RED	i8	 	 	 	 
		case GL_R8UI:	//GL_RED	ui8	 	 	 	 
		case GL_R16I:	//GL_RED	i16	 	 	 	 
		case GL_R16UI:	//GL_RED	ui16	 	 	 	 
		case GL_R32I:	//GL_RED	i32	 	 	 	 
		case GL_R32UI:	//GL_RED	ui32	 	 	 	 
		case GL_RG8I:	//GL_RG	i8	i8	 	 	 
		case GL_RG8UI:	//GL_RG	ui8	ui8	 	 	 
		case GL_RG16I:	//GL_RG	i16	i16	 	 	 
		case GL_RG16UI:	//GL_RG	ui16	ui16	 	 	 
		case GL_RG32I:	//GL_RG	i32	i32	 	 	 
		case GL_RG32UI:	//GL_RG	ui32	ui32	 	 	 
		case GL_RGB8I:	//GL_RGB	i8	i8	i8	 	 
		case GL_RGB8UI:	//GL_RGB	ui8	ui8	ui8	 	 
		case GL_RGB16I:	//GL_RGB	i16	i16	i16	 	 
		case GL_RGB16UI:	//GL_RGB	ui16	ui16	ui16	 	 
		case GL_RGB32I:	//GL_RGB	i32	i32	i32	 	 
		case GL_RGB32UI:	//GL_RGB	ui32	ui32	ui32	 	 
		case GL_RGBA8I:	//GL_RGBA	i8	i8	i8	i8	 
		case GL_RGBA8UI:	//GL_RGBA	ui8	ui8	ui8	ui8	 
		case GL_RGBA16I:	//GL_RGBA	i16	i16	i16	i16	 
		case GL_RGBA16UI:	//GL_RGBA	ui16	ui16	ui16	ui16	 
		case GL_RGBA32I:	//GL_RGBA	i32	i32	i32	i32	 
		case GL_RGBA32UI:
		case GL_COMPRESSED_RED_RGTC1:	//GL_RED	Specific
		case GL_COMPRESSED_SIGNED_RED_RGTC1:	//GL_RED	Specific
		case GL_COMPRESSED_RG_RGTC2:	//GL_RG	Specific
		case GL_COMPRESSED_SIGNED_RG_RGTC2:	//GL_RG	Specific
		case GL_COMPRESSED_RGBA_BPTC_UNORM_ARB:	//GL_RGBA	Specific
		case GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM_ARB:	//GL_RGBA	Specific
		case GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT_ARB:	//GL_RGB	Specific
		case GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT_ARB:	//GL_RGB
		default:
//BC4	GL_COMPRESSED_RED_RGTC1	[0..1]
//BC4	GL_COMPRESSED_SIGNED_RED_RGTC1	[-1..1]
//BC5	GL_COMPRESSED_RED_GREEN_RGTC2	[0..1]
//BC5	GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2	[-1..1]
//BC6H	GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT	
//BC6H	GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT	
//BC7	GL_COMPRESSED_RGBA_BPTC_UNORM	
//BC7	GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM	







			o_PFD.SetPixelFormat( g2dPFD::e_Unknown );
			DBG_WARNING("PFDFromGLFormat: Unsupported GL texure format " << i_Format);
		break;
	}
}
GLenum ogl::GLFormatFromPFD(const g2dPFD& i_PFD)
{
	int format = i_PFD.GetPixelFormat();

	if ( format == g2dPFD::e_Color )
	{
		switch( i_PFD.BitsPerPixel() )
		{
			case 32:
				return GL_RGBA8;
			break;
		}
	}
	if ( format == g2dPFD::e_RColor )
	{
		switch( i_PFD.BitsPerPixel() )
		{
		case 32:
			return GL_RGBA8;
			break;
		}
	}
	else if ( format == g2dPFD::e_BumpMapV8U8 )
	{
		return 	GL_RG8_SNORM;
	}
	else if ( format == g2dPFD::e_BumpMapV16U16 )
	{
		return GL_RG16_SNORM;
	}
	else if (format == g2dPFD::e_RGBA16f )
	{
		return GL_RGBA16F;
	}
	else if (format == g2dPFD::e_RGBA16UInt)
	{
		return GL_RGBA16;
	}
	else if (format == g2dPFD::e_RGBA32f )
	{
		return GL_RGBA32F;
	}
	else if (format == g2dPFD::e_GR32f )
	{
		return GL_RG32F;
	}
	else if (format == g2dPFD::e_Float16 )
	{
		return GL_R16F;
	}
	else if (format == g2dPFD::e_Float32 )
	{
		return GL_R32F;
	}
	else if (format == g2dPFD::e_Luminance8 )
	{
		return GL_R8;
	}
	

	DBG_ERROR("Unknown g2dPFD format " << format);
	return GL_INVALID_VALUE;
}
