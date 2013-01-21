#include "rndrSampling.h"


rndrSampling::rndrSampling(void)
:	mWidth(1), mHeight(1),
	mTileWidth(1), mTileHeight(1),
	mPixelSamplesX(1), mPixelSamplesY(1),
	mFilterType(eFilterBox),
	mFilterWidthX(1), mFilterWidthY(1)
{
}


rndrSampling::~rndrSampling(void)
{
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
bool rndrSampling::operator == (const rndrSampling& i_Other)
{
	return (
		(mWidth == i_Other.mWidth) &&
		(mHeight == i_Other.mHeight) &&
		(mTileWidth == i_Other.mTileWidth) &&
		(mTileHeight == i_Other.mTileHeight) &&
		(mPixelSamplesX == i_Other.mPixelSamplesX) &&
		(mPixelSamplesY == i_Other.mPixelSamplesY) &&
		(mFilterType == i_Other.mFilterType) &&
		(mFilterWidthX == i_Other.mFilterWidthX) &&
		(mFilterWidthY == i_Other.mFilterWidthY)
	);
}
bool rndrSampling::operator != (const rndrSampling& i_Other)
{
	return !(this->operator==(i_Other));
}
