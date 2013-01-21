#pragma once

#include <boost/scoped_ptr.hpp>
#include <vector> 
class oglDevice;
class oglTexture2d;
class maFloatRGBA;
class shdrVS;
class shdrPS;
class shdrPipeline;
struct sth_stash;

class oglTextDraw
{
public:
	oglTextDraw(oglDevice* iDevice);
	virtual ~oglTextDraw(void);

	void Draw(const std::string& s, int x, int y, float scale, const maFloatRGBA& color);
private:
	struct sth_stash* stash;
	int droidRegular;

	shdrVS* mVS;
	shdrPS* mPS;
	shdrPipeline* mPipeline;
};

