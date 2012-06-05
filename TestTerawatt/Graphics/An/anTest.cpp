#include <stdio.h>
#include <windows.h>
#include <ddraw.h>
#include <math.h>

#undef CreateFile
#undef CopyFile
#undef DeleteFile
#undef CreateDirectory
#undef DrawText

#include "an2StateAnimation.hpp"
#include "anKeyAnimation.hpp"
#include "anPackage.hpp"
#include "appApplication.hpp"
#include "appApplicationPAC.hpp"
#include "appCharEvent.hpp"
#include "appCharEventHandler.hpp"
#include "appMouseEvent.hpp"
#include "appMouseEventHandler.hpp"
#include "appFlowEvent.hpp"
#include "appFlowEventHandler.hpp"
#include "appPackage.hpp"
#include "appTime.hpp"
#include "dbgLog.hpp"
#include "dbgPackage.hpp"
#include "envError.hpp"
#include "envPackage.hpp"
#include "fsPackage.hpp"
#include "fsLocator.hpp"
#include "fsFileStream.hpp"
#include "fsFileUtil.hpp"
#include "fsFileX.hpp"
#include "g2dARGBColor.hpp"
#include "g2dFontUtil.hpp"
#include "g2dImage.hpp"
#include "g2dImageDrawUtil.hpp"
#include "g2dImageLoadUtil.hpp"
#include "g2dPackage.hpp"
#include "g2dScreen.hpp"
#include "g2dScreenDrawUtil.hpp"
#include "itLocaleUtil.hpp"
#include "itPackage.hpp"

//====================================================================
//====================================================================
class MyApp	:	public appApplication,
				public appCharEventHandler,
				public appMouseEventHandler,
				public appFlowEventHandler
{
	public:
		
		MyApp();
		~MyApp();

		virtual void Think();

		virtual void ReceiveCharEvent(appCharEvent& i_Event);

		virtual void ReceiveMouseUpEvent(appMouseUpEvent& i_Event);
		virtual void ReceiveMouseDownEvent(appMouseDownEvent& i_Event);
		virtual void ReceiveMouseMoveEvent(appMouseMoveEvent& i_Event);

		virtual void ReceiveStartEvent(appStartEvent& i_Event);
		virtual void ReceiveStopEvent(appStopEvent& i_Event);
		virtual void ReceiveSuspendEvent(appSuspendEvent& i_Event);
		virtual void ReceiveResumeEvent(appResumeEvent& i_Event);

	private:
		
		g2dFontHandle m_FontSmall;
		g2dFontHandle m_FontLarge;
		int m_Red, m_Green, m_Blue;
		std::vector<anAnimation*> m_Animations;
		std::vector<anAnimInstance*> m_AnimInstances;

		std::vector<anTypedAnimation<float>*> m_GraphedAnimations;
};

