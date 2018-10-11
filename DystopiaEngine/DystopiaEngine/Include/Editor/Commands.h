/* HEADER *********************************************************************************/
/*!
\file	Commands.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _COMMANDS_H_
#define _COMMANDS_H_
#include "Editor/CommandList.h"
#include <deque>

namespace Dystopia
{
	// Command handler to record all commands made in the editor. *Intended only for editor class to use*
	class CommandHandler
	{
	public:
		CommandHandler(size_t _nHistory=20);
		~CommandHandler();

		bool HasUnsavedChanges() const;

		void UndoCommand();
		void RedoCommand();

		void EndRecording();
		bool IsRecording() const;

		void SaveCallback();

		void Shutdown();

		void InvokeCommand(Commands *_comd);
		void InvokeCommandInsert(GameObject&, Scene&, bool *_notify = nullptr);
		void InvokeCommandInsert(const AutoArray<GameObject*>&, Scene&, bool *_notify = nullptr);
		void InvokeCommandDelete(GameObject&, Scene&, bool *_notify = nullptr);
		void InvokeCommandDelete(const AutoArray<GameObject*>&, Scene&, bool *_notify = nullptr);

		template<class Component, typename T>
		void InvokeCommand(const uint64_t& _id, T* _var, const T& _oldVal, bool *_notify = nullptr)
		{
			InvokeCommand(new ComdModifyValue<T, Component, void>{ _id, _var, _oldVal, _notify });
		}
		template<class Component, typename T>
		void InvokeCommand(T* _var, const T& _oldVal, bool *_notify = nullptr)
		{
			static constexpr uint64_t invalidID = 0;
			InvokeCommand(new ComdModifyValue<T, Component, void>{ invalidID, _var, _oldVal, _notify });
		}

		template<class C, typename ... Params>
		void InvokeCommand(const uint64_t & _id,
						   const FunctionModWrapper<C, Params ...>& _old, 
						   const FunctionModWrapper<C, Params ...>& _new)
		{
			using FMW = FunctionModWrapper<C, Params ...>;
			InvokeCommand(new ComdModifyComponent<FMW, FMW>{ _id, _new, _old });
		}

		template<class C, typename ... Params>
		auto Make_FunctionModWrapper(void(C::*_ptrFunc)(Params ...), const std::remove_reference_t<Params>& ... _vals)
		{
			using FMW = FunctionModWrapper<C, Params...>;
			return FMW{ _ptrFunc, _vals... };
		}

		template<class C, typename T>
		void StartRecording(const uint64_t& _id, T* _target, bool *_notify = nullptr)
		{ 
			if (mRecording) return;
			mpRecorder = new ComdRecord<T, C, void>(_id, _target, _notify);
			mRecording = true;
		}

		template<class C, typename T>
		void StartRecording(T* _target, bool *_notify = nullptr)
		{
			if (mRecording) return;
			static constexpr uint64_t invalidID = 0;
			mpRecorder = new ComdRecord<T, C, void>(invalidID, _target, _notify);
			mRecording = true;
		}

		std::deque<Commands*>& GetDeqRedo(void);
		std::deque<Commands*>& GetDeqUndo(void);

	private:
		RecordBase				*mpRecorder;
		std::deque<Commands*>	mDeqRedo;
		std::deque<Commands*>	mDeqUndo;
		bool					mRecording;
		bool					mUnsavedChanges;
		void					PopFrontOfDeque(std::deque<Commands*>&);
		void					RemoveStray(std::deque<Commands*>&);
		size_t					mMaxSize;
	};
}

#endif // _COMMANDS_H_
#endif // EDITOR



