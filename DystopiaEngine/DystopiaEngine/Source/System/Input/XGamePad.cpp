#include <Windows.h>
#include <Xinput.h>
#if(_WIN32_WINNT >= _WIN32_WINNT_WIN8)
#pragma comment(lib, "XINPUT9_1_0.lib")
#else
#pragma comment(lib, "XINPUT9_1_0.lib")
#endif

#include "System/Input/XGamePad.h"
#include "Math/MathLib.h"

static constexpr unsigned short g_ControllerHexa[14] = 
{
	XINPUT_GAMEPAD_DPAD_UP,          
	XINPUT_GAMEPAD_DPAD_DOWN,       
	XINPUT_GAMEPAD_DPAD_LEFT,        
	XINPUT_GAMEPAD_DPAD_RIGHT,      
	XINPUT_GAMEPAD_START,            
	XINPUT_GAMEPAD_BACK,            
	XINPUT_GAMEPAD_LEFT_THUMB,       
	XINPUT_GAMEPAD_RIGHT_THUMB,      
	XINPUT_GAMEPAD_LEFT_SHOULDER,    
	XINPUT_GAMEPAD_RIGHT_SHOULDER,   
	XINPUT_GAMEPAD_A,                
	XINPUT_GAMEPAD_B,                
	XINPUT_GAMEPAD_X,                
	XINPUT_GAMEPAD_Y                
};

XGamePad::XGamePad(const unsigned _id)
	:
	mArrXBtnStates{},
	mxLeftThumb{},
	mxRightThumb{},
	mcTrigger{},
	msButtons{},
	mdwID{ static_cast<DWORD>(_id) },
	mpxState{ new XINPUT_STATE{} },
	mbConnected{ false },
	mbChangeDetected{ false },
	mfDeadZoneL{ XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE }, 
	mfDeadZoneR{ XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE }, 
	mfTriggerThresh{ XINPUT_GAMEPAD_TRIGGER_THRESHOLD },
	mfMaxThumbVal{ 32767 },
	mpxVibrate{ new XINPUT_VIBRATION{} }
{
}

XGamePad::~XGamePad(void)
{
	delete mpxState;
	delete mpxVibrate;
}

void XGamePad::PollInputs(void)
{
	DWORD prevPacket = mpxState->dwPacketNumber;
	ZeroMemory(&*mpxState, sizeof(XINPUT_STATE));
	mbConnected = XInputGetState(mdwID, &*mpxState) == ERROR_SUCCESS;
	mbChangeDetected = prevPacket != mpxState->dwPacketNumber;
	if (mbConnected)
	{
		UpdateLeftThumb();
		UpdateRightThumb();
		UpdateTriggers();
		UpdateButtons();
	}
}

void XGamePad::UpdateLeftThumb(void)
{
	float LX = mpxState->Gamepad.sThumbLX;
	float LY = mpxState->Gamepad.sThumbLY;
	float magnitude = sqrt(LX*LX + LY*LY);
	float normLX = LX / magnitude;
	float normLY = LY / magnitude;
	float normMag = 0;

	if (magnitude > mfDeadZoneL)
	{
		if (magnitude > mfMaxThumbVal) magnitude = mfMaxThumbVal;

		magnitude -= mfDeadZoneL;
		normMag = magnitude / (mfMaxThumbVal - mfDeadZoneL);
	}
	else
	{
		magnitude = 0;
		normMag = 0;
	}

	mxLeftThumb[0].mfActualVal			= LX;
	mxLeftThumb[0].mfNormal				= normLX;
	mxLeftThumb[0].mfMagnitude			= magnitude;
	mxLeftThumb[0].mfMagnitudeNormal	= normMag;

	mxLeftThumb[1].mfActualVal			= LY;
	mxLeftThumb[1].mfNormal				= normLY;
	mxLeftThumb[1].mfMagnitude			= magnitude;
	mxLeftThumb[1].mfMagnitudeNormal	= normMag;
}

void XGamePad::UpdateRightThumb(void)
{
	float RX = mpxState->Gamepad.sThumbRX;
	float RY = mpxState->Gamepad.sThumbRY;
	float magnitude = sqrt(RX*RX + RY*RY);
	float normRX = RX / magnitude;
	float normRY = RY / magnitude;
	float normMag = 0;

	if (magnitude > mfDeadZoneR)
	{
		if (magnitude > mfMaxThumbVal) magnitude = mfMaxThumbVal;

		magnitude -= mfDeadZoneR;
		normMag = magnitude / (mfMaxThumbVal - mfDeadZoneR);
	}
	else
	{
		magnitude = 0;
		normMag = 0;
	}

	mxRightThumb[0].mfActualVal			= RX;
	mxRightThumb[0].mfNormal			= normRX;
	mxRightThumb[0].mfMagnitude			= magnitude;
	mxRightThumb[0].mfMagnitudeNormal	= normMag;

	mxRightThumb[1].mfActualVal			= RY;
	mxRightThumb[1].mfNormal			= normRY;
	mxRightThumb[1].mfMagnitude			= magnitude;
	mxRightThumb[1].mfMagnitudeNormal	= normMag;
}

void XGamePad::UpdateTriggers(void)
{
	unsigned char lTrig = mpxState->Gamepad.bLeftTrigger;
	unsigned char rTrig = mpxState->Gamepad.bRightTrigger;

	if (static_cast<float>(lTrig) < mfTriggerThresh)
		lTrig = 0;
	
	if (static_cast<float>(rTrig) < mfTriggerThresh)
		rTrig = 0;

	mcTrigger[0] = lTrig;
	mcTrigger[1] = rTrig;
}

void XGamePad::UpdateButtons(void)
{
	msButtons = mpxState->Gamepad.wButtons;
	for (unsigned i = eButton::XBUTTON_DPAD_UP, Index = 0; i < eButton::XBUTTON_LAST; ++Index,++i)
	{
		bool pressed = msButtons & g_ControllerHexa[Index];
		auto& btn = mArrXBtnStates[Index];

		btn.mbTriggered = pressed && !btn.mbPressed;
		btn.mbReleased  = !pressed && btn.mbPressed;
		btn.mbPressed   = pressed;
	}
}

void XGamePad::Vibrate(unsigned short _speedL, unsigned short _speedR)
{
	if (mbConnected)
	{
		ZeroMemory(&*mpxVibrate, sizeof(XINPUT_VIBRATION));
		mpxVibrate->wLeftMotorSpeed	= _speedL;
		mpxVibrate->wRightMotorSpeed	= _speedR;
		XInputSetState(mdwID, &*mpxVibrate);
	}
}

void XGamePad::StopVibrate(void)
{
	Vibrate(0, 0);
}

float XGamePad::GetAnalogX(int _i) const
{
	return (!_i) ? mxLeftThumb[0].mfNormal : 
				   mxRightThumb[0].mfNormal ;
}

float XGamePad::GetAnalogY(int _i) const
{
	return (!_i) ? mxLeftThumb[1].mfNormal :
				   mxRightThumb[1].mfNormal ;
}

float XGamePad::GetTriggers(int _i) const
{
	return (!_i) ? mcTrigger[0] : mcTrigger[1];
}

bool XGamePad::IsKeyPressed(eButton _btn) const
{
	return mArrXBtnStates[_btn].mbPressed;
}

bool XGamePad::IsKeyTriggered(eButton _btn) const
{
	return mArrXBtnStates[_btn].mbTriggered;
}

bool XGamePad::IsKeyReleased(eButton _btn) const
{
	return mArrXBtnStates[_btn].mbReleased;
}

bool XGamePad::HasChanged(void) const
{
	return mbChangeDetected;
}

bool XGamePad::IsConnected(void) const
{
	return mbConnected;
}
