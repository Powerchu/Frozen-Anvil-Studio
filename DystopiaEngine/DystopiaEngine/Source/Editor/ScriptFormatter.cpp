/* HEADER *********************************************************************************/
/*!
\file	ScriptFormatter.cpp
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#include "DataStructure\AutoArray.h"
#include "Editor\ScriptFormatter.h"
#include "System/Driver/Driver.h"
#include "System/File/FileSystem.h"
#include "Utility/GUID.h"
#include <algorithm>
#include <fstream>
#include <Windows.h>
#include <iostream>

static const std::string ProjectFolder = "..\\DystopiaEngine";
static const std::string ResourceFolder = "Resource";
static const std::string BehavioursFolder = "Behaviours\\BehavioursScripts";
static const std::string Default_Creation_Folder = "\\" + BehavioursFolder;
static const std::string TemplateBodyPath   = ProjectFolder + "\\" + ResourceFolder + "\\BehaviourFormat\\BehaviourScript.cpp";
static const std::string TemplateHeaderPath = ProjectFolder + "\\" + ResourceFolder + "\\BehaviourFormat\\BehaviourScript.h";
static constexpr int tokenCount = 5;
static const std::string TokensToReplace[tokenCount] =
{
	"_SF_ClassName_",
	"SF_DefineScriptName",
	"_SF_CreatorName_",
	"_SF_CreatorLogin_",
	"_SF_UID_"
};

namespace Dystopia
{	
	BOOL MakeFolder(const std::wstring& _foldPath)
	{
		return CreateDirectory(_foldPath.c_str(), NULL);
	}

	std::ofstream MakeFile(const std::string& _fullPath)
	{
		return std::ofstream{ _fullPath.c_str(), std::ios::out };
	}

	std::string FindAndReplaceLine(const std::string& _line, const std::string& _toReplace, const std::string& _withString)
	{
		std::string result = _line;
		size_t foundPos = result.find(_toReplace, 0);
		size_t end;
		while (foundPos != std::string::npos)
		{
			end = foundPos + _toReplace.length();
			result.erase(result.begin() + foundPos, result.begin() + end);
			result.insert(foundPos, _withString);
			foundPos = result.find(_toReplace, 0);
		}
		return result;
	}

	void ChangeStringInPath(const std::string& _path, const std::string (&_replace)[tokenCount], const std::string (&_with)[tokenCount], std::ofstream& _writeTo)
	{
		std::string line;
		std::ifstream stream{ _path, std::ios::in };
		unsigned int currentLine = 0;
		while (std::getline(stream, line))
		{
			currentLine++;
			for (unsigned int i = 0; i < tokenCount; ++i)
			{
				line = FindAndReplaceLine(line, _replace[i], _with[i]);
			}
			_writeTo << line << "\n";
		}
		stream.close();
	}

	bool GenerateScript(const std::string& _className, const std::string& _creatorName, const std::string& _creatorLogin)
	{
		std::string newFolderPath = Default_Creation_Folder;// + "\\" + _className;
		//std::wstring folder = std::wstring{ newFolderPath.begin(), newFolderPath.end() };

		//if (!MakeFolder(folder))  return false;	//Early out for already existing

		std::string filePath = EngineCore::GetInstance()->Get<FileSystem>()->GetProjectFolders<std::string>(eFileDir::eResource)  + newFolderPath + "\\" + _className;
		std::ofstream osFileC = MakeFile(filePath + ".cpp");
		std::ofstream osFileH = MakeFile(filePath + ".h");

		const std::string replacers[tokenCount] = { _className, 
													_className,
													_creatorName, 
													_creatorLogin,
													std::to_string(GUIDGenerator::GetUniqueID())};

		ChangeStringInPath(TemplateBodyPath, TokensToReplace, replacers, osFileC);
		ChangeStringInPath(TemplateHeaderPath, TokensToReplace, replacers, osFileH);
		osFileC.close();
		osFileH.close();
		return true;
	}
}








#endif //EDITOR



