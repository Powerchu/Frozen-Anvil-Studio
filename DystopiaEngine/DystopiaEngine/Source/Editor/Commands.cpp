/* HEADER *********************************************************************************/
/*!
\file	Commands.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "Editor\Commands.h"
#include "Editor\CommandList.h"
#include "..\..\Dependancies\ImGui\imgui.h"
#include <typeinfo>

namespace Dystopia
{
	CommandHandler::CommandHandler(size_t _nHistory)
		: mDeqRedo{ _nHistory, nullptr }, mDeqUndo{ _nHistory, nullptr }, mRecording{ false },
		mpRecorder{ nullptr }
	{}

	CommandHandler::~CommandHandler()
	{
		for (auto e : mDeqRedo)
		{
			if (e)
			{
				delete e;
				e = nullptr;
			}
		}
		for (auto e : mDeqUndo)
		{
			if (e)
			{
				delete e;
				e = nullptr;
			}
		}

		mDeqRedo.clear();
		mDeqUndo.clear();
	}

	void CommandHandler::InvokeCommand(Commands *_comd)
	{
		_comd->ExecuteDo();

		if (mDeqUndo.size() == mDeqUndo.max_size())
			PopFrontOfDeque(mDeqUndo);

		mDeqUndo.push_back(_comd);
		mDeqRedo.clear();
	}

	void CommandHandler::UndoCommand()
	{
		if (!mDeqUndo.back()) return;

		mDeqUndo.back()->ExecuteUndo();

		if (mDeqRedo.size() == mDeqRedo.max_size())
			PopFrontOfDeque(mDeqRedo);

		mDeqRedo.push_back(mDeqUndo.back());
		mDeqUndo.pop_back();
	}

	void CommandHandler::RedoCommand()
	{
		if (!mDeqRedo.back()) return;

		Commands *pTemp = mDeqRedo.back();
		mDeqRedo.pop_back();
		InvokeCommand(pTemp);
	}

	void CommandHandler::PopFrontOfDeque(std::deque<Commands*>& _targetDeque)
	{
		if (!_targetDeque.size() && _targetDeque.front())
		{
			delete _targetDeque.front();
			_targetDeque.pop_front();
		}
	}

	void CommandHandler::EndRecording()
	{
		if (!mRecording) return;
		if (mpRecorder)
		{
			mpRecorder->EndRecord();
			InvokeCommand(mpRecorder);
			mpRecorder = nullptr;
		}
		mRecording = false;
	}

	bool CommandHandler::IsRecording() const
	{
		return mRecording;
	}
}




