/* HEADER *********************************************************************************/
/*!
\file	Milestone.h
\author keith (100%)
\par    email: keith.goh\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _Milestone_H_
#define _Milestone_H_

#define str(s) #s

#include "Behaviour\Behaviour.h"
#include "Reflection\Reflection.h"

#define DllExport   __declspec( dllexport )

namespace Dystopia
{

	class Milestone : Behaviour
	{
	public:

		static constexpr const char * BehaviourName = str(Milestone);
#if !EDITOR
		
		using SYSTEM = BehaviourSystem;
		unsigned GetBehaviourType(void) const
		{
			return Utility::MetaFind_t<Utility::Decay_t<decltype(*this)>, AllBehaviours>::value;
		};

#endif
		virtual const std::string GetEditorName(void) const override { return "Milestone"; }
		static uint64_t constexpr mMilestoneID = 18446744073591342592;

		Milestone();
		~Milestone();
		
		virtual void Load(void) override;
		virtual void Init(void) override;

		virtual void Update(const float _fDeltaTime) override;
		virtual void FixedUpdate(const float _fDeltaTime) override;
		virtual void PostUpdate(void) override;

		virtual void GameObjectDestroy(void) override;
		virtual void Unload(void) override;

		virtual void Serialise(TextSerialiser&) const override;

		virtual void Unserialise(TextSerialiser&) override;

		virtual const char * const GetBehaviourName() const;

		virtual Milestone * Duplicate() const;
		
		virtual void EditorUI(void) noexcept override;
		
		
		virtual TypeErasure::TypeEraseMetaData       GetMetaData();
		virtual TypeErasure::TypeEraseMetaData const GetMetaData() const;

		
		float mFloat = 5.f;
        int   mInt   = 25;
        float mFloat2 = 10.f;
        int   mInt2   = 100;
        int   WeiXiang = 200;
		
	private:



	};

	extern "C"
	{
		DllExport Milestone * MilestoneClone()
		{
			return new Milestone;
		}
	}
}

PP_REFLECT(Dystopia::Milestone, mFloat, mInt, mFloat2, mInt2, WeiXiang)

#endif //_Milestone_H_


