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

XGamePad::XGamePad(const unsigned _id, float maxTimer)
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
	mfDeadZoneL{ XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE + 100.F}, 
	mfDeadZoneR{ XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE }, 
	mfTriggerThresh{ XINPUT_GAMEPAD_TRIGGER_THRESHOLD },
	mfMaxThumbVal{ 32767 },
	mpxVibrate{ new XINPUT_VIBRATION{} },
	mfMaxTimer(maxTimer),
	mfTimer(0.0f)
{
}

XGamePad::~XGamePad(void)
{
	delete mpxState;
	delete mpxVibrate;
}

void XGamePad::PollInputs(void)
{
	const DWORD prevPacket = mpxState->dwPacketNumber;
	ZeroMemory(&*mpxState, sizeof(XINPUT_STATE));
	auto res = XInputGetState(mdwID, &*mpxState);
	mbConnected = res == ERROR_SUCCESS;
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
	const float LX = mpxState->Gamepad.sThumbLX;
	const float LY = mpxState->Gamepad.sThumbLY;
	float magnitude = sqrt(LX*LX + LY*LY);
	const float normLX = LX / magnitude;
	const float normLY = LY / magnitude;
	float normMag;

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
	const float RX = mpxState->Gamepad.sThumbRX;
	const float RY = mpxState->Gamepad.sThumbRY;
	float magnitude = sqrt(RX*RX + RY*RY);
	const float normRX = RX / magnitude;
	const float normRY = RY / magnitude;
	float normMag;

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
		const bool pressed = msButtons & g_ControllerHexa[Index];
		auto& btn = mArrXBtnStates[Index];

		btn.mbTriggered = pressed && !btn.mbPressed;
		btn.mbReleased  = !pressed && btn.mbPressed;
		btn.mbPressed   = pressed;
	}
}

void XGamePad::Vibrate(unsigned short _speedL, unsigned short _speedR) const
{
	//0-65534
	if (mbConnected)
	{
		ZeroMemory(&*mpxVibrate, sizeof(XINPUT_VIBRATION));
		mpxVibrate->wLeftMotorSpeed	= _speedL;
		mpxVibrate->wRightMotorSpeed	= _speedR;
		XInputSetState(mdwID, &*mpxVibrate);
	}
}

void XGamePad::StopVibrate(void) const
{
	Vibrate(0, 0);
}

void XGamePad::SetVibrationSetting(float intensity, float maxTime, float bal)
{
	if (intensity > 1.f) intensity = 1.f;
	else if (intensity < 0.f) intensity = 0.f;

	if (maxTime > mfMaxTimer) mfMaxTimer = maxTime;

	mfTimer = mfMaxTimer;
	mfIntensity = intensity;
	mfBalance = bal;
}

void XGamePad::VibrateHelper()
{
	if (mfIntensity <= 0 || mfTimer <= 0) return;

	//0-65534
	if (mfBalance > 1.f) mfBalance = 1.f;
	else if (mfBalance < -1.f) mfBalance = -1.f;

	const unsigned maxFreq = 65535;
	const unsigned _lowFreq = static_cast<unsigned>((1.f - (mfBalance + 1.f)) * (static_cast<float>(maxFreq) / 2) + (static_cast<float>(maxFreq) / 2));
	const unsigned _highFreq = maxFreq - _lowFreq;

	if (mfTimer > 0.f)
	{
		Vibrate( static_cast<unsigned short>( mfIntensity * _lowFreq * (mfTimer/mfMaxTimer)), static_cast<unsigned short>(mfIntensity * _highFreq * (mfTimer / mfMaxTimer)));
	}
	else
	{
		mfTimer = 0.0f;
		mfIntensity = 0.0f;
		StopVibrate();
	}
}

float XGamePad::GetAnalogX(int _i) const
{
	/*Note - Shnannon I change this (Keith) 28/10/2018*/
	return (!_i) ? mxLeftThumb[0].mfNormal  * mxLeftThumb[0].mfMagnitudeNormal:
				   mxRightThumb[0].mfNormal * mxRightThumb[0].mfMagnitudeNormal;
}

float XGamePad::GetAnalogY(int _i) const
{
	/*Note - Shnannon I change this (Keith) 28/10/2018*/
	return (!_i) ? mxLeftThumb[1].mfNormal  * mxLeftThumb[1].mfMagnitudeNormal:
				   mxRightThumb[1].mfNormal * mxRightThumb[1].mfMagnitudeNormal;
}

float XGamePad::GetTriggers(int _i) const
{
	return (!_i) ? float(mcTrigger[0] / 255.f) : float(mcTrigger[1] / 255.f);
}

bool XGamePad::IsKeyPressed(eButton _btn) const
{
	if (_btn > eXBUTTON_LAST) return false;
	return mArrXBtnStates[_btn].mbPressed;
}

bool XGamePad::IsKeyTriggered(eButton _btn) const
{
	if (_btn > eXBUTTON_LAST) return false;
	return mArrXBtnStates[_btn].mbTriggered;
}

bool XGamePad::IsKeyReleased(eButton _btn) const
{
	if (_btn > eXBUTTON_LAST) return false;
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
