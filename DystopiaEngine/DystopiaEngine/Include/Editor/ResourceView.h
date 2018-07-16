/* HEADER *********************************************************************************/
/*!
\file	ResourceView.h
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
#ifndef _RESOURCE_VIEW_H_
#define _RESOURCE_VIEW_H_
#include "DataStructure\AutoArray.h"
#include "EditorTab.h"
#include "Math\Vector4.h"
#include <string>
#include <vector>

namespace Dystopia
{
	static constexpr size_t MAX_SEARCH_SIZE = 64;

	class ResourceView : public EditorTab
	{
	public:
		struct CrawlFolder;
		struct CrawlFile;

		ResourceView();
		~ResourceView();

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

		void FindFile(CrawlFolder *,const std::string&);

		struct CrawlFile
		{
			CrawlFile(const std::string& _name, const std::string& _path, CrawlFolder* _pParent);
			CrawlFile(const char* _name, const char* _path, CrawlFolder* _pParent);

			std::string		mFileName;
			std::string		mFilePath;
			CrawlFolder		*mpParentFolder;
		};

		struct CrawlFolder
		{
			CrawlFolder(const char* _name, const char* _path);
			CrawlFolder(const std::string& _name, const std::string& _path);
			~CrawlFolder();

			CrawlFolder*			GetParent();
			void					SetParent(CrawlFolder*);
			void					AddFolder(CrawlFolder*);
			void					AddFile(const std::string& _name, const std::string& _path);
			void					Crawl();
			void					PrintAll();

			std::string				mFolderName;
			std::string				mFolderPath;
			bool					mToggle;
			CrawlFolder				*mpParentFolder;
			AutoArray<CrawlFolder*>	mArrChildFolders;
			AutoArray<CrawlFile*>	mArrFiles;
			bool					mRefreshMe;
		};

	private:
		AutoArray<CrawlFile*>	mArrFileSearchResultPtrs;
		CrawlFolder				*mpRootFolder;
		CrawlFolder				*mpCurrentFolder;
		std::string				mLabel;
		char					mSearchBarText[MAX_SEARCH_SIZE];
		
		void					StartCrawl();
		void					ReCrawl(CrawlFolder*);
		void					CrawlAll(CrawlFolder*);
		void					FolderInterface(CrawlFolder*);
		void					FileInterface(CrawlFile&);
	};
}

#endif //_RESOURCE_VIEW_H_
#endif //EDITOR





