#include "XGamePad.h"
#include "Math/MathLib.h"

XGamePad::XGamePad(unsigned _id)
	: mdwID{ static_cast<DWORD>(_id) },
	mbConnected{ false }, 
	mxState{},
	mfDeadZoneL{ XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE }, 
	mfDeadZoneR{ XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE }, 
	mfMaxThumbVal{ 32767 },
	mfTriggerThresh{ XINPUT_GAMEPAD_TRIGGER_THRESHOLD },
	mxVibrate{},
	mxLeftThumb{},
	mxRightThumb{},
	mcTrigger{},
	msButtons{}
{
}

XGamePad::~XGamePad(void)
{}

void XGamePad::PollInputs(void)
{
	ZeroMemory(&mxState, sizeof(XINPUT_STATE));
	mbConnected = (XInputGetState(mdwID, &mxState) == ERROR_SUCCESS) ? true : false ;

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
	float LX = mxState.Gamepad.sThumbLX;
	float LY = mxState.Gamepad.sThumbLY;
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
	float RX = mxState.Gamepad.sThumbRX;
	float RY = mxState.Gamepad.sThumbRY;
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
	unsigned char lTrig = mxState.Gamepad.bLeftTrigger;
	unsigned char rTrig = mxState.Gamepad.bRightTrigger;

	if (static_cast<float>(lTrig) < mfTriggerThresh)
		lTrig = 0;
	
	if (static_cast<float>(rTrig) < mfTriggerThresh)
		rTrig = 0;

	mcTrigger[0] = lTrig;
	mcTrigger[1] = rTrig;
}

void XGamePad::UpdateButtons(void)
{
	msButtons = mxState.Gamepad.wButtons;
}

void XGamePad::Vibrate(unsigned short _speedL, unsigned short _speedR)
{
	if (mbConnected)
	{
		ZeroMemory(&mxVibrate, sizeof(XINPUT_VIBRATION));
		mxVibrate.wLeftMotorSpeed	= _speedL;
		mxVibrate.wRightMotorSpeed	= _speedR;
		XInputSetState(mdwID, &mxVibrate);
	}
}

std::string XGamePad::GetPrint(void) const
{
	std::string printout = "";

	printout += "Left Thumb X	: " + std::to_string(mxLeftThumb[0].mfMagnitudeNormal);
	printout += "\n";
	printout += "Left Thumb Y	: " + std::to_string(mxLeftThumb[1].mfMagnitudeNormal);
	printout += "\n";
	printout += "Right Thumb X	: " + std::to_string(mxRightThumb[0].mfMagnitudeNormal);
	printout += "\n";
	printout += "Right Thumb Y	: " + std::to_string(mxRightThumb[1].mfMagnitudeNormal);
	printout += "\n";
	printout += "Trigger L		: " + std::to_string(mcTrigger[0]);
	printout += "\n";
	printout += "Trigger R		: " + std::to_string(mcTrigger[1]);
	printout += "\n";
	printout += "Buttons		: " + std::to_string(msButtons);

	return printout;
}

bool XGamePad::IsConnected(void) const
{
	return mbConnected;
}
