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

Dystopia::DatabaseSystem::DatabaseSystem(void) noexcept
{
}

Dystopia::DatabaseSystem::~DatabaseSystem(void) noexcept
{
}

void Dystopia::DatabaseSystem::Shutdown(void) noexcept
{
	for (auto& elem : mArrDataSheets)
		elem.second.CloseSheet();
}

bool Dystopia::DatabaseSystem::NewData(const HashString& _name)
{
	for (const auto& elem : mArrDataSheets)
		if (elem.first == _name)
			return false;

	mArrDataSheets.Insert(std::make_pair(_name, DataSheet{ GetPathFormatted(_name).c_str() }));
	return true;
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
	auto path = EngineCore::Get<FileSystem>()->GetProjectFolders<std::string>(eFileDir::eCurrent);
	path += '/';
	path += _name.c_str();

	if (_name.find(Gbl::DATASHEET_EXT) == std::string::npos)
	{
		path += '.';
		path += Gbl::DATASHEET_EXT;
	}
	return path.c_str();
}





