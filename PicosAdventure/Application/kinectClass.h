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

struct KinectStruct
	{
		Point handPos;
		Point elbowPos;
		float handRot;
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

		// TRACKING HAND POSITION
		Vector4 _handRightCoord;
		Point _handRightScreenCoord;
};

#endif