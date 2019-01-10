/* HEADER *********************************************************************************/
/*!
\file	EInput.h
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _E_INPUT_H_
#define _E_INPUT_H_
#include "DataStructure/AutoArray.h"

#include "Editor/EditorSystem.h"
#include "Editor/EHotkey.h"

namespace Dystopia
{
	class InputManager;
}

namespace Editor
{
	class EInput : public EditorSystem
	{
	public:
		EInput(void);
		~EInput(void);

		void Load(void) override;
		bool Init(void) override;
		void StartFrame(void) override;
		void Update(float) override;
		void EndFrame(void) override;
		void Shutdown(void) override;
		void Message(eEMessage) override;
		void SaveSettings(Dystopia::TextSerialiser& _out) const override;
		void LoadSettings(Dystopia::TextSerialiser& _in) override;

		template<unsigned N>
		size_t RegisterHotkey(Hotkey<N> _hotkey);
		//Array<eButton, 2> key = {eButton::KEYBOARD_CTRL, eButton::KEYBOARD_S};
		//Array<eHKState, 2> state = {eHKState::eHK_HOLD, eHKState::eHK_TRIGGERED};
		//Editor::Hotkey<2> save{key, state};
		//size_t myHKID = p->RegisterHotkey(save);
		bool IsHotkeyTriggered(size_t _id) const;

		Dystopia::InputManager * const GetInputManager(void) const;

		class HKBase
		{
		public:
			virtual bool Occured(Dystopia::InputManager *) const = 0;
			virtual ~HKBase(void) {}
		};

		template<unsigned N>
		class HKDerived : public HKBase
		{
		public:
			HKDerived(const Array<eButton, N>& , const Array<eHKState, N>&);
			~HKDerived(void) {}

			bool Occured(Dystopia::InputManager *) const;

			Array<eButton, N> mArrKeys;
			Array<eHKState, N> mArrStates;
		};

	private:
		AutoArray<HKBase*> mArrHotkeyPtrs;
		Dystopia::InputManager *mpInputMgr;
	};
}

template<unsigned N>
size_t Editor::EInput::RegisterHotkey(Hotkey<N> _hotkey)
{
	size_t index = mArrHotkeyPtrs.size();
	HKBase* pHK = Dystopia::DefaultAllocator<Editor::EInput::HKDerived<N>>::ConstructAlloc(_hotkey.mArrKeys, _hotkey.mArrStates);
	mArrHotkeyPtrs.Insert(pHK);
	return index;
}

template<unsigned N>
Editor::EInput::HKDerived<N>::HKDerived(const Array<eButton, N>& _btns, const Array<eHKState, N>& _states)
	: mArrKeys{ _btns }, mArrStates{ _states }
{}

template<unsigned N>
bool Editor::EInput::HKDerived<N>::Occured(Dystopia::InputManager *mpInputMgr) const
{
	for (unsigned i = 0; i < N; ++i)
	{
		switch (mArrStates[i])
		{
		case eHK_HOLD:
			if (!mpInputMgr->GetKey(mArrKeys[i]))
				return false;
			break;
		case eHK_RELEASED:
			if (!mpInputMgr->GetKeyUp(mArrKeys[i]))
				return false;
			break;
		case eHK_TRIGGERED:
			if (!mpInputMgr->GetKeyDown(mArrKeys[i]))
				return false;
			break;
		case eHK_NEGATE:
			if (mpInputMgr->GetKeyDown(mArrKeys[i]) ||
				mpInputMgr->GetKeyUp(mArrKeys[i]) ||
				mpInputMgr->GetKey(mArrKeys[i]))
				return false;
			break;
		}
	}
	return true;
}
#endif //_E_INPUT_H_
#endif





