/* HEADER *********************************************************************************/
/*!
\file	TagSystem.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _TAG_SYSTEM_H_
#define _TAG_SYSTEM_H_

#define TagType unsigned

#include "DataStructure/Array.h"
#include <string>

namespace Dystopia
{
	class TextSerializer;
	class TagSystem
	{
	public:
		TagSystem(void);
		~TagSystem(void);

		void Init(void);
		void Shutdown(void);
		void Update(float);
		void LoadDefault(void);
		void LoadSettings(void);
		void Serialize(TextSerializer&);
		void UnSerialize(TextSerializer&);

		void AddTag(unsigned _index);
		void AddTag(const std::string& _tagName);
		void RemoveTag(unsigned _index);
		void RemoveTag(const std::string& _tagName);

	private:
		Array<std::string, sizeof(TagType)> mArrStoredTags;

	};
}

#endif //_TAG_SYSTEM_H_


