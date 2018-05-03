/* HEADER *********************************************************************************/
/*!
\file	AudioTypes.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Definitions for SoundSystem.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _AUDIOTYPES_H_
#define _AUDIOTYPES_H_

enum eSoundType
{
	AUDIOTYPE_BGM,
	AUDIOTYPE_EFFECT,
	AUDIOTYPE_MENU,
	
	// Special Values
	AUDIOTYPE_MASTER,
	AUDIOTYPE_TOTAL = AUDIOTYPE_MASTER,
};



#endif		// INCLUDE GUARD

