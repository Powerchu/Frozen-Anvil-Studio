/* HEADER *********************************************************************************/
/*!
\file	SystemMessage.h
\author Tan Jie Wei Jacky (0%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Messages systems can send and recieve

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _SYSTEM_MESSAGES_H_
#define _SYSTEM_MESSAGES_H_


namespace Dystopia
{
	enum class eSysMessage
	{
		FOCUS_GAIN,
		FOCUS_LOST,
		MAXIMIZE,
		MINIMIZE,
		ACTIVATE,
		QUIT,
	};
}



#endif		// INCLUDE GUARD

