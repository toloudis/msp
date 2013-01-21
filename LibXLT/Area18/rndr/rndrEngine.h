#pragma once
class rndrEngine
{
public:
	rndrEngine(void);
	virtual ~rndrEngine(void);

	virtual void resize(int w, int h) {mWidth = w; mHeight = h; doResize(w,h);}
	virtual void draw() = 0;
	// auto redraw ?
	virtual bool animated() { return false; }

	int width() const {return mWidth;}
	int height() const {return mHeight;}
private:
	virtual void doResize(int w, int h) {}
	int mWidth, mHeight;
};

