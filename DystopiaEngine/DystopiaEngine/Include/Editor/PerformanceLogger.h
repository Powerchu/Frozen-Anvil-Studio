/* HEADER *********************************************************************************/
/*!
\file	PerformanceLog.cpp
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _PERFORMANCE_LOGGER_H_
#define _PERFORMANCE_LOGGER_H_
#include <string>

namespace Dystopia
{
	// use LogDataS for sub graphs
	void LogDataS(const std::string& _category, const std::string& _graphLabel, 
				  const float& _val, float _min, float _max, bool _bigGraph = false);

	// use LogDataG for a general graph, the _catMainGraph is also the category. Meaning calling LogDataS with a _category same
	// as this _catMainGraph, will result in them being a sub graph of this
	void LogDataG(const std::string& _catMainGraph, const float& _val, 
				  float _min, float _max, bool _bigGraph = true);
}

#endif	//_PERFORMANCE_LOG_H_
#endif	//EDITOR