MyApp::MyApp() 
{
	this->SetWindowSize(100, 100, 320, 240);
	this->SetWindowTitle(itString("Testing animation"));
	m_Red = m_Green = m_Blue = 0;

	an2StateAnimation<float>* new_anim1;
	new_anim1 = new an2StateAnimation<float>(0, 255, 2.2f);
	new_anim1->SetLooping(true);
	new_anim1->SetReversing(true);
	m_Animations.push_back(new_anim1);

	anAnimation* clone = new_anim1->Clone();

/*	anKeyAnimation<float>* new_anim = new anKeyAnimation<float>(0);
	new_anim->AddKey(3.0, 125);
	new_anim->AddKey(9.0, 125);
	new_anim->AddKey(12.0, 255);
	new_anim->SetLooping(true);
	new_anim->SetReversing(true);
	m_Animations.push_back(new_anim);
*/
	anKeyAnimation<float>* new_anim2 = new anKeyAnimation<float>(0);
	new_anim2->AddKey(1.0, 255);
	new_anim2->SetLooping(true);
	new_anim2->SetReversing(true);
	m_Animations.push_back(new_anim2);

	anKeyAnimation<float>* new_anim3 = new anKeyAnimation<float>(0);
	new_anim3->AddKey(5.0, 50);
	new_anim3->SetLooping(true);
	new_anim3->SetReversing(true);
	m_Animations.push_back(new_anim3);

	//	non-smooth animations
	anKeyAnimation<float>* graph_anim1 = new anKeyAnimation<float>(0);
	graph_anim1->AddKey(50.0, 50);
	graph_anim1->SetLooping(false);
	graph_anim1->SetReversing(false);
	m_GraphedAnimations.push_back(graph_anim1);

	//
	anKeyAnimation<float>* graph_anim2 = new anKeyAnimation<float>(0);
	graph_anim2->AddKey(50.0, 50);
	graph_anim2->SetLooping(false);
	graph_anim2->SetReversing(true);
	m_GraphedAnimations.push_back(graph_anim2);

	//
	anKeyAnimation<float>* graph_anim3 = new anKeyAnimation<float>(0);
	graph_anim3->AddKey(50.0, 50);
	graph_anim3->SetLooping(true);
	graph_anim3->SetReversing(false);
	m_GraphedAnimations.push_back(graph_anim3);

	//
	anKeyAnimation<float>* graph_anim4 = new anKeyAnimation<float>(0);
	graph_anim4->AddKey(50.0, 50);
	graph_anim4->AddKey(10.0, 15);
	graph_anim4->AddKey(25.0, 25);
	graph_anim4->AddKey(40.0, 45);
	graph_anim4->SetLooping(true);
	graph_anim4->SetReversing(true);
	m_GraphedAnimations.push_back(graph_anim4);
	
	//
	anKeyAnimation<float>* graph_anim5 = dynamic_cast<anKeyAnimation<float>*>(graph_anim4->Clone());
	graph_anim5->Rescale(0.5f);
	m_GraphedAnimations.push_back(graph_anim5);

	//
	anKeyAnimation<float>* graph_anim6 = dynamic_cast<anKeyAnimation<float>*>(graph_anim3->Clone());
	graph_anim6->Rescale(1.5f);
	m_GraphedAnimations.push_back(graph_anim6);
}

MyApp::~MyApp() 
{
}

void MyApp::Think()
{
	g2dScreenDrawUtil::Clear(g2dRGBColor(0x40, 0x70, 0x90));

	//	draw some animation graphs
	//	first the non-smooth versions
	//
	int i;
	int num = m_GraphedAnimations.size() / 2;
	for( i = 0 ; i < num ; i++ )
	{
		const anTypedAnimation<float>& cur_anim = *(m_GraphedAnimations[i]);
		const int by = i * 100 + 80;
		const int bx = 30;
	
		//	Draw the graph lines - x and y axes
		//
//		g2dScreenDrawUtil::DrawLine(bx, by, bx + 200, by, g2dRGBColor(0, 0, 0), false);
//		g2dScreenDrawUtil::DrawLine(bx, by - 30, bx, by + 30, g2dRGBColor(0, 0, 0), false);

		int param;									
		for( param = 0 ; param < 200 ; param ++ )
		{
			float val = cur_anim.GetValue(float(param));

			int y = by - int(val);

//			g2dScreenDrawUtil::DrawPixel(bx + param, y, g2dRGBColor(0xff, 0xff, 0xff));
		}
	}	

	num = m_GraphedAnimations.size();
	for( i = (num / 2) ; i < num ; i++ )
	{
		const anTypedAnimation<float>& cur_anim = *(m_GraphedAnimations[i]);
		const int by = (i - num/2) * 100 + 80;
		const int bx = 300;
	
		//	Draw the graph lines - x and y axes
		//
//		g2dScreenDrawUtil::DrawLine(bx, by, bx + 200, by, g2dRGBColor(0, 0, 0), false);
//		g2dScreenDrawUtil::DrawLine(bx, by - 30, bx, by + 30, g2dRGBColor(0, 0, 0), false);

		int param;									
		for( param = 0 ; param < 200 ; param ++ )
		{
			float val = cur_anim.GetValue(float(param));

			int y = by - int(val);

	//		g2dScreenDrawUtil::DrawPixel(bx + param, y, g2dRGBColor(0xff, 0xff, 0xff));
		}
	}	

	//	draw some text to show color animation	
	//
	itString text("This text has an animating color");
	int red, green, blue;

	float time = appTime::GetTime();
	int num_anims = m_AnimInstances.size();
	for( i = 0 ; i < num_anims ; i++ )
	{
		anAnimInstance* instance = m_AnimInstances[i];
		
		anTypedAnimInstance<float>* typed_instance;
		typed_instance = dynamic_cast<anTypedAnimInstance<float>* >(instance);

		switch( i )
		{
			case 0:
				red = (int) typed_instance->GetValue(time);
			break;
		
			case 1:
				green = (int) typed_instance->GetValue(time);
			break;

			case 2: 
				blue = (int) typed_instance->GetValue(time);
			break;
		}
	}

	g2dScreenDrawUtil::DrawText(10, 400, m_FontLarge, text, g2dRGBColor(red, green, blue));
	g2dScreen::EndScene();
}

