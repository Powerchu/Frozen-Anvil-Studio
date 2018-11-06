/* HEADER *********************************************************************************/
/*!
\file	EditorLauncher.h
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _EDITOR_LAUNCHER_H_
#define _EDITOR_LAUNCHER_H_
#include "DataStructure/HashString.h"
#include "DataStructure/AutoArray.h"

namespace Editor
{
	class EditorLauncher
	{
	public:
		EditorLauncher(void);
		~EditorLauncher(void);

		void Init(void);
		void StartFrame(void);
		void Update(float);
		void EndFrame(void);
		void Shutdown(void);

		bool IsClosing(void) const;

		HashString GetProjectPath(void) const;

	private:
		AutoArray<HashString> mArrProjectFolders;
		HashString mProjectSelected;
		bool mbClosing;
		unsigned long mOriginStyle;
		unsigned long mOriginStyleEx;
		int mOriginSizeX;
		int mOriginSizeY;
		int mCurrentlySelected;

		void SetWindowOptions(void);
		void RemoveWindowOptions(void);
		void FindAllProjects(void);
		void ProjectDetails(void);
	};

}

#endif 
#endif

























