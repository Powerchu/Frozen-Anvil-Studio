#include "System/Tag/TagSystem.h"
#include "Utility/Utility.h"
namespace Dystopia
{
	TagSystem::TagSystem()
		:mTags{ std::make_pair(Tags::TAG_0, "Untagged"),
				std::make_pair(Tags::TAG_1, "Respawn"),
				std::make_pair(Tags::TAG_2, "Finish"),
				std::make_pair(Tags::TAG_3, "Player"),
				std::make_pair(Tags::TAG_4, "Enemy"),
				_TAG_(27) }
	{
	}

	void TagSystem::MapNameToTag(HashString const& _name, Tags _TagsNum)
	{
		if(mTags.find(_TagsNum) != mTags.end())
		{
			mTags[_TagsNum] = _name;
		}
	}

	Tags TagSystem::GetTag(HashString const& _TagName) const
	{
		for (auto & elem : mTags)
			if (elem.second == _TagName)
				return elem.first;

		return Tags(0);
	}

	void TagSystem::RemoveTag(HashString const&) const
	{

	}

	void TagSystem::EditorUI()
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

	AutoArray<HashString> TagSystem::ConvertTagsToHash(Tags _Tag) const
	{
		AutoArray <HashString> toRet;
		for (auto & elem : mTags)
		{
			if(static_cast<unsigned>(elem.first) & static_cast<unsigned>(_Tag))
			{
				toRet.push_back(elem.second);
			}
		}
		return Ut::Move(toRet);
	}

	AutoArray<std::string> TagSystem::ConvertTagsToString(Tags _Tag) const
	{
		AutoArray <std::string> toRet;
		for (auto & elem : mTags)
		{
			if (static_cast<unsigned>(elem.first) & static_cast<unsigned>(_Tag))
			{
				toRet.push_back(elem.second.c_str());
			}
		}
		return Ut::Move(toRet);
	}

	AutoArray<Tags> TagSystem::GetTagsAsArray(Tags _Tag) const
	{
		AutoArray <Tags> toRet;
		for (auto & elem : mTags)
		{
			if (static_cast<unsigned>(elem.first) & static_cast<unsigned>(_Tag))
			{
				toRet.push_back(elem.first);
			}
		}
		return Ut::Move(toRet);
	}
}

