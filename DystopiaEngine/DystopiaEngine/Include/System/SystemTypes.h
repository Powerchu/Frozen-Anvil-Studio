/* HEADER *********************************************************************************/
/*!
\file	SystemsTypes.h
\author Tan Jie Wei Jacky (0%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Please add newly created systems to the enum to ensure correct count
	Order of system within the enum does matter.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _SYSTEM_TYPES_H_
#define _SYSTEM_TYPES_H_

// Update the enum whenever a new system is made
// This ensures the count is correct
// Update order is the same as the order within this enum
enum eSYSTEMS : unsigned
{
	TIME_SYSTEM,
	INPUT_SYSTEM,
	WINDOW_SYSTEM,
	COLLISION_SYSTEM,
	PHYSICS_SYSTEM,
	SCENE_SYSTEM,
	SOUND_SYSTEM,
	CAMERA_SYSTEM,
	GRAPHIC_SYSTEM,
	EVENT_SYSTEM,
	AI_SYSTEM,
	BEHAVIOUR_SYSTEM,
	INTERMEDIARY_SYSTEM,
	DATABASE_SYSTEM,
	FILE_SYSTEM,
	PARTICLE_SYSTEM,
	POSTPROCESS_SYSTEM,
	PROFILER_SYSTEM,
	VIDEO_SYSTEM,
};


#endif		// INCLUDE GUARD

