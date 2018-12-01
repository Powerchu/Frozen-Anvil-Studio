#pragma once
#ifndef _TAG_SYSTEM_H
#define _TAG_SYSTEM_H

#include "Tags.h"
#include "DataStructure/HashString.h"
#include "DataStructure/AutoArray.h"
#include "Reflection/PPUtils.h"

/*STL*/
#include <map>

#define _TAG_STRINGIFY_(_STR_) #_STR_
#define _TAG_GENERATOR_(_NUM_) std::make_pair<Tags, HashString>(Tags::TAG_ ## _NUM_ , _TAG_STRINGIFY_(Tag_ ## _NUM_))

#define _TAG_(_SIZE_) PP_EVAL(_TAGAUX_ ## _SIZE_)()
#define _TAGAUX_32() _TAG_GENERATOR_(0), _TAG_GENERATOR_(1), _TAG_GENERATOR_(2), _TAG_GENERATOR_(3), _TAG_GENERATOR_(4), _TAG_GENERATOR_(5)        , \
				     _TAG_GENERATOR_(6), _TAG_GENERATOR_(7), _TAG_GENERATOR_(8), _TAG_GENERATOR_(9), _TAG_GENERATOR_(10), _TAG_GENERATOR_(11)      , \
					 _TAG_GENERATOR_(12), _TAG_GENERATOR_(13), _TAG_GENERATOR_(14), _TAG_GENERATOR_(15), _TAG_GENERATOR_(16), _TAG_GENERATOR_(17)  , \
                     _TAG_GENERATOR_(18), _TAG_GENERATOR_(19), _TAG_GENERATOR_(20), _TAG_GENERATOR_(21), _TAG_GENERATOR_(22), _TAG_GENERATOR_(23)  , \
                     _TAG_GENERATOR_(24), _TAG_GENERATOR_(25), _TAG_GENERATOR_(26), _TAG_GENERATOR_(27), _TAG_GENERATOR_(28), _TAG_GENERATOR_(29)  , \
                     _TAG_GENERATOR_(30), _TAG_GENERATOR_(31)
#define _TAGAUX_27() _TAG_GENERATOR_(5)																												, \
				     _TAG_GENERATOR_(6), _TAG_GENERATOR_(7), _TAG_GENERATOR_(8), _TAG_GENERATOR_(9), _TAG_GENERATOR_(10), _TAG_GENERATOR_(11)      , \
					 _TAG_GENERATOR_(12), _TAG_GENERATOR_(13), _TAG_GENERATOR_(14), _TAG_GENERATOR_(15), _TAG_GENERATOR_(16), _TAG_GENERATOR_(17)  , \
                     _TAG_GENERATOR_(18), _TAG_GENERATOR_(19), _TAG_GENERATOR_(20), _TAG_GENERATOR_(21), _TAG_GENERATOR_(22), _TAG_GENERATOR_(23)  , \
                     _TAG_GENERATOR_(24), _TAG_GENERATOR_(25), _TAG_GENERATOR_(26), _TAG_GENERATOR_(27), _TAG_GENERATOR_(28), _TAG_GENERATOR_(29)  , \
                     _TAG_GENERATOR_(30), _TAG_GENERATOR_(31)

namespace Dystopia
{
	class TagSystem
	{
	public:
		TagSystem();

		void MapNameToTag(HashString const & _name, Tags _TagsNum);
		 
		Tags GetTag(HashString const & _TagName) const;

		void RemoveTag(HashString const & _TagName) const;
		
		void EditorUI(void);

		AutoArray < std::string > GetAllTagsName() const;

		AutoArray<HashString>  ConvertTagsToHash  (Tags _Tag) const;
		AutoArray<std::string> ConvertTagsToString(Tags _Tag) const;
		AutoArray<Tags>        GetTagsAsArray(Tags _Tag)    const;

		using TagTable = std::map<Tags, HashString>;

	private:
		TagTable mTags;
	};
}


#endif


