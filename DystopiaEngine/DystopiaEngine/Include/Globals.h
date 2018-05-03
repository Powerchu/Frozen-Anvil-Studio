/* HEADER *********************************************************************************/
/*!
\file	Globals.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Global variables or defines that is used everywhere
	and feels annoying to manually type them out everytime.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#if EDITOR

#define _EDITOR_START_TRY		try {
#define _EDITOR_CATCH(_X_)		} catch (_X_) {
#define _EDITOR_END_TRY			}
#define _EDITOR_THROW(_X_)		throw (_X_)
#define _NOEXCEPT_IF_GAME

#else

#define _EDITOR_START_TRY		
#define _EDITOR_CATCH(_X_)		
#define _EDITOR_END_TRY			
#define _EDITOR_THROW(_X_)		
#define _NOEXCEPT_IF_GAME		noexcept

#endif

#define _SAVE_VERSION			1


#endif

