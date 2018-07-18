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
		: mDeqRedo{ _nHistory }, mDeqUndo{ _nHistory }, mRecording{ false },
		mpRecorder{ nullptr }
	{
		mDeqRedo.clear();
		mDeqUndo.clear();
	}

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
		for (auto e : mDeqRedo)
			delete e;
		mDeqRedo.clear();
	}

	void CommandHandler::UndoCommand()
	{
		if (!mDeqUndo.size()) return; 

		mDeqUndo.back()->ExecuteUndo();

		if (mDeqRedo.size() == mDeqRedo.max_size())
			PopFrontOfDeque(mDeqRedo);

		mDeqRedo.push_back(mDeqUndo.back());
		mDeqUndo.pop_back();
	}

	void CommandHandler::RedoCommand()
	{
		if (!mDeqRedo.size()) return;

		mDeqRedo.back()->ExecuteDo();

		if (mDeqUndo.size() == mDeqUndo.max_size())
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

	void CommandHandler::EndRecording()
	{
		if (!mRecording) return;
		if (mpRecorder)
		{
			mpRecorder->EndRecord();
			if (!mpRecorder->Unchanged()) InvokeCommand(mpRecorder);
			mpRecorder = nullptr;
		}
		mRecording = false;
	}

	bool CommandHandler::IsRecording() const
	{
		return mRecording;
	}
}




