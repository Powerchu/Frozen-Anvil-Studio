/* HEADER *********************************************************************************/
/*!
\file	Globals.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Global variables or defines that is used everywhere
	and feels annoying to manually type them out everytime.

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#ifndef EDITOR
#define EDITOR 0
#endif

#if EDITOR

#define _EDITOR_START_TRY			try {
#define _EDITOR_CATCH(_X_)	        } catch (_X_)
#define _EDITOR_END_TRY				}
#define _EDITOR_THROW(_X_)			throw _X_
#define _EDITOR_CODE(_X_)           _X_
#define _GAME_NOEXCEPT
#define UNUSED_PARAMETER(_X_)       static_cast<void>(_X_);

#ifdef  EXPORT
	#define _DLL_EXPORT   __declspec( dllexport )
	#else 
	#define _DLL_EXPORT   __declspec( dllimport )
#endif


#define _DLL_EXPORT_ONLY  __declspec( dllexport )


#else

#define _EDITOR_START_TRY	
#define _EDITOR_CATCH(_X_)	
#define _EDITOR_END_TRY		
#define _EDITOR_THROW(_X_)	
#define _EDITOR_CODE(_X_)
#define _GAME_NOEXCEPT		noexcept

#define _DLL_EXPORT
#endif

#define _FIXED_UPDATE_DT		20000000Ui64


#endif		// INCLUDE GUARD

