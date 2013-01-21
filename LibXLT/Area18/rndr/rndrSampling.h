#pragma once

enum eFilterFunc
{
	eFilterBox,
	eFilterGaussian,
	eFilterMitchell,
	eFilterTriangle,
	eFilterSinc,
	eFilterLanczos,
	eFilterBlackmanHarris,
	eFilterCatmullRom,
};

class rndrSampling
{
public:
	rndrSampling(void);
	virtual ~rndrSampling(void);

	// final image in pixels
	int mWidth, mHeight;
	// bucket size in final pixels
	int mTileWidth, mTileHeight;
	// num samples per pixel
	int mPixelSamplesX, mPixelSamplesY;
	// filtering
	eFilterFunc mFilterType;
	float mFilterWidthX, mFilterWidthY;

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	bool operator == (const rndrSampling& i_Other);
	bool operator != (const rndrSampling& i_Other);
};
