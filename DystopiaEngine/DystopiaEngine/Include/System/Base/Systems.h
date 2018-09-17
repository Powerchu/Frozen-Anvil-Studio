/* HEADER *********************************************************************************/
/*!
\file	Systems.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Contains the Systems public interface the program core uses.
	Please update SystemTypes.h when adding a new system

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _SYSTEMS_H_
#define _SYSTEMS_H_

namespace Dystopia
{
	enum class eSysMessage;
	using DysSerialiser_t = class TextSerialiser;

	class Systems
	{
	public:
		virtual void PreInit(void)					    { };
		virtual bool Init(void)						    = 0;
		virtual void PostInit(void)					    { };
													    
		virtual void FixedUpdate(float)				    { };
		virtual void Update(float)					    = 0;
		virtual void PostUpdate(void)				    { };
		virtual void Shutdown(void)					    = 0;
													    
		virtual void LoadDefaults(void)				    { };
		virtual void LoadSettings(DysSerialiser_t&)	    { };
		virtual void SaveSettings(DysSerialiser_t&)     { };

		virtual void ReceiveMessage(const eSysMessage&) { };

		virtual ~Systems(void)						    = default;

	};
}



#endif		// INCLUDE GUARD

