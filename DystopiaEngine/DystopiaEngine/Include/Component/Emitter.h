/* HEADER *********************************************************************************/
/*!
\file	Emitter.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Particle Emitter

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _PARTICLE_EMITTER_H_
#define _PARTICLE_EMITTER_H_

#include "Component/Component.h"
#include "Component/ComponentList.h"

#include "DataStructure/AutoArray.h"

#include "Math/MathFwd.h"

#include "Globals.h"

namespace Dystopia
{
	class ParticleSystem;

	class _DLL_EXPORT Emitter : public Component
	{
	public:
		using SYSTEM = ParticleSystem;
		unsigned GetComponentType(void) const
		{
			return Ut::MetaFind_t<Ut::Decay_t<decltype(*this)>, AllComponents>::value;
		};
		unsigned GetRealComponentType(void) const
		{
			return Ut::MetaFind_t<Ut::Decay_t<decltype(*this)>, UsableComponents>::value;
		};
		static const std::string GetCompileName(void) { return "Particle Emitter"; }
		const std::string GetEditorName(void) const { return GetCompileName(); }

		Emitter(void) noexcept;
		~Emitter(void) noexcept;



	private:

		AutoArray<Math::Vec3> mPositions;
	};
}






// ============================================ FUNCTION DEFINITIONS ============================================ // 




#endif		// INCLUDE GUARD

