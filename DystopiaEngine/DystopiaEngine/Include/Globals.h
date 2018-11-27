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
#define UNUSED_PARAMETER(_X_)       static_cast<void>(_X_);

#ifdef  EXPORT
#define _DLL_EXPORT   __declspec( dllexport )
#else 
#define _DLL_EXPORT   __declspec( dllimport )
#endif


#define _DLL_EXPORT_ONLY  __declspec( dllexport )

#endif


namespace Gbl
{
	constexpr bool	FULLSCREEN      = true;
	constexpr int   WINDOW_WIDTH    = 1600;
	constexpr int   WINDOW_HEIGHT   = 1000;

	constexpr auto  FIXEDUPDATE_DT  = 20000000Ui64;
	constexpr float FIXEDUPDATE_FDT = float(FIXEDUPDATE_DT / 1e9);
	
	constexpr auto  ATLAS_EXT       = "dmap";
	constexpr auto  FONTMAP_EXT     = ATLAS_EXT;
	constexpr auto  SETTINGS_EXT    = "dyst";
	constexpr auto  SCENE_EXT       = "dscene";
	constexpr auto  PREFAB_EXT      = "dobj";
	constexpr auto  MESH_EXT        = "dmesh";
	constexpr auto  FRAGSHADER_EXT  = "dfrag";
	constexpr auto  VERTSHADER_EXT  = "dvert";

	constexpr auto  PNG_EXT			= "png";
	constexpr auto  BMP_EXT			= "bmp";
	constexpr auto  CPP_EXT			= "cpp";
	constexpr auto  MP3_EXT			= "mp3";
	constexpr auto  DDS_EXT			= "dds";
	constexpr auto  WAV_EXT			= "wav";

	constexpr auto  CRASHREPORT_EXT = "dlog";
	constexpr auto  METADATA_EXT    = "dmeta";
}


#endif		// INCLUDE GUARD

