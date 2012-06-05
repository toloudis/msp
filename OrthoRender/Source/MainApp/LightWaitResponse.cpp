#include "stdafx.h"
#include "MainApp/LightWaitResponse.hpp"
#include "Core/Env/envSTLHelpers.hpp"

#include <queue>
#include <ctime>
#include <windows.h>


//========================================================================
//========================================================================

LightWaitImage::LightWaitImage(char *thefileName,
	char *theformat,
	int thewidth,
	int theheight,
	float theFps,
	std::string theAnimFile,
	float theAnimStartTime,
	float theAnimLength,
	float theOrientationY,
	char *thecamera,
	float thetimePoint,
	char *theBatchName,
	bool theMask) {
		format = theformat ;
		width = thewidth ;
		height = theheight ;
		fps = theFps ;
		m_AnimFile = theAnimFile;
		m_fAnimStartTime= theAnimStartTime;
		m_fAnimLength= theAnimLength;
		m_OrientationY= theOrientationY;
		fileName = thefileName ;
		timePoint = thetimePoint ;
		camera = thecamera ;
		batchName = theBatchName ;
		mask = theMask ;
}

LightWaitImage::~LightWaitImage() {
}

string LightWaitImage::toXML(void) {
	string outString ;
	if (mask) {
		outString.append("<Mask>") ;
	}
	else {
		outString.append("<Image>") ;
	}
	
	char tempStr[256] ;

	outString.append("<AnimationTime>") ;
	sprintf(tempStr,"%f",timePoint) ;
	outString.append(tempStr) ;
	outString.append("</AnimationTime>") ;
	
	outString.append("<FileName>") ;
	outString.append(fileName) ;
	outString.append("</FileName>") ;

	outString.append("<Format>") ;
	outString.append(format) ;
	outString.append("</Format>") ;

	outString.append("<Width>") ;
	sprintf(tempStr,"%d",width) ;
	outString.append(tempStr) ;
	outString.append("</Width>") ;

	outString.append("<Height>") ;
	sprintf(tempStr,"%d",height) ;
	outString.append(tempStr) ;
	outString.append("</Height>") ;

	if (m_fAnimLength > 0.0) {
		outString.append("<Animation>") ;
			outString.append("<Name>") ;
			sprintf(tempStr,"%s",m_AnimFile.c_str()) ;
			outString.append(tempStr) ;
			outString.append("</Name>") ;

			outString.append("<StartTime>") ;
			sprintf(tempStr,"%f",m_fAnimStartTime) ;
			outString.append(tempStr) ;
			outString.append("</StartTime>") ;

			outString.append("<Length>") ;
			sprintf(tempStr,"%f",m_fAnimLength) ;
			outString.append(tempStr) ;
			outString.append("</Length>") ;

			outString.append("<Orientation>") ;
			sprintf(tempStr,"%.0f", m_OrientationY * (180.0/3.1415926)) ;
			outString.append(tempStr) ;
			outString.append("</Orientation>") ;

			outString.append("<FPS>") ;
			sprintf(tempStr,"%f",fps) ;
			outString.append(tempStr) ;
			outString.append("</FPS>") ;
		outString.append("</Animation>") ;
	}

	outString.append("<Camera>") ;
	outString.append(camera) ;
	outString.append("</Camera>") ;
	
	if (mask) {
		outString.append("</Mask>") ;
	}
	else {
		outString.append("</Image>") ;
	}

	return outString ;
}


string LightWaitImage::toPlain(void) {
	string outString ;

	outString.append(fileName) ;
	outString.append(" ") ;

	outString.append(camera) ;
	outString.append(" ") ;

	outString.append("NAME \"") ;
	outString.append(batchName) ;
	outString.append("\"") ;

	return outString ;
}



//========================================================================
//========================================================================

LightWaitResponse::LightWaitResponse(char *themodelName, char *theName)
{
	response.clear() ;
	modelName = themodelName ;
	batchName = theName ;
	SubDivLevel = 0;
}

LightWaitResponse::~LightWaitResponse()
{
	envSTLHelpers::DeleteContainer( images );
	envSTLHelpers::DeleteContainer( masks );
}


void LightWaitResponse::setModelDescriptor(string descriptor) {
	modelDescriptor = descriptor ;
}

void LightWaitResponse::setCameraDescriptor(string cameras) {
	cameraDescriptor = cameras ;
}


void LightWaitResponse::Clear(void) {
	response.clear() ;
	envSTLHelpers::DeleteContainer( masks );
	envSTLHelpers::DeleteContainer( images );
//	masks.clear() ;
//	images.clear() ;
	imageCount = 0 ;
	maskCount = 0 ;
}