void MyApp::ReceiveCharEvent(appCharEvent& i_Event)
{
	if( i_Event.GetChar() == itString::CharType('q') )
		Exit();
}

void MyApp::ReceiveMouseUpEvent(appMouseUpEvent& i_Event)
{
}

void MyApp::ReceiveMouseDownEvent(appMouseDownEvent& i_Event)
{
}

void MyApp::ReceiveMouseMoveEvent(appMouseMoveEvent& i_Event)
{
}

void MyApp::ReceiveStartEvent(appStartEvent& i_Event)
{
	DBG_LOG0("Start Event");

	g2dScreen::InitializeWindow(640, 480, 100, 100);
//	g2dScreen::InitializeFullScreen(1024, 768, 16);

	m_FontSmall = g2dFontUtil::LoadFont(itString("Arial Unicode MS"), 16);
	m_FontLarge = g2dFontUtil::LoadFont(itString("Arial Unicode MS"), 64);

	float time = appTime::GetTime();
	int i;
	int num_anims = m_Animations.size();
	for( i = 0 ; i < num_anims ; i++ )
		m_AnimInstances.push_back(m_Animations[i]->CreateAnimInstance(time));
}

void MyApp::ReceiveStopEvent(appStopEvent& i_Event)
{
	DBG_LOG0("Stop Event");

	g2dFontUtil::ReleaseFont(m_FontSmall);
	g2dFontUtil::ReleaseFont(m_FontLarge);
	g2dScreen::DeInitialize();
}

void MyApp::ReceiveSuspendEvent(appSuspendEvent& i_Event)
{
	DBG_LOG0("Suspend Event");
}

void MyApp::ReceiveResumeEvent(appResumeEvent& i_Event)
{
	DBG_LOG0("Resume Event");
}

//====================================================================
//====================================================================
int WINAPI WinMain(
					HINSTANCE hInstance,      // handle to current instance
					HINSTANCE hPrevInstance,  // handle to previous instance
					LPSTR lpCmdLine,          // command line
					int nCmdShow)             // show state
{
	appApplicationPAC::SetHINSTANCE(hInstance);

	envPackage::Init();
	dbgPackage::Init();
	fsPackage::Init();
	itPackage::Init();
	appPackage::Init();
	anPackage::Init();
	g2dPackage::Init();

	MyApp app;

	app.Run();
	
	g2dPackage::CleanUp();
	anPackage::CleanUp();
	appPackage::CleanUp();
	itPackage::CleanUp();
	fsPackage::CleanUp();
	dbgPackage::CleanUp();
	envPackage::CleanUp();
	return 13;
}
