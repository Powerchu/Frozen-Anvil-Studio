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
		// Constructs with a fixed size	maximum history of commands recorded).
		CommandHandler(size_t _nHistory=20);

		// Destructor
		~CommandHandler();

		// has unsaved changes
		bool HasUnsavedChanges() const;

		// shutdown
		void Shutdown();

		// Calls the ExecuteDo function of the param command and passes it into the undo deque, also empties the redo deque
		void InvokeCommand(Commands *_comd);
		
		//template<typename T>
		//void InvokeCommand(T * const _var, const T& _newVal)
		//{
		//	  InvokeCommand(new ComdModifyValue<T>{ _var, _newVal });
		//}
		void InvokeCommandInsert(GameObject&, Scene&, bool *_notify = nullptr);
		void InvokeCommandDelete(GameObject&, Scene&, bool *_notify = nullptr);

		template<class Component, typename T>
		void InvokeCommand(const uint64_t& _id, T* _var, const T& _oldVal, bool *_notify = nullptr)
		{
			InvokeCommand(new ComdModifyValue<T, Component>{ _id, _var, _oldVal, _notify });
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

		// Calls the ExecuteUndo function of latest command in the undo deque and puts it into the redo deque
		void UndoCommand();
		// Performs InvokeCommand on the latest command in the redo deque 
		void RedoCommand();
		// Attempt to create a command that last several frames. Like a continuously changing variable but at some point you want to revert back to 
		// Takes in a pointer to the variable to be changed and keeps the initial first call as the value to revert back when undo is called.
		// Only 1 pointer can be stored to recording at any given time. 
		// DO NOT pass in any variable that you would be deleted without EndRecording being called.
		template<class C, typename T>
		void StartRecording(const uint64_t& _id, T* _target, bool *_notify = nullptr)
		{ 
			if (mRecording) return;
			mpRecorder = new ComdRecord<T, C>(_id, _target, _notify);
			mRecording = true;
		}

		// Call after done recording the data 
		void EndRecording();
		bool IsRecording() const;

		// Save Callback
		void SaveCallback();

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