void LightWaitResponse::setSubDivLevel(int level)
{
	SubDivLevel = level;
}

string LightWaitResponse::toXML(void) {
	response.append("<?xml version=\"1.0\" encoding=\"UTF-8\"?>") ;
	
	response.append("<RenderOutput>") ;

	char tempStr[256] ;
	
	response.append("<Status>") ;
	response.append(status) ;
	response.append("</Status>") ;

	//response.append("<ModelName>") ;
	//response.append(modelName) ;
	//response.append("</ModelName>") ;

	response.append("<Name>") ;
	response.append(batchName) ;
	response.append("</Name>") ;

	response.append("<RenderRequestTimeDate>") ;
	time_t rawtime;
	tm * ptm;
	time ( &rawtime );
	ptm = gmtime ( &rawtime );
	sprintf (tempStr,"%2d/%02d/%04d %2d:%02d:%02d GMT", 
		ptm->tm_mon, ptm->tm_mday, 1900 + ptm->tm_year, 
		ptm->tm_hour,ptm->tm_min,ptm->tm_sec);
	response.append(tempStr) ;
	response.append("</RenderRequestTimeDate>") ;

	response.append("<RendererStatus>") ;

	response.append("<RendererBuildDate>") ;
	response.append(__TIMESTAMP__) ;
	response.append("</RendererBuildDate>") ;

		response.append("<ProcessingTime>") ;
			response.append("<JobTime>") ;
			sprintf(tempStr,"%f",processingTime) ;
			response.append(tempStr) ;
			response.append("</JobTime>") ;

			response.append("<RenderTime>") ;
			sprintf(tempStr,"%f",renderTime) ;
			response.append(tempStr) ;
			response.append("</RenderTime>") ;

			response.append("<AnimationLoadTime>") ;
			sprintf(tempStr,"%f",animationLoadingTime) ;
			response.append(tempStr) ;
			response.append("</AnimationLoadTime>") ;

			response.append("<SceneLoadTime>") ;
			sprintf(tempStr,"%f",sceneLoadingTime) ;
			response.append(tempStr) ;
			response.append("</SceneLoadTime>") ;

			response.append("<JobProcessingTime>") ;
			sprintf(tempStr,"%f",jobProcessingTime) ;
			response.append(tempStr) ;
			response.append("</JobProcessingTime>") ;

			response.append("<ObjectsProcessingTime>") ;
			sprintf(tempStr,"%f",objectsProcessingTime) ;
			response.append(tempStr) ;
			response.append("</ObjectsProcessingTime>") ;

			response.append("<ModificationsProcessingTime>") ;
			sprintf(tempStr,"%f",modificationsProcessingTime) ;
			response.append(tempStr) ;
			response.append("</ModificationsProcessingTime>") ;

			response.append("<RenderRequestProcessingTime>") ;
			sprintf(tempStr,"%f",renderRequestProcessingTime) ;
			response.append(tempStr) ;
			response.append("</RenderRequestProcessingTime>") ;

			response.append("<SceneProcessingTime>") ;
			sprintf(tempStr,"%f",sceneProcessingTime) ;
			response.append(tempStr) ;
			response.append("</SceneProcessingTime>") ;
		response.append("</ProcessingTime>") ;

		response.append("<MemoryStatus>") ;
			MEMORYSTATUS stat;
			GlobalMemoryStatus (&stat);

			sprintf(tempStr,"<VideoMemoryUsage>%lu</VideoMemoryUsage>", maxVRam - currentVRam );
			response.append(tempStr) ;
			sprintf(tempStr,"<VideoMemoryFree>%lu</VideoMemoryFree>", currentVRam );
			response.append(tempStr) ;
			sprintf(tempStr,"<VideoMemorySize>%lu</VideoMemorySize>", maxVRam );
			response.append(tempStr) ;

			response.append("<MemoryLoad>") ;
			sprintf(tempStr,"%ld",stat.dwMemoryLoad) ;
			response.append(tempStr) ;
			response.append("</MemoryLoad>") ;

			response.append("<PhysicalMemorySize>") ;
			sprintf(tempStr,"%ld",stat.dwTotalPhys) ;
			response.append(tempStr) ;
			response.append("</PhysicalMemorySize>") ;

			response.append("<PhysicalMemoryFree>") ;
			sprintf(tempStr,"%ld",stat.dwAvailPhys) ;
			response.append(tempStr) ;
			response.append("</PhysicalMemoryFree>") ;

			response.append("<PagingMemorySize>") ;
			sprintf(tempStr,"%ld",stat.dwTotalPageFile) ;
			response.append(tempStr) ;
			response.append("</PagingMemorySize>") ;

			response.append("<PagingMemoryFree>") ;
			sprintf(tempStr,"%ld",stat.dwAvailPageFile) ;
			response.append(tempStr) ;
			response.append("</PagingMemoryFree>") ;

			response.append("<VirtualMemorySize>") ;
			sprintf(tempStr,"%ld",stat.dwTotalVirtual) ;
			response.append(tempStr) ;
			response.append("</VirtualMemorySize>") ;

			response.append("<VirtualMemoryFree>") ;
			sprintf(tempStr,"%ld",stat.dwAvailVirtual) ;
			response.append(tempStr) ;
			response.append("</VirtualMemoryFree>") ;
		response.append("</MemoryStatus>") ;

		response.append("<SubDivLevel>");
		sprintf(tempStr,"%d",SubDivLevel);
		response.append(tempStr);
		response.append("</SubDivLevel>");
	response.append("</RendererStatus>");

	response.append("<Masks>") ;

	response.append("<MaskCount>") ;
	sprintf(tempStr,"%d",masks.size()) ;
	response.append(tempStr) ;
	response.append("</MaskCount>") ;

	list<LightWaitImage *>::iterator maskIterator;
	for ( maskIterator = masks.begin() ; maskIterator != masks.end(); maskIterator++ ) {
		response.append((*maskIterator)->toXML()) ;
	}
	response.append("</Masks>") ;


	response.append("<Images>") ;

	response.append("<ImageCount>") ;
	sprintf(tempStr,"%d",images.size()) ;
	response.append(tempStr) ;
	response.append("</ImageCount>") ;

	list<LightWaitImage *>::iterator imageIterator;
	for ( imageIterator = images.begin() ; imageIterator != images.end(); imageIterator++ ) {
		response.append((*imageIterator)->toXML()) ;
	}
	response.append("</Images>") ;


	response.append(cameraDescriptor) ;

	response.append(modelDescriptor) ;

	response.append("</RenderOutput>") ;

	return response ;
}

