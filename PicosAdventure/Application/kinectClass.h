#ifndef _KINECT_CLASS_H_
#define _KINECT_CLASS_H_

#include <windows.h>

#pragma warning( disable : 4005 )

#include <D2D1.h>
#include <xnamath.h>

#include "NuiApi.h"
#include <KinectBackgroundRemoval.h>

#include "inputManager.h"

#include "../Utils/notifierClass.h"
#include "../Utils/listenerClass.h"

#include "../Math/pointClass.h"
#include "../Math/vectorClass.h"

#pragma comment(lib, "d2d1")

class ApplicationManager;

enum messageType
	{
		GREETINGS_KINECT,
		TORSO_POSITION,
		LEFT_HAND_POSITION_ROT,
		RIGHT_HAND_POSITION_ROT,
		HOLD_HANDS
	};

struct KinectStruct
	{
		messageType type;
		Point handPos;
		Point secondJoint;
		Point thirdJoint;	
		float handRot;
		bool boolean;
	};

struct Player
	{
		Vector4 leftHand, rightHand;
		Vector4 leftShoulder, rightShoulder;
		Vector4 rightElbow, leftElbow;
		Vector4 hipCenter;
	};

class KinectClass : public Notifier<KinectClass, KinectStruct>
{
	public:
		KinectClass();
		KinectClass(const KinectClass&);
		~KinectClass();

		bool setup(HWND, IDXGISwapChain*);
		void update();
		void draw();
		void destroy();

	private:
		Point SkeletonToScreen(Vector4 skeletonPoint, int width, int height);
		void detectSekeltonsJoints(NUI_SKELETON_FRAME myFrame);

		HRESULT			ProcessDepth();
		HRESULT			ProcessColor();
		HRESULT			ProcessSkeleton();
		HRESULT			ComposeImage();

		HRESULT         ChooseSkeleton(NUI_SKELETON_DATA* pSkeletonData);

		HWND            hWnd_;
		BOOL			nearMode_;

		INuiSensor*     nuiSensor_;

		ID2D1Factory*   D2DFactory_;
		ID2D1RenderTarget* renderTarget_;
		ID2D1Bitmap*    bitmap_;

		HANDLE			hEvents[4];
		HANDLE          depthStreamHandle_;
		HANDLE          nextDepthFrameEvent_;
		HANDLE          colorStreamHandle_;
		HANDLE          nextColorFrameEvent_;
		HANDLE          nextSkeletonFrameEvent_;
		HANDLE          nextBackgroundRemovedFrameEvent_;

		BYTE*           backgroundRGBX_;
		BYTE*           outputRGBX_;

		UINT            colorWidth_;
		UINT            colorHeight_;
		UINT            depthWidth_;
		UINT            depthHeight_;

		INuiBackgroundRemovedColorStream*  backgroundRemovalStream_;
		
		DWORD           trackedSkeleton_;

		Player players[2];
		int numPlayer;
		bool risedHand; 
		int countSwipe;
		int lastHandPosition,handPosition;
		double handsDist;
		KinectStruct kinectStruct;
};

#endif