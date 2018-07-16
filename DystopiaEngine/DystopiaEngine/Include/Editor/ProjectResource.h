/* HEADER *********************************************************************************/
/*!
\file	ProjectResource.h
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
#ifndef _PROJECT_RESOURCE_H_
#define _PROJECT_RESOURCE_H_
#include "DataStructure\AutoArray.h"
#include "EditorTab.h"
#include "Math\Vector4.h"
#include <string>
#include <vector>

namespace Dystopia
{
	struct File;
	struct CrawlItem
	{
		CrawlItem(const std::string& _name, const std::string& _path);
		std::string mName;
		std::string mPath;
	};

	struct Folder : CrawlItem
	{
		Folder(const std::string& _name, const std::string& _path, Folder * const _parent);
		~Folder();

		void				Crawl();
		void				ClearFolder();
		Folder*				mpParentFolder;
		AutoArray<File*>	mArrPtrFiles;
		AutoArray<Folder*>	mArrPtrFolders;
		bool				mToggle;
	};

	struct File : CrawlItem
	{
		File(const std::string& _name, const std::string& _path, Folder * const _parent);
		~File();

		Folder*	mpParentFolder;
	};

	static constexpr size_t MAX_SEARCH = 64;
	class ProjectResource : public EditorTab
	{
	public:
		ProjectResource();
		~ProjectResource();

		/* Init() is called immediately after the creation of the object */
		virtual void Init() override;

		/* Update() is called before Window(), so alter most variables (frame based) here to be printed in Window() later */
		virtual void Update(const float&) override;

		/* Window() is where you do the EGUI/IMGUI functions. GUI variable changes will be recorded here */
		virtual void Window() override;

		/* Shutdown() is called right before deleting this object */
		virtual void Shutdown() override;

		/* GetLabel() returns the string to identify this class. EditorTab requires this to create a tab for you using the label */
		virtual std::string GetLabel() const override;

		void FindFile(Folder* _startFromFolder, AutoArray<File*>& _outResult, const std::string& _item);

	private:
		std::string		mLabel;
		char			mSearchText[MAX_SEARCH];
		Folder			*mpRootFolder;
		Folder			*mpCurrentFolder;

		void			FullCrawl(Folder*);
		void			FolderUI(Folder*);
		void			FileUI(File*);
	};
}

#endif //_PROJECT_RESOURCE_H_
#endif //EDITOR




