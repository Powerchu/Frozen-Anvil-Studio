/* HEADER *********************************************************************************/
/*!
\file	Commands.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Editor/Commands.h"

#include "System/Driver/Driver.h"
#include "System/Window/WindowManager.h"
#include "System/Window/Window.h"
#include "System/Scene/Scene.h"
#include "Object/GameObject.h"

#include "../../Dependancies/ImGui/imgui.h"
#include <typeinfo>

namespace Dystopia
{
	CommandHandler::CommandHandler(size_t _nHistory)
		: mDeqRedo{ }, mDeqUndo{ }, mRecording{ false },
		mpRecorder{ nullptr }, mMaxSize{ _nHistory }
	{
	
	}

	CommandHandler::~CommandHandler()
	{
		if (!mDeqRedo.empty() || !mDeqUndo.empty()) Shutdown();
	}

	void CommandHandler::Shutdown()
	{
		for (auto& e : mDeqRedo)
		{
			delete e;
			e = nullptr;
		}
		for (auto& e : mDeqUndo)
		{
			delete e;
			e = nullptr;
		}
		mDeqRedo.clear();
		mDeqUndo.clear();
	}

	void CommandHandler::InvokeCommand(Commands *_comd)
	{
		if (!_comd->ExecuteDo()) return;
		if (Editor::GetInstance()->CurrentState() == EDITOR_PLAY)
		{
			delete _comd;
			return;
		}

		auto& w = EngineCore::GetInstance()->GetSystem<WindowManager>()->GetMainWindow();
		w.SetTitle(w.GetTitle() + L"*");

		mUnsavedChanges = true;
		if (mDeqUndo.size() == mMaxSize)
			PopFrontOfDeque(mDeqUndo);
		for (auto& e : mDeqRedo)
		{
			delete e;
			e = nullptr;
		}
		mDeqRedo.clear();
		mDeqUndo.push_back(_comd);
	}

	void CommandHandler::UndoCommand()
	{
		EndRecording();
		if (!mDeqUndo.size()) return; 
		if (!mDeqUndo.back()->ExecuteUndo())
		{
			RemoveStray(mDeqUndo);
			return;
		}

		mUnsavedChanges = true;
		if (mDeqRedo.size() == mMaxSize)
			PopFrontOfDeque(mDeqRedo);

		mDeqRedo.push_back(mDeqUndo.back());
		mDeqUndo.pop_back();
	}

	void CommandHandler::RedoCommand()
	{
		EndRecording();
		if (!mDeqRedo.size()) return;
		if (!mDeqRedo.back()->ExecuteDo())
		{
			RemoveStray(mDeqRedo);
			return;
		}

		mUnsavedChanges = true;
		if (mDeqUndo.size() == mMaxSize)
			PopFrontOfDeque(mDeqUndo);
		mDeqUndo.push_back(mDeqRedo.back());
		mDeqRedo.pop_back();
	}

	void CommandHandler::PopFrontOfDeque(std::deque<Commands*>& _targetDeque)
	{
		Commands *pTemp = _targetDeque.front();
		_targetDeque.pop_front();
		delete pTemp;
	}

	void CommandHandler::RemoveStray(std::deque<Commands*>& _targetDeque)
	{
		Commands *pTemp = _targetDeque.back();
		_targetDeque.pop_back();
		delete pTemp;
	}

	void CommandHandler::EndRecording()
	{
		if (!mRecording) return;
		if (mpRecorder)
		{
			if (mpRecorder->EndRecord() && !mpRecorder->Unchanged())
			{
				InvokeCommand(mpRecorder);
			}
			else 
				delete mpRecorder;

			mpRecorder = nullptr;
		}
		mRecording = false;
	}

	bool CommandHandler::IsRecording() const
	{
		return mRecording;
	}

	void CommandHandler::InvokeCommandInsert(GameObject& _pObj, Scene& _scene, bool * _notify)
	{
		InvokeCommand(new ComdInsertObject{&_pObj, &_scene, _notify });
	}

	void CommandHandler::InvokeCommandDelete(GameObject& _pObj, Scene& _scene, bool * _notify)
	{
		InvokeCommand(new ComdDeleteObject{ &_pObj, &_scene, _notify });
	}

	void CommandHandler::InvokeCommandInsert(const AutoArray<GameObject*>& _arrObj, Scene& _scene, bool *_notify)
	{
		if (_arrObj.size() == 1)
			InvokeCommandInsert(**(_arrObj.begin()), _scene, _notify);
		else
		{
			AutoArray<Commands*> mComdArray{ _arrObj.size() };
			for (const auto& elem : _arrObj)
				mComdArray.Insert(new ComdInsertObject{ elem, &_scene, _notify });
			InvokeCommand(new ComdBatch{ Ut::Move(mComdArray) });
		}
	}

	void CommandHandler::InvokeCommandDelete(const AutoArray<GameObject*>& _arrObj, Scene& _scene, bool *_notify)
	{
		if (_arrObj.size() == 1)
			InvokeCommandDelete(**(_arrObj.begin()), _scene, _notify);
		else
		{
			AutoArray<Commands*> mComdArray{ _arrObj.size() };
			for (const auto& elem : _arrObj)
				mComdArray.Insert(new ComdDeleteObject{ elem, &_scene, _notify });
			InvokeCommand(new ComdBatch{ Ut::Move(mComdArray) });
		}
	}

	void CommandHandler::SaveCallback()
	{
		mUnsavedChanges = false;
	}

	bool CommandHandler::HasUnsavedChanges() const
	{
		return mUnsavedChanges;
	}

	std::deque<Commands*>& CommandHandler::GetDeqRedo(void)
	{
		return mDeqRedo;
	}

	std::deque<Commands*>& CommandHandler::GetDeqUndo(void)
	{
		return mDeqUndo;
	}
}




