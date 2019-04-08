/* HEADER *********************************************************************************/
/*!
\file	PostProcessSys.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Post Processing System

All Content Copyright © 2019 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _POSTPROCESS_SYS_H_
#define _POSTPROCESS_SYS_H_

#include "System/Base/Systems.h"		 // System
#include "System/Base/ComponentDonor.h"


namespace Dystopia
{
	class Framebuffer;

	class PostProcessSystem : public Systems
	{
	public:

		bool Init(void) override;
		void PostInit(void) override;

		void Update(float) override;

		void ApplyBlur(void);

		void Shutdown(void) override;

		void LoadDefaults(void) override;
		void LoadSettings(DysSerialiser_t&) override;
		void SaveSettings(DysSerialiser_t&) override;


	private:

		unsigned mSettings;
		Framebuffer* first;
		Framebuffer* second;
		Framebuffer* third;
		Framebuffer* game;
	};
}



#endif		// INCLUDE GUARD

