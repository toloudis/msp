#pragma once
class eventHandler
{
public:
	eventHandler(void);
	virtual ~eventHandler(void);

	enum Button { LBTN = 1, RBTN = 2, MBTN = 4 };
	virtual void mouseClick(int x, int y, unsigned int buttons) {}
	virtual void mouseRelease(int x, int y, unsigned int buttons) {}
	virtual void mouseMove(int x, int y, unsigned int buttons) {}
	virtual void mouseDblClick(int x, int y, unsigned int buttons) {}
};

