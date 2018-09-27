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
#include "Math/Vector2.h"
#include "Editor/EditorTab.h"

namespace Dystopia
{
	class BinarySerializer;
	class StyleScheme : public EditorTab
	{
	public:
		static StyleScheme* GetInstance();
		~StyleScheme();

		/* Init() is called immediately after the creation of the object */
		void		Init();
		/* Update() is called before Window(), so alter most variables (frame based) here to be printed in Window() later */
		void		Update(const float&);
		/* Window() is where you do the EGUI/IMGUI functions. GUI variable changes will be recorded here */
		void		EditorUI();
		/* Shutdown() is called right before deleting this object */
		void		Shutdown();
		/* GetLabel() returns the string to identify this class. EditorTab requires this to create a tab for you using the label */
		std::string GetLabel() const;

		void SaveSettings(BinarySerializer& _out) const;
		void LoadSettings(BinarySerializer& _in);

		void Apply() const;
		void Remove() const;

	private:
		StyleScheme();
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

		std::string								mLabel;
		Array<StyleVarsV, eSTYLE_DATA_V_LAST>	mArrVecData;
		Array<StyleVarsF, eSTYLE_DATA_F_LAST>	mArrFData;

		std::string ToName(eStyleDataFloats);
		std::string ToName(eStyleDataVectors);
	};
}


#endif // _STYLE_SCHEME_H_
#endif // EDITOR

