/* HEADER *********************************************************************************/
/*!
\file	EClipboard.h
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _EDITOR_CLIPBOARD_H_
#define _EDITOR_CLIPBOARD_H_
#include "DataStructure/AutoArray.h"
#include "Editor/EditorSystem.h"

namespace Dystopia
{
	class GameObject;
}

namespace Editor
{
	class EditorClipboard : public EditorSystem
	{
	public:
		EditorClipboard(void);
		~EditorClipboard(void);

		void Load(void);
		bool Init(void);
		void StartFrame(void);
		void Update(float);
		void EndFrame(void);
		void Shutdown(void);
		void Message(eEMessage) ;
		void SaveSettings(Dystopia::TextSerialiser& _out) const;
		void LoadSettings(Dystopia::TextSerialiser& _in);

		void AddGameObject(const uint64_t&);
		void RemoveGameObject(const uint64_t&);
		void RemoveGameObjectP(const uint64_t&);
		void ClearAll(void);

		void Copy(void);
		void Delete(void);
		void Duplicate(void);
		void Paste(void);

		AutoArray<uint64_t>& GetSelectedIDs(void);

	private:
		AutoArray<uint64_t> mArrSelectedIDs;

		size_t mnCopyID;
		size_t mnPasteID;
		size_t mnDeleteID;
		size_t mnDupliID;
	};
}

#endif
#endif



