#include "System/Tag/TagSystem.h"
#include "Utility/Utility.h"
namespace Dystopia
{
	TagSystem::TagSystem()
		:mTags{ _TAG_(32) }
	{

	}
	AutoArray < std::string > TagSystem::GetAllTagsName() const
	{
		AutoArray < std::string > toRet;
		for (auto & elem : mTags)
		{
			toRet.push_back(elem.second.c_str());
		}

		return Ut::Move(toRet);
	}
}

