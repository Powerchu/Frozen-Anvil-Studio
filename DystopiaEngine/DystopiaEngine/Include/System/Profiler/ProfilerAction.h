/* HEADER *********************************************************************************/
/*!
\file	ProfilerAction.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	For use with scoped timer

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _PROFILER_ACTION_H_
#define _PROFILER_ACTION_H_

#include <chrono>
#include <string>


namespace Dystopia
{
	class ProfilerAction
	{
	public:
		using Precision_t = std::chrono::microseconds;

		explicit ProfilerAction(const std::string& _strModuleName, const std::string& _strFunctionName);

		void PostDuration(Precision_t);

	private:

		std::string strModule, strFunction;
	};
}



#endif		// INCLUDE GUARD

