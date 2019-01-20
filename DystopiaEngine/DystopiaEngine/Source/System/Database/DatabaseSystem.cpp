/* HEADER *********************************************************************************/
/*!
\file	DatabaseSystem.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Database/DatabaseSystem.h"
#include "System/Driver/Driver.h"
#include "System/File/FileSystem.h"

#include "IO/TextSerialiser.h"
#include "Globals.h"

#include <string>

Dystopia::DatabaseSystem::DatabaseSystem(void) 
{
}

Dystopia::DatabaseSystem::~DatabaseSystem(void) 
{
}

bool Dystopia::DatabaseSystem::Init(void)
{
	return true;
}

void Dystopia::DatabaseSystem::Update(float)
{
}

void Dystopia::DatabaseSystem::Shutdown(void)
{
	for (auto& elem : mArrDataSheets)
		elem.second.CloseSheet();

	mArrDataSheets.clear();
	mArrSheetNames.clear();
}

void Dystopia::DatabaseSystem::LoadDefaults(void)
{
}

void Dystopia::DatabaseSystem::LoadSettings(DysSerialiser_t& _in)
{
	size_t n = 0;
	_in >> n;
	for (size_t i = 0; i < n; ++i)
	{
		HashString name;
		_in >> name;
		if (name.length())
			NewData(name);
	}
}

void Dystopia::DatabaseSystem::SaveSettings(DysSerialiser_t& _out)
{
	_out << mArrDataSheets.size();
	for (auto& sheet : mArrDataSheets)
		_out << sheet.first;
}

bool Dystopia::DatabaseSystem::NewData(const HashString& _name)
{
	for (const auto& elem : mArrDataSheets)
		if (elem.first == _name)
			return false;

	auto n = GetPathFormatted(_name);
	if (!n.length())
		return false;

	HashString name{ _name };
	mArrDataSheets.Insert(std::make_pair(name, DataSheet{ n.c_str() }));
	mArrSheetNames.Insert(mArrDataSheets.back().first.c_str());

	HashString temp{ _name };
	if (temp.find(".ddata") == HashString::nPos)
	{
		temp += '.';
		temp += Gbl::DATASHEET_EXT;
	}

	auto found = EngineCore::Get<FileSystem>()->GetFullPath(temp.c_str(), eFileDir::eResource);
	if (!found.length())
		auto file = TextSerialiser::OpenFile(n.c_str(), Serialiser::MODE_WRITE);

	return mArrDataSheets.back().second.OpenSheet();
}

bool Dystopia::DatabaseSystem::OpenData(const HashString& _name)
{
	for (auto& elem : mArrDataSheets)
	{
		if (elem.first == _name)
		{
			auto& db = elem.second;
			if (!db.IsOpened())
				db.OpenSheet();
			return true;
		}
	}
	return false;
}

bool Dystopia::DatabaseSystem::CloseData(const HashString& _name)
{
	for (auto& elem : mArrDataSheets)
	{
		if (elem.first == _name)
		{
			auto& db = elem.second;
			if (db.IsOpened())
				db.CloseSheet();
			return true;
		}
	}
	return false;
}

void Dystopia::DatabaseSystem::UnloadData(const HashString& _name)
{
	for (size_t i = 0; i < mArrDataSheets.size(); i++)
	{
		if (mArrDataSheets[i].first == _name)
		{
			mArrDataSheets[i].second.CloseSheet();
			mArrDataSheets.FastRemove(i);
			mArrSheetNames.FastRemove(i);
			break;
		}
	}
}

Dystopia::DataSheet* Dystopia::DatabaseSystem::GetDatabase(const HashString& _name)
{
	for (auto& elem : mArrDataSheets)
		if (elem.first == _name)
			return &elem.second;
	return nullptr;
}

void Dystopia::DatabaseSystem::SaveAllSheets(void)
{
	for (auto& elem : mArrDataSheets)
	{
		elem.second.CloseSheet();
		elem.second.OpenSheet();
	}
}

HashString Dystopia::DatabaseSystem::GetPathFormatted(const HashString& _name)
{
	auto path = EngineCore::Get<FileSystem>()->GetProjectFolders<std::string>(eFileDir::eResource);

	if (!path.length())
		return HashString{};

	path += '/';
	path += _name.c_str();

	if (_name.find(Gbl::DATASHEET_EXT) == std::string::npos)
	{
		path += '.';
		path += Gbl::DATASHEET_EXT;
	}
	return path.c_str();
}

AutoArray<std::pair<HashString, Dystopia::DataSheet>>& Dystopia::DatabaseSystem::GetAllSheets(void)
{
	return mArrDataSheets;
}

AutoArray<const char *>& Dystopia::DatabaseSystem::GetAllNames(void)
{
	return mArrSheetNames;
}






