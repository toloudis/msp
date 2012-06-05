#pragma once

#ifdef LIGHTWAIT_RESPONSE_HPP
#error LightWaitResponse.hpp multiply included
#endif
#define LIGHTWAIT_RESPONSE_HPP

#include <stdlib.h>
#include <iostream>
#include <queue>
#include <list>


//========================================================================
//========================================================================
using namespace std;



//========================================================================
//========================================================================

class LightWaitImage {
protected:
	string format ;
	int height ;
	int width ;
	string fileName ;
	float fps ;
	std::string m_AnimFile;
	float		m_fAnimStartTime;
	float		m_fAnimLength;
	float		m_OrientationY;
	string camera ;
	float timePoint ;
	string batchName ;
	bool mask ;

public:
	LightWaitImage(char *thefileName,
		char *theformat,
		int thewidth,
		int theheight,
		float fps,
		std::string m_AnimFile,
		float		m_fAnimStartTime,
		float		m_fAnimLength,
		float		m_OrientationY,
		char *thecamera,
		float thetimePoint,
		char *theBatchName,
		bool theMask) ;
	virtual ~LightWaitImage() ;
	virtual string toXML(void) ;
	virtual string toPlain(void) ;
	virtual float getFPS() { return fps ; }
} ;



//========================================================================
//========================================================================

class LightWaitResponse
{
protected:
	string response ;

	long int imageCount ;
	long int maskCount ;

	float renderTime  ;
	float processingTime ;
	float animationLoadingTime ;
	float sceneLoadingTime ;
	float jobProcessingTime ;
	float objectsProcessingTime ;
	float modificationsProcessingTime ;
	float renderRequestProcessingTime ;
	float sceneProcessingTime ;

	string status ;
	list<LightWaitImage *> images ;
	list<LightWaitImage *> masks ;
	string modelName ;
	string batchName ;
	string modelDescriptor ;
	string cameraDescriptor ;

	unsigned long currentVRam;
	unsigned long maxVRam;
	int  SubDivLevel;

public:
	LightWaitResponse(char *themodelName, char *theName) ;
	virtual ~LightWaitResponse() ;	
	virtual string toXML(void) ;
	virtual string toPlain(void) ;
	virtual void setRenderTime(float renderTime) ;
	virtual void setCommandProcessingTime(float renderTime) ;
	virtual void setJobProcessingTime(float processingTime) ;
	virtual void setObjectsProcessingTime(float processingTime) ;
	virtual void setModificationsProcessingTime(float processingTime) ;
	virtual void setSceneProcessingTime(float processingTime) ;
	virtual void setRenderRequestProcessingTime(float processingTime) ;
	virtual void setSceneLoadngTime(float renderTime) ;
	virtual void setAnimationLoadngTime(float renderTime) ;
	virtual void setMemory( unsigned long curmem, unsigned long maxmem );
	virtual void setStatus(char *status) ;
	virtual void setModelDescriptor(string descriptor) ;
	virtual void setCameraDescriptor(string descriptor) ;
	virtual void setSubDivLevel(int level);
	virtual void Clear(void) ;

	virtual void addImage(char *fileName,
		char *format,
		int width,
		int height,
		float fps,
		std::string m_AnimFile,
		float		m_fAnimStartTime,
		float		m_fAnimLength,
		float		m_OrientationY,
		char *camera,
		float timePoint,
		bool mask) ;
} ;
