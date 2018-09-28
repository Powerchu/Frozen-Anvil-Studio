/* HEADER *********************************************************************************/
/*!
\file	Gizmo.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#include "Editor/Gizmo.h"
#include "Editor/EGUI.h"
#include "Editor/Editor.h"
#include "Object/GameObject.h"

static Math::Vec2 g_Axis2D[2] = 
{ 
	{ 1.f, 0.f },	//x-axis
	{ 0.f, 1.f }	//y-axis
};

void EGUI::Gizmo2D::SetReferenceAxis(const Math::Vec2& _xAxis, const Math::Vec2&  _yAxis)
{
	g_Axis2D[0] = _xAxis;
	g_Axis2D[1] = _yAxis;
}

bool EGUI::Gizmo2D::Translate(Math::Vec4& _outputVec, int _size)
{
	_outputVec;
	_size;
	return false;
}

bool EGUI::Gizmo2D::Scale(Math::Vec4& _outputVec, int _size)
{
	_outputVec;
	_size;
	return false;
}


#endif // EDITOR
