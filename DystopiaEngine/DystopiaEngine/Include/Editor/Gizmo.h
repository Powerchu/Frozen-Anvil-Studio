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
#include "Editor/DragStatus.h"

namespace EGUI
{
	namespace Gizmo2D
	{
		eDragStatus ArrowLeft(const std::string& _uniqueID, float& _outputValX, const Math::Vec2& _origin = Math::Vec2{ 0,0 }, float _vSpeed = 1.f, 
							  const Math::Vec4& _col = Math::Vec4{ 1,1,1,1 }, bool* _pHovered = nullptr, float _len = 40.f, float _thickness = 12.f);

		eDragStatus ArrowUp(const std::string& _uniqueID, float& _outputValY, const Math::Vec2& _origin = Math::Vec2{ 0,0 }, float _vSpeed = 1.f,
							const Math::Vec4& _col = Math::Vec4{ 1,1,1,1 }, bool* _pHovered = nullptr, float _len = 40.f, float _thickness = 12.f);

		eDragStatus Box(const std::string& _uniqueID, float& _outputValX, float& _outputValY, const Math::Vec2& _origin = Math::Vec2{ 0,0 }, 
						float _vSpeed = 1.f, const Math::Vec4& _col = Math::Vec4{ 1,1,1,1 }, bool* _pHovered = nullptr, float _side = 15.f);
	}
}

#endif // _GIZMO_H_
#endif // EDITOR

