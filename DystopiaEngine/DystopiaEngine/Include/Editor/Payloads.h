/* HEADER *********************************************************************************/
/*!
\file	Payloads.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

If you need anything specificly, lemme know and I will create a user-friendly one for you.
Else, just hack it - include imgui.h and use more functions available at your own risk.

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _PAYLOADS_H_
#define _PAYLOADS_H_

namespace EGUI
{
	enum ePayloadTags
	{
		FILE,
		PNG,
		BMP,
		GAMEOBJECT,
		COMPONENT,

		ePAY_LOAD_LAST
	};

	inline const char* GetPayloadString(ePayloadTags _tag)
	{
		switch (_tag)
		{
			case FILE:			return "FILE";
			case PNG:			return "PNG";
			case BMP:			return "BMP";
			case GAMEOBJECT:	return "GAMEOBJECT";
			case COMPONENT:		return "COMPONENT";
			default:			return "ePAY_LOAD_LAST";
		}
	}
}

#endif	// _PAYLOADS_H_
#endif	// EDITOR

