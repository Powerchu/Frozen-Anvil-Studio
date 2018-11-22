/* HEADER *********************************************************************************/
/*!
\file	EHotkey.h
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _EHOTKEY_H_
#define _EHOTKEY_H_
#include "DataStructure/Array.h"
#include "System/Input/InputMap.h"

enum eHKState
{
	eHK_HOLD,
	eHK_RELEASED,
	eHK_TRIGGERED,
	eHK_NEGATE
};

namespace Editor
{
	template<unsigned N>
	class Hotkey
	{
	public:

		Hotkey(const Array<eButton, N>&, const Array<eHKState, N>&);
		Array<eButton, N> mArrKeys;
		Array<eHKState, N> mArrStates;
	};
}

template<unsigned N>
Editor::Hotkey<N>::Hotkey(const Array<eButton, N>& _keys, const Array<eHKState, N>& _states)
	: mArrKeys{ _keys }, mArrStates{ _states }
{
}
#endif
#endif 



