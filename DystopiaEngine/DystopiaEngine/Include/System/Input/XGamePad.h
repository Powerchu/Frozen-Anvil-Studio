#ifndef _XGAMEPAD_H_
#define _XGAMEPAD_H_
#include <string>
#include "System/Input/InputMap.h"
enum eXButtons : unsigned
{
	eXBUTTON_DPAD_UP = 0,          
	eXBUTTON_DPAD_DOWN,      
	eXBUTTON_DPAD_LEFT,        
	eXBUTTON_DPAD_RIGHT,       
	eXBUTTON_START,            
	eXBUTTON_BACK,             
	eXBUTTON_LEFT_THUMB,       
	eXBUTTON_RIGHT_THUMB,      
	eXBUTTON_LEFT_SHOULDER,    
	eXBUTTON_RIGHT_SHOULDER,   
	eXBUTTON_A,                
	eXBUTTON_B,                
	eXBUTTON_X,                
	eXBUTTON_Y,

	eXBUTTON_LAST
}; 

typedef struct _XINPUT_STATE XINPUT_STATE, *PXINPUT_STATE;
typedef struct _XINPUT_VIBRATION XINPUT_VIBRATION, *PXINPUT_VIBRATION;
typedef unsigned long DWORD;

class XGamePad
{
public:
	XGamePad(unsigned _id);
	~XGamePad(void);

	void PollInputs(void);

	bool IsConnected(void) const;
	bool HasChanged(void) const;

	// left is low frequency motor, right is high frequency motor
	void Vibrate(unsigned short _speedL = 32000, unsigned short _speedR = 16000); //0-65534
	void StopVibrate(void);

	bool IsKeyPressed(eButton) const;
	bool IsKeyTriggered(eButton) const;
	bool IsKeyReleased(eButton) const;

	float GetAnalogY(int) const;	//0 for left analog, all others for right
	float GetAnalogX(int) const;	//0 for left analog, all others for right

	float GetTriggers(int) const;	// 0 for left trigger, all others for right
									// values returned are between: mfTriggerThresh ~ 255

private:
	void UpdateLeftThumb(void);
	void UpdateRightThumb(void);
	void UpdateTriggers(void);
	void UpdateButtons(void);

	struct XThumbStick
	{
		float mfActualVal;
		float mfNormal;
		float mfMagnitude;
		float mfMagnitudeNormal;
	};

	struct XBtnStates
	{
		bool mbTriggered = false;
		bool mbPressed = false;
		bool mbReleased = false;
	};

	XBtnStates		mArrXBtnStates[eXBUTTON_LAST];
	XThumbStick		mxLeftThumb[2];		// [0] = x, [1] = y
	XThumbStick		mxRightThumb[2];	// [0] = x, [1] = y
	unsigned char	mcTrigger[2];		// [0] = Left, [1] = Right , val 0 - 255

	unsigned short	msButtons;

	DWORD			mdwID;
	XINPUT_STATE	*mpxState;
	bool			mbConnected;
	bool			mbChangeDetected;
	float			mfDeadZoneL;		//0-65534
	float			mfDeadZoneR;		//0-65534
	float			mfTriggerThresh;	//30
	float			mfMaxThumbVal;		//32767

	XINPUT_VIBRATION *mpxVibrate;
};


#endif //_XGAMEPAD_H_














