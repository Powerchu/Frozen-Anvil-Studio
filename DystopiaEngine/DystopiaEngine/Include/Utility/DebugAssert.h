/* HEADER *********************************************************************************/
/*!
\file	DebugAssert.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Macros for runtime error assertion.
	DEPRECATED once Editor (Game) & Logging (System) is done.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _DEBUG_ASSERT_H_
#define _DEBUG_ASSERT_H_


#include <cassert>

#if EDITOR

#include "System\Logger\LogPriority.h"
#include "System\Logger\LoggerSystem.h"

#include <cstdio>


#define DEBUG_PRINT(_X_, ...)                             \
do {								                      \
	Dystopia::LoggerSystem::ConsoleLog(_X_, __VA_ARGS__); \
} while (false)
 
#define DEBUG_ASSERT(_X_, ...)		                      \
do {								                      \
	if ((_X_))						                      \
	{                                                     \
		DEBUG_PRINT(eLog::ERROR,                          \
		"!ERROR IN %s Line %lu, \n",                      \
		__FUNCSIG__, __LINE__);	                          \
		DEBUG_PRINT(eLog::ERROR, __VA_ARGS__);	          \
		assert(true);				                      \
	}								                      \
} while (false)
 
#define DEBUG_LOG(_X_, ...)			                      \
do {								                      \
	if ((_X_))						                      \
	{								                      \
		DEBUG_PRINT(eLog::WARNING, 	                      \
		"!IN %s Line %lu, \n",                            \
		__FUNCSIG__, __LINE__);                           \
		DEBUG_PRINT(eLog::WARNING, __VA_ARGS__);          \
	}                                                     \
} while (false)
#define DEBUG_BREAK(_X_, ...)                             \
do {                                                      \
  if ((_X_))						                      \
  {                                                       \
    DEBUG_PRINT(                                          \
    eLog::ERROR,	                                      \
    "!ERROR IN %s Line %lu, \n",                          \
    __FUNCSIG__, __LINE__);                               \
    DEBUG_PRINT(eLog::ERROR, __VA_ARGS__);                \
    __debugbreak();				                          \
  }								                          \
} while (false)

#else

#define DEBUG_PRINT(...)
#define DEBUG_ASSERT(x, ...)	(x)
#define DEBUG_BREAK(x, ...)		(x)
#define DEBUG_LOG(x, ...)		(x)

#endif // DEBUG



#endif		// INCLUDE GUARD

