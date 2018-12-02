/* HEADER *********************************************************************************/
/*!
\file	EditorStatesTypes.h
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _EDITOR_STATES_TYPES_H_
#define _EDITOR_STATES_TYPES_H_

namespace Editor
{
	enum eState : unsigned
	{
		LAUNCHER = 0,
		MAIN,
		PLAY,
		PAUSED,
		EXIT
	};
}

#endif 
#endif




