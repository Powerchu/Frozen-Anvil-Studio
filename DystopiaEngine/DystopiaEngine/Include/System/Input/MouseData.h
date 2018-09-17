/* HEADER *********************************************************************************/
/*!
\file	InputMap.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Definitions for InputManager. If you need a key that is not here, let me know.
	Since we want to avoid littering windows header everywhere,
	we use raw values instead of the defines.

	It is okay to add more into eUserButton yourself.
	It is NOT okay to add into eButton. Let me know.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _MOUSEDATA_H_
#define _MOUSEDATA_H_

namespace Dystopia
{
	struct MouseData
	{
		int mnWheel;
		float mfDeltaX, mfDeltaY;
	};
}



#endif		// INCLUDE GUARD