string LightWaitResponse::toPlain(void) {
	response.clear() ;
	
	list<LightWaitImage *>::iterator imageIterator;
	for ( imageIterator = images.begin() ; imageIterator != images.end(); imageIterator++ ) {
		response.append((*imageIterator)->toPlain()) ;
	}

	list<LightWaitImage *>::iterator maskIterator;
	for ( maskIterator = masks.begin() ; maskIterator != masks.end(); maskIterator++ ) {
		response.append((*maskIterator)->toPlain()) ;
	}

	return response ;
}


void LightWaitResponse::setRenderTime(float theRenderTime) {
	renderTime = theRenderTime ;
}

void LightWaitResponse::setStatus(char *theStatus) {
	status = theStatus ;
}

void LightWaitResponse::setMemory( unsigned long curmem, unsigned long maxmem )
{
	currentVRam = maxmem - curmem;
	maxVRam = maxmem;
}

void LightWaitResponse::setCommandProcessingTime(float processingTime) {
	this->processingTime = processingTime ;
}


void LightWaitResponse::setJobProcessingTime(float processingTime) {
	this->jobProcessingTime = processingTime ;
}


void LightWaitResponse::setSceneProcessingTime(float processingTime) {
	this->sceneProcessingTime = processingTime ;
}

void LightWaitResponse::setRenderRequestProcessingTime(float processingTime) {
	this->renderRequestProcessingTime = processingTime ;
}

void LightWaitResponse::setModificationsProcessingTime(float processingTime) {
	this->modificationsProcessingTime = processingTime ;
}

void LightWaitResponse::setObjectsProcessingTime(float processingTime) {
	this->objectsProcessingTime = processingTime ;
}


void LightWaitResponse::setSceneLoadngTime(float loadingTime) {
	this->sceneLoadingTime = loadingTime ;
}


void LightWaitResponse::setAnimationLoadngTime(float loadingTime) {
	this->animationLoadingTime = loadingTime ;
}


void LightWaitResponse::addImage(char *fileName,
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
	bool mask) {
		if (mask) {
			masks.push_back(new LightWaitImage(fileName,
			format,
			width,
			height,
			fps,
			m_AnimFile,
			m_fAnimStartTime,
			m_fAnimLength,
			m_OrientationY,
			camera,
			timePoint,
			(char *) batchName.c_str(),
			mask)) ;
		} 
		else {
			images.push_back(new LightWaitImage(fileName,
			format,
			width,
			height,
			fps,
			m_AnimFile,
			m_fAnimStartTime,
			m_fAnimLength,
			m_OrientationY,
			camera,
			timePoint,
			(char *) batchName.c_str(),
			mask)) ;
		}
}

