/* HEADER *********************************************************************************/
/*!
\file	StyleScheme.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _STYLE_SCHEME_H_
#define _STYLE_SCHEME_H_
#include "Math/Vectors.h"
//#include "Editor/EditorTab.h"
#include "Editor/EditorPanel.h"

namespace Dystopia
{
	class TextSerialiser;
}


namespace Editor
{
	class TextSerialiser;
	class StyleScheme : public EditorPanel//EditorTab
	{
	public:
		//static StyleScheme* GetInstance();
		StyleScheme(void);
		~StyleScheme(void);

		void Load(void);
		bool Init(void);
		void Update(float);
		void EditorUI(void);
		void Shutdown(void);
		void Message(eEMessage);
		void SaveSettings(Dystopia::TextSerialiser& _out) const;
		void LoadSettings(Dystopia::TextSerialiser& _in);
		HashString GetLabel(void) const;

		void Apply(void) const;

	private:
		enum eStyleDataVectors
		{  
			eWINDOW_PADDING,         
			eWINDOW_MIN_SIZE,
			eWINDOW_TITLE_ALIGNMENT,     
			eFRAME_PADDING,           
			eITEM_SPACING,            
			eITEM_SPACING_INNER,         
			eBUTTON_TEXT_ALIGNMENT,      

			eSTYLE_DATA_V_LAST
		};
		enum eStyleDataFloats
		{
			eALPHA,
			eWINDOW_ROUNDING,
			eWINDOW_BORDER,
			eCHILD_ROUNDING,
			eCHILD_BORDER,
			ePOPUP_ROUNDING,
			ePOPUP_BORDER,
			eFRAME_ROUNDING,
			eFRAME_BORDER,
			eINDENT_SPACE,
			eSCROLL_SIZE,           
			eSCROLL_ROUNDING,
			eGRAB_SIZE,
			eGRAB_ROUNDING,

			eSTYLE_DATA_F_LAST
		};

		struct StyleVarsF
		{
			StyleVarsF(float _v = 0.f, float _min = 0.f, float _max = 1.f)
				: mValue{ _v }, mMin{ _min }, mMax{ _max }
			{}

			float mValue;
			float mMin;
			float mMax;
		};

		struct StyleVarsV
		{
			StyleVarsV(Math::Vec2 _v = { 0.f,0.f }, float _min = 0.f, float _max = 1.f)
				: mValue{ _v }, mMin{ _min }, mMax{ _max }
			{}

			Math::Vec2 mValue;
			float mMin;
			float mMax;
		};

		HashString								mLabel;
		Array<StyleVarsV, eSTYLE_DATA_V_LAST>	mArrVecData;
		Array<StyleVarsF, eSTYLE_DATA_F_LAST>	mArrFData;

		HashString ToName(eStyleDataFloats);
		HashString ToName(eStyleDataVectors);
	};
}


#endif // _STYLE_SCHEME_H_
#endif // EDITOR

