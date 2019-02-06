/* HEADER *********************************************************************************/
/*!
\file	Factory.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _FACTORY_H_
#define _FACTORY_H_

#include "Allocator/DefaultAlloc.h"
#include "Utility/Utility.h"

#include "DataStructure/HashString.h"
#include "DataStructure/MagicArray.h"

#include "Math/Vectors.h"

namespace Dystopia
{
	class Component;
	class TextSerialiser;
	class Behaviour;
	class GameObject;

	class Factory
	{
	public:
		Factory(void);
		~Factory(void);

		GameObject* SpawnPrefab(const HashString&, const Math::Pt3D&);

	private:

		unsigned LoadSegment(GameObject&, Dystopia::TextSerialiser&);
		void LoadSegmentC(GameObject&, unsigned, Dystopia::TextSerialiser&);
		void LoadSegmentB(GameObject&, Dystopia::TextSerialiser&);

		MagicArray<HashString> mArrPrefabPaths;
	
	};
}


#endif





