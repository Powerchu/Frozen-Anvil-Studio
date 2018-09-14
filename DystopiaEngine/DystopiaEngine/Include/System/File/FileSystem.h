#pragma once
#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H


#include <map>
#include <string>

#include "System/Base/Systems.h"

namespace Dystopia
{

	typedef enum class eFileDirectories : unsigned
	{
		eHeader,
		eSource,
		eResource,
		eRoot,
		eAppData,
		eTotalFilePath

	} eFileDir;

	struct FileSystem
	{
	public:

		FileSystem();

		std::string GetFullPath(std::string const & _FileName, eFileDir _ParentDirectory);

		bool CreateFiles(std::string const & _FileName, eFileDir _Directory);

	private:

		using PathTable = std::map<eFileDir, std::string>;

		static PathTable mPathTable;
		
	};
}

#endif