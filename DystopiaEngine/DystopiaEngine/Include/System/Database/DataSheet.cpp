/* HEADER *********************************************************************************/
/*!
\file	DataSheet.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Database/DataSheet.h"
#include "System/Driver/Driver.h"
#include "System/File/FileSystem.h"
#include "IO/TextSerialiser.h"

#include <string>

Dystopia::DataSheet::DataSheet(const HashString& _fullpath)
	: mbIsOpened{ false }, mCurrentSheetPath{ _fullpath }
{
}

Dystopia::DataSheet::~DataSheet(void)
{
	CloseSheet();
	mCurrentSheetPath.clear();
}

bool Dystopia::DataSheet::OpenSheet()
{
	if(mbIsOpened)
		return mbIsOpened;

	if (!std::filesystem::exists(mCurrentSheetPath.c_str()))
		return false;

	mbIsOpened = true;
	LoadSheet();
	return mbIsOpened;
}

bool Dystopia::DataSheet::CloseSheet(bool _save)
{
	if (mbIsOpened)
	{
		if (_save)
			SaveSheet();

		for (auto& elem : mArrSheetElements)
			Dystopia::DefaultAllocator<DataSheetElementBase>::DestructFree(elem);

		mArrSheetElements.clear();
		mArrNames.clear();
	}
	mbIsOpened = false;
	return !mbIsOpened;
}

bool Dystopia::DataSheet::IsOpened(void) const
{
	return mbIsOpened;
}

const HashString& Dystopia::DataSheet::GetOpened(void) const
{
	return mCurrentSheetPath;
}

AutoArray<DataSheetElementBase*>& Dystopia::DataSheet::GetAllElements(void)
{
	return mArrSheetElements;
}

AutoArray<const char*>& Dystopia::DataSheet::GetAllNames(void)
{
	return mArrNames;
}

bool Dystopia::DataSheet::RemoveElement(const HashString& _name)
{
	for (size_t i = 0; i < mArrSheetElements.size(); ++i)
	{
		if (mArrSheetElements[i]->mName == _name)
		{
			mArrNames.FastRemove(i);
			mArrSheetElements.FastRemove(i);
			return true;
		}
	}
	return false;
}

void Dystopia::DataSheet::SaveSheet(void)
{
	auto file = TextSerialiser::OpenFile(mCurrentSheetPath.c_str(), Serialiser::MODE_WRITE);

	file.InsertStartBlock("_Number Of Elements");
	file << mArrSheetElements.size();
	file.InsertEndBlock("_Number Of Elements");
	for (size_t i = 0; i < mArrSheetElements.size(); i++)
	{
		HashString label{ "_" };
		label += static_cast<unsigned>(i);

		file.InsertStartBlock(label.c_str()); 
		auxHelper.SaveElement(this, mArrSheetElements[i], file);
		file.InsertEndBlock(label.c_str());
	}
}

void Dystopia::DataSheet::LoadSheet()
{
	auto file = TextSerialiser::OpenFile(mCurrentSheetPath.c_str(), Serialiser::MODE_READ);

	size_t count = 0;
	file.ConsumeStartBlock();
	file >> count;
	file.ConsumeEndBlock();
	for (size_t i = 0; i < count; i++)
	{
		unsigned typeN = static_cast<unsigned>(-1);

		file.ConsumeStartBlock();
		file >> typeN;
		if (typeN >= 0)
			auxHelper.LoadElement(this, typeN, file);
		file.ConsumeEndBlock();
	}
}









