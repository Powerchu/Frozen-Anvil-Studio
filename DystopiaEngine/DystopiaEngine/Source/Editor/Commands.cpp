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

namespace Dystopia
{
	CommandHandler::CommandHandler(size_t _nHistory)
		: mDeqRedo{ _nHistory }, mDeqUndo{ _nHistory }
	{
	}

	CommandHandler::~CommandHandler()
	{
		mDeqRedo.clear();
		mDeqUndo.clear();
	}

	void CommandHandler::InvokeCommand(Commands *_comd)
	{
		_comd->ExecuteDo();

		if (mDeqUndo.size() == mDeqUndo.max_size())
			mDeqUndo.pop_front();

		mDeqUndo.push_back(_comd);
		mDeqRedo.clear();
	}

	void CommandHandler::UndoCommand()
	{
		if (mDeqUndo.empty()) return;

		mDeqUndo.back()->ExecuteUndo();

		if (mDeqRedo.size() == mDeqRedo.max_size())
			mDeqRedo.pop_front();

		mDeqRedo.push_back(mDeqUndo.back());
		mDeqUndo.pop_back();
	}

	void CommandHandler::RedoCommand()
	{
		if (mDeqRedo.empty()) return;

		InvokeCommand(mDeqRedo.back());
		mDeqRedo.pop_back();
	}
}




