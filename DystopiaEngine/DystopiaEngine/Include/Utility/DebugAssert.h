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

#if !defined(DISABLE_ASSERTS)

#include <cstdio>
#include <cassert>

#define DEBUG_PRINT(...)				\
do {									\
	::fprintf(stderr, __VA_ARGS__);		\
} while (false)

#define DEBUG_ASSERT(x, ...)			\
do {									\
	if ((x))						    \
	{									\
		DEBUG_PRINT(					\
		"!ERROR IN %s Line %lu, \n"		\
		, __FUNCTION__, __LINE__);		\
		DEBUG_PRINT(__VA_ARGS__);		\
		assert(true);					\
	}									\
} while (false)

#if defined(DEBUG) | defined(_DEBUG)
#define DEBUG_BREAK(x, ...)			    \
do {									\
	if ((x))							\
	{									\
		DEBUG_PRINT(					\
		"!ERROR IN %s Line %lu, \n"		\
		, __FUNCTION__, __LINE__);		\
		DEBUG_PRINT(__VA_ARGS__);		\
		__debugbreak();					\
	}									\
} while (false)
#else
#define DEBUG_BREAK(x, ...) DEBUG_ASSERT(x, __VA_ARGS__)
#endif

#define DEBUG_LOG(x, ...)				\
do {									\
	if ((x))							\
	{									\
		DEBUG_PRINT(					\
		"!IN %s Line %lu, \n"			\
		, __FUNCTION__, __LINE__);		\
		DEBUG_PRINT(__VA_ARGS__);		\
	}									\
} while (false)

#else

#define DEBUG_PRINT(...)		
#define DEBUG_ASSERT(x, ...)	(x)
#define DEBUG_BREAK(x, ...)		(x)
#define DEBUG_LOG(x, ...)		(x)

#endif // DISABLE_ASSERTS



#endif		// INCLUDE GUARD

