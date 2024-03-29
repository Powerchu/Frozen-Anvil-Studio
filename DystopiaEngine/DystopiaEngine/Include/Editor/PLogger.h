/* HEADER *********************************************************************************/
/*!
\file	PLogger.cpp
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _P_LOGGER_H_
#define _P_LOGGER_H_
#include "Editor/PLogInfo.h"

namespace Editor//Dystopia
{
	namespace Performance
	{

		// use LogDataS for sub graphs
		void LogDataS(const HashString& _category, const HashString& _graphLabel, float _val);

		// use LogDataG for a general graph, the _catMainGraph is also the category. Meaning calling LogDataS with a _category same
		// as this _catMainGraph, will result in them being a sub graph of this
		void LogDataG(const HashString& _catMainGraph, float _val);

		void LogTaskMgr(const PLogTaskManager&);
	}
}

#endif	//_P_LOGGER_H_
#endif	//EDITOR
