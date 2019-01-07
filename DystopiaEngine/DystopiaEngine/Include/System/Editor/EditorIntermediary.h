/* HEADER *********************************************************************************/
/*!
\file	EditorIntermediary.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _EDITOR_INTERMEDIARY_H_
#define _EDITOR_INTERMEDIARY_H_
#include "System/Base/Systems.h"

namespace Dystopia
{
	class EditorIntermediary : public Systems
	{
	public:
		EditorIntermediary(void);
		~EditorIntermediary(void);
		void PreInit(void) { };
		bool Init(void);
		void PostInit(void) { };

		void PreFixedUpdate(float) { };
		void FixedUpdate(float) { };
		void Update(float);
		void PostUpdate(void) { };
		void Shutdown(void);

		void LoadDefaults(void) { };
		void LoadSettings(DysSerialiser_t&) { };
		void SaveSettings(DysSerialiser_t&) { };

		void Serialise() const { };
		void Unserialise() { };

		void ReceiveMessage(const eSysMessage&, size_t);

	};
}
#endif		// INCLUDE GUARD

