/* HEADER *********************************************************************************/
/*!
\file	Gizmo.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _GIZMO_H_
#define _GIZMO_H_
#include "Math/Vector4.h"
#include "Math/Vector2.h"

namespace Dystopia
{
	class GameObject;
}

namespace EGUI
{
	enum GizmoType
	{
		eTranslate,
		eScale,
		eRotate,

		eNone
	};

	namespace Gizmo2D
	{
		void SetReferenceAxis(const Math::Vec2& _xAxis, const Math::Vec2&  _yAxis);
		bool Translate(Math::Vec4& _outputVec, const int _size = 1);
		bool Scale(Math::Vec4& _outputVec, const int _size = 1);

		//bool Rotate();
	}
}

#endif // _GIZMO_H_
#endif // EDITOR

