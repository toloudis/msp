// NOTE: THIS COMMENTED CODE IS SUBJECT TO SIGNIFICANT REVISION

/*****************************************************************************
**	captStereoUtil.cpp
**
**		see .hpp
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Support/capt/captStereoUtil.hpp"

#include "Core/Fs/fsLocator.hpp"
#include "Core/ma/maFloatRGBA.hpp"
#include "Graphics/g2d/g2dImage.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"


//--------------------------------------------------------------------
// GetPixelType()
//--------------------------------------------------------------------
int captStereoUtil::GetNumBytesPerPixel(g2dPFD::PixelFormat rendererPFD)
{
	switch ( rendererPFD )
	{
		case g2dPFD::e_Color: 
			return 4;
		case g2dPFD::e_RGBA16f: 
			return 8;
		case g2dPFD::e_RGBA32f: 
			return 16;
		case g2dPFD::e_Float32: 
			return 4;
		default:
			DBG_ERROR( "Unsupported PixelFormat.");
			return 0;
	}
}

//--------------------------------------------------------------------
// setChannelOffsets()
//--------------------------------------------------------------------
void captStereoUtil::setChannelOffsets(g2dPFD::PixelFormat rendererPFD, int * offsets) 
{
	switch ( rendererPFD ) 
	{
		case g2dPFD::e_Color:
			offsets[0] = R_OFFSET_E_COLOR;
			offsets[1] = G_OFFSET_E_COLOR;
			offsets[2] = B_OFFSET_E_COLOR;
			offsets[3] = A_OFFSET_E_COLOR;
			break;
		case g2dPFD::e_RGBA16f:
			offsets[0] = R_OFFSET_RGBAxxf;
			offsets[1] = G_OFFSET_RGBAxxf;
			offsets[2] = B_OFFSET_RGBAxxf;
			offsets[3] = A_OFFSET_RGBAxxf;
			break;
		case g2dPFD::e_RGBA32f:
			offsets[0] = R_OFFSET_RGBAxxf;
			offsets[1] = G_OFFSET_RGBAxxf;
			offsets[2] = B_OFFSET_RGBAxxf;
			offsets[3] = A_OFFSET_RGBAxxf;
			break;
	}
}

//--------------------------------------------------------------------
// GetLeftColorFilter()
//--------------------------------------------------------------------
void captStereoUtil::SetLeftColorFilter(int selection, maFloatRGBA * filter) 
{
	switch(selection)
	{
		case 0:
			*filter = maFloatRGBA(1,0,0,1);
			break;
		case 1:
			*filter = maFloatRGBA(1,0,0,1);
			break;
		case 2:
			*filter = maFloatRGBA(1,0,0,1);
			break;
		case 3:
			*filter = maFloatRGBA(0,1,1,1);
			break;
		case 4:
			*filter = maFloatRGBA(0,0,1,1);
			break;
		case 5:
			*filter = maFloatRGBA(0,1,0,1);
			break;
	}
}

//--------------------------------------------------------------------
// GetRightColorFilter()
//--------------------------------------------------------------------
void captStereoUtil::SetRightColorFilter(int selection, maFloatRGBA * filter) 
{
	switch(selection)
	{
		case 0:
			*filter = maFloatRGBA(0,1,1,1);
			break;
		case 1:
			*filter = maFloatRGBA(0,0,1,1);
			break;
		case 2:
			*filter = maFloatRGBA(0,1,0,1);
			break;
		case 3:
			*filter = maFloatRGBA(1,0,0,1);
			break;
		case 4:
			*filter = maFloatRGBA(1,0,0,1);
			break;
		case 5:
			*filter = maFloatRGBA(1,0,0,1);
			break;
	}	
}

//--------------------------------------------------------------------------------
//	MergeLeftRight()
//--------------------------------------------------------------------------------
void captStereoUtil::MergeLeftRight(g2dImage* ana, g2dImage* left, g2dImage* right, g2dPFD::PixelFormat rendererPFD )
{
	int numBytesPerPixel = GetNumBytesPerPixel( rendererPFD );
	int * offsets = new int[4];
	setChannelOffsets( rendererPFD , offsets );

	// Access bits in pImg and pAnaglyph
	void* leftBits = left->Lock();
	void* rightBits = right->Lock();
	void* anaBits = ana->Lock();
	BYTE* pLeftBits = (BYTE*)leftBits;
	BYTE* pRightBits = (BYTE*)rightBits;
	BYTE* pAnaBits = (BYTE*)anaBits;

	int r_offset = offsets[0];
	int g_offset = offsets[1];
	int b_offset = offsets[2];
	int a_offset = offsets[3];

	int bytesPerChannel = numBytesPerPixel >> 2;

	//Loop through all the pixels
	int i, j;
	for (i = 0 ; i < left->GetHeight() ; i++) 
	{
		for (j = 0 ; j < left->GetWidth() ; j++)
		{
			int idx = (j*numBytesPerPixel) + (i*left->GetStride());

			if ( rendererPFD == g2dPFD::e_Color )
			{
				// Accumulate filtered color onto anaglyph
				pAnaBits[idx]   = pLeftBits[idx] + pRightBits[idx];
				pAnaBits[idx+1] = pLeftBits[idx+1] + pRightBits[idx+1];
				pAnaBits[idx+2] = pLeftBits[idx+2] + pRightBits[idx+2];
				pAnaBits[idx+3] = pLeftBits[idx+3] + pRightBits[idx+3];

			} 
			else if ( rendererPFD == g2dPFD::e_RGBA32f )
			{
				// Apply color filter
				//*(float*)(pLeftBits + idx + r_offset*bytesPerChannel) *= (filter.GetRed());
				//*(float*)(pLeftBits + idx + g_offset*bytesPerChannel) *= (filter.GetGreen());
				//*(float*)(pLeftBits + idx + b_offset*bytesPerChannel) *= (filter.GetBlue());
				//*(float*)(pLeftBits + idx + a_offset*bytesPerChannel) *= (filter.GetAlpha());

				//// Accumulate filtered color onto anaglyph
				//*(float*)(pAnaBits + idx + r_offset*bytesPerChannel) += *(float*)(pLeftBits + idx + r_offset*bytesPerChannel);
				//*(float*)(pAnaBits + idx + g_offset*bytesPerChannel) += *(float*)(pLeftBits + idx + g_offset*bytesPerChannel);
				//*(float*)(pAnaBits + idx + b_offset*bytesPerChannel) += *(float*)(pLeftBits + idx + b_offset*bytesPerChannel);
				//*(float*)(pAnaBits + idx + a_offset*bytesPerChannel) += *(float*)(pLeftBits + idx + a_offset*bytesPerChannel);
			}
			else if ( rendererPFD == g2dPFD::e_RGBA16f )
			{	
				// Apply color filter
				//*(short*)(pLeftBits + idx + r_offset*bytesPerChannel) *= (short)(filter.GetRed());
				//*(short*)(pLeftBits + idx + g_offset*bytesPerChannel) *= (short)(filter.GetGreen());
				//*(short*)(pLeftBits + idx + b_offset*bytesPerChannel) *= (short)(filter.GetBlue());
				//*(short*)(pLeftBits + idx + a_offset*bytesPerChannel) *= (short)(filter.GetAlpha());

				//// Accumulate filtered color onto anaglyph
				//*(short*)(pAnaBits + idx + r_offset*bytesPerChannel) += *(short*)(pLeftBits + idx + r_offset*bytesPerChannel);
				//*(short*)(pAnaBits + idx + g_offset*bytesPerChannel) += *(short*)(pLeftBits + idx + g_offset*bytesPerChannel);
				//*(short*)(pAnaBits + idx + b_offset*bytesPerChannel) += *(short*)(pLeftBits + idx + b_offset*bytesPerChannel);
				//*(short*)(pAnaBits + idx + a_offset*bytesPerChannel) += *(short*)(pLeftBits + idx + a_offset*bytesPerChannel);
			}
		}
	}

	// Unlock surfaces
	left->Release();
	right->Release();
	ana->Release();
	delete offsets;
	offsets = NULL;
}


//--------------------------------------------------------------------------------
//	ApplyAnaglyphColorFilter()
//--------------------------------------------------------------------------------
void captStereoUtil::ApplyAnaglyphColorFilter(g2dImage* orig, const maFloatRGBA& filter, g2dPFD::PixelFormat rendererPFD )
{
	int numBytesPerPixel = GetNumBytesPerPixel( rendererPFD );
	int * offsets = new int[4];
	setChannelOffsets( rendererPFD , offsets );

	// Access bits in pImg and pAnaglyph
	void* imgBits = orig->Lock();
	BYTE* pImgBits = (BYTE*)imgBits;

	int r_offset = offsets[0];
	int g_offset = offsets[1];
	int b_offset = offsets[2];
	int a_offset = offsets[3];

	int bytesPerChannel = numBytesPerPixel >> 2;

	//Loop through all the pixels
	int i, j;
	for (i = 0 ; i < orig->GetHeight() ; i++) 
	{
		for (j = 0 ; j < orig->GetWidth() ; j++)
		{

			int idx = (j*numBytesPerPixel) + (i*orig->GetStride());

			if ( rendererPFD == g2dPFD::e_Color )
			{
				// Apply color filter
				pImgBits[idx+r_offset] *= (BYTE)(filter.GetRed());
				pImgBits[idx+g_offset] *= (BYTE)(filter.GetGreen());
				pImgBits[idx+b_offset] *= (BYTE)(filter.GetBlue());
				pImgBits[idx+a_offset] *= (BYTE)(filter.GetAlpha());
			} 
			else if ( rendererPFD == g2dPFD::e_RGBA32f )
			{
				// Apply color filter
				*(float*)(pImgBits + idx + r_offset*bytesPerChannel) *= (filter.GetRed());
				*(float*)(pImgBits + idx + g_offset*bytesPerChannel) *= (filter.GetGreen());
				*(float*)(pImgBits + idx + b_offset*bytesPerChannel) *= (filter.GetBlue());
				*(float*)(pImgBits + idx + a_offset*bytesPerChannel) *= (filter.GetAlpha());
			}
			else if ( rendererPFD == g2dPFD::e_RGBA16f )
			{	
				// Apply color filter
				*(short*)(pImgBits + idx + r_offset*bytesPerChannel) *= (short)(filter.GetRed());
				*(short*)(pImgBits + idx + g_offset*bytesPerChannel) *= (short)(filter.GetGreen());
				*(short*)(pImgBits + idx + b_offset*bytesPerChannel) *= (short)(filter.GetBlue());
				*(short*)(pImgBits + idx + a_offset*bytesPerChannel) *= (short)(filter.GetAlpha());
			}
		}
	}

	// Unlock surfaces
	orig->Release();
	delete offsets;
	offsets = NULL;
}

//--------------------------------------------------------------------
// SetStereoCamPos()
//--------------------------------------------------------------------
float captStereoUtil::SetStereoCamPos(const maVector3d& origLeftVec, const maPoint3d& origPos, const maPoint3d& origTarget, 
									 const maPoint3d& origUp, camCamera * pCamera, int currentEye) 
{
	float viewportOffset = 0;
	float zero_parallax = pCamera->GetStereoFD();
	float IOD = pCamera->GetStereoIOD();
	maPoint3d newPos = origPos;
	maPoint3d newTarget = origTarget;

	int projectionType = pCamera->GetStereoProjection();

	// Use our own Toe-in method
	if ( projectionType == TOE_IN )
	{
		maVector3d front = origTarget - origPos;
		front.Normalize();

		newPos = origPos;
		newTarget = origPos + front * zero_parallax;
		switch( currentEye )
		{
			case STEREO_RIGHTCAM:
			{
				newPos -= origLeftVec * (IOD*0.5f);
				break;
			}
			case STEREO_LEFTCAM:
			{
				newPos += origLeftVec * (IOD*0.5f);
				break;
			}
		}
	} 

	// Use Paul Bourke's Off-axis method
	// http://local.wasp.uwa.edu.au/~pbourke/miscellaneous/stereographics/stereorender/
	if ( projectionType == OFF_AXIS ) {

		float ndfl = pCamera->GetNearClip() / zero_parallax;

		maVector3d r = origLeftVec*-1;
		r *= (IOD * 0.5f);

		switch( currentEye )
		{
			case STEREO_RIGHTCAM:
			{				
				viewportOffset = (0.5f * IOD * ndfl)*-1;
				
				pCamera->SetHorizontalFilmOffset( viewportOffset );

				newPos = origPos + r;
				newTarget = origTarget + r;
				break;
			}
			case STEREO_LEFTCAM:
			{
				viewportOffset = (0.5f * IOD * ndfl);

				pCamera->SetHorizontalFilmOffset( viewportOffset );

				newPos = origPos - r;
				newTarget = origTarget - r;
				break;
			}
		}
	}

	pCamera->LookAt(newPos,newTarget,origUp);

	return viewportOffset;
}
