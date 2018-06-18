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
#include <string>
#include <vector>
 
static const std::string GLOBAL_DEFAULT_PROJECT_PATH = "..";

namespace Dystopia
{
	class ResourceView
	{
	public:
		ResourceView();
		~ResourceView();

		void Init();
		void Update(const float&);
		void Shutdown();

		void StartCrawl();
		void Window();
		void SetWidth(float);
		void SetHeight(float);


		struct CrawlFile
		{
			CrawlFile(const std::string&);
			std::string mFileName;
		};

		struct CrawlFolder
		{
			CrawlFolder(const char*);
			CrawlFolder(const std::string&);
			~CrawlFolder();

			void SetParent(CrawlFolder*);
			void AddFolder(CrawlFolder*);
			void AddFile(const std::string&);
			void Crawl();
			void PrintAll();
			bool FileExist(const std::string&);
			bool FolderExist(const std::string&);

			std::string mFolderName;
			CrawlFolder *mpParentFolder;
			AutoArray<CrawlFolder*> mArrChildFolders;
			AutoArray<CrawlFile> mArrFiles;
		};

	private:
		CrawlFolder *mpCrawlData;
		CrawlFolder *mpCurrentFolder;
		int mSelectedID;
		float mWidth;
		float mHeight;
		void *mpFocusData;
	};
}

#endif //_RESOURCE_VIEW_H_
#endif //EDITOR





