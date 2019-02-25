/* HEADER *********************************************************************************/
/*!
\file	ShaderView.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _SHADER_VIEW_H_
#define _SHADER_VIEW_H_
#include "DataStructure/HashString.h"
#include "DataStructure/AutoArray.h"
#include "DataStructure/Array.h"
#include "Math/Vectors.h"
#include "Editor/EditorPanel.h"
#include "Lib/Gfx/Shaders.h"

namespace Dystopia
{
	class ShaderSystem;
}

namespace Editor
{
	class ShaderView : public EditorPanel
	{
	public:
		ShaderView(void);
		~ShaderView(void);

		void Load(void);
		bool Init(void);
		void Update(float);
		void EditorUI(void);
		void Shutdown(void);
		void Message(eEMessage);
		void SaveSettings(Dystopia::TextSerialiser& _out) const;
		void LoadSettings(Dystopia::TextSerialiser& _in);
		const HashString& GetLabel(void) const;

	private:

		HashString mLabel;

		Dystopia::ShaderSystem *mpShaderSystem;

		AutoArray<unsigned> mArrStageVert;
		AutoArray<unsigned> mArrStageFrag;
		AutoArray<unsigned> mArrStageGeo;
		Array<unsigned, 3> mArrIndexTracker;

		Array<bool, 3> mArrShowPopup;

		void ShaderUI(void);
		void ProgramUI(void);

		void StageVert(void);
		void StageFrag(void);
		void StageGeo(void);

		void Prompt(void);
		void MakeShaderProgram(::Gfx::ShaderStage _stage, const HashString& _name);
		void MakeShader(const HashString&);
	};
}


#endif 
#endif // EDITOR









