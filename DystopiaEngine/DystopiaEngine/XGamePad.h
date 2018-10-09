#ifndef _XGAMEPAD_H_
#define _XGAMEPAD_H_
#include <Windows.h>
#include <Xinput.h>
#include <string>
#pragma comment(lib, "XInput.lib")

struct XThumbStick
{
	float mfActualVal;
	float mfNormal;
	float mfMagnitude;
	float mfMagnitudeNormal;
};

class XGamePad
{
public:
	XGamePad(unsigned _id);
	~XGamePad(void);

	void PollInputs(void);
	void Vibrate(unsigned short _speedL = 32000, unsigned short _speedR = 16000); //0-65534
	bool IsConnected(void) const;

	std::string GetPrint(void) const;

private:
	void			UpdateLeftThumb(void);
	void			UpdateRightThumb(void);
	void			UpdateTriggers(void);
	void			UpdateButtons(void);

	XThumbStick		mxLeftThumb[2];		// [0] = x, [1] = y
	XThumbStick		mxRightThumb[2];	// [0] = x, [1] = y
	unsigned char	mcTrigger[2];		// [0] = Left, [1] = Right , val 0 - 255
	unsigned short	msButtons;

	DWORD			mdwID;
	XINPUT_STATE	mxState;
	bool			mbConnected;
	float			mfDeadZoneL;		//0-65534
	float			mfDeadZoneR;		//0-65534
	float			mfMaxThumbVal;		//32767
	float			mfTriggerThresh;	//30
	XINPUT_VIBRATION mxVibrate;
};


#endif //_XGAMEPAD_H_














