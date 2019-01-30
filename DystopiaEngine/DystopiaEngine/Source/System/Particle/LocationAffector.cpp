/* HEADER *********************************************************************************/
/*!
\file	LocationAffector.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

		location affector bytes and bits:

		data[0,1,2,3] used for float dt
		data[4,5] used for dimensions (radius) - circle
		data[6,7] used for dimensions (height) - rect
		data[8,9] used for dimensions (width) - rect
		data[10,11] used for dimensions (triangle edge) - triangle
		data[12,13] used for dimensions (star edge) - star
		data[14,15] used for rotation rate

		reserved[0] used for flags
			- 1 << 0 use surface of shapes
			- 1 << 1 use circle
			- 1 << 2 use rectangle
			- 1 << 3 use triangle
			- 1 << 4 use star
			- 1 << 5 use rotation
			- 1 << 6 rotation direction 1 cw, 0 anti-cw


All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System/Particle/LocationAffector.h"

#include "Component/Emitter.h"

#include "Math/MathLib.h"

#include <random>

#if EDITOR
#include "Editor/EGUI.h"
#endif 

Dystopia::LocationAffector::LocationAffector(void)
	: ParticleAffector{ &LocationAffector::AffectorSpawn }
{
	memset(reinterpret_cast<void*>(data), 0, 16);
	memset(reinterpret_cast<void*>(reserved), 0, 4);
}

Dystopia::LocationAffector::~LocationAffector(void)
{
}

void Dystopia::LocationAffector::SetOnSurface(bool _surfaceOnly)
{
	if (_surfaceOnly)
		reserved[0] |= (1 << 0);
	else
		reserved[0] &= ~(1 << 0);
}

/* circle */
void Dystopia::LocationAffector::ToggleCircle(void)
{
	reserved[0] |= (1 << 1);
	reserved[0] &= ~(1 << 2);
	reserved[0] &= ~(1 << 3);
	reserved[0] &= ~(1 << 4);
}

void Dystopia::LocationAffector::SetRadius(unsigned short _val)
{
	*reinterpret_cast<unsigned short*>(data + 4) = _val;
}

/* rectangle */
void Dystopia::LocationAffector::ToggleRectangle(void)
{
	reserved[0] &= ~(1 << 1);
	reserved[0] |= (1 << 2);
	reserved[0] &= ~(1 << 3);
	reserved[0] &= ~(1 << 4);
}

void Dystopia::LocationAffector::SetHeight(unsigned short _val)
{
	*reinterpret_cast<unsigned short*>(data + 6) = _val;
}

void Dystopia::LocationAffector::SetWidth(unsigned short _val)
{
	*reinterpret_cast<unsigned short*>(data + 8) = _val;
}

/* triangle */
void Dystopia::LocationAffector::ToggleTriangle(void)
{
	reserved[0] &= ~(1 << 1);
	reserved[0] &= ~(1 << 2);
	reserved[0] |= (1 << 3);
	reserved[0] &= ~(1 << 4);
}

void Dystopia::LocationAffector::SetTriangleEdge(unsigned short _val)
{
	*reinterpret_cast<unsigned short*>(data + 10) = _val;
}

/* star */
void Dystopia::LocationAffector::ToggleStar(void)
{
	reserved[0] &= ~(1 << 1);
	reserved[0] &= ~(1 << 2);
	reserved[0] &= ~(1 << 3);
	reserved[0] |= (1 << 4);
}

void Dystopia::LocationAffector::SetStarEdge(unsigned short _val)
{
	*reinterpret_cast<unsigned short*>(data + 12) = _val;
}

void Dystopia::LocationAffector::TogglePoint(void)
{
	reserved[0] &= ~(1 << 1);
	reserved[0] &= ~(1 << 2);
	reserved[0] &= ~(1 << 3);
	reserved[0] &= ~(1 << 4);
}

void Dystopia::LocationAffector::AffectorSpawn(Emitter& _emitter, float)
{
	// check circle mode 
	if (reserved[0] & (1 << 1))
	{
		Circle(_emitter);
	}
	// check rect mode 
	else if (reserved[0] & (1 << 2))
	{
		Rectangle(_emitter);
	}
	// check tri mode 
	else if (reserved[0] & (1 << 3))
	{
		Triangle(_emitter);
	}
	// check star mode 
	else if (reserved[0] & (1 << 4))
	{
		Star(_emitter);
	}
	
	// else its a point
}

void Dystopia::LocationAffector::Circle(Emitter& _emitter)
{
	auto& particle = _emitter.GetSpawnDefaults();
	std::random_device rDev;
	std::mt19937 gen{ rDev() };
	std::uniform_int_distribution<unsigned short> distr{ 0, 360 };

	unsigned short randomAngle = distr(gen);
	float rads = Math::Degrees{ static_cast<float>(randomAngle) }.Radians();
	if (reserved[0] & (1 << 0)) // if surface only
	{
		particle.mPos.x = sin(rads) * static_cast<float>(*reinterpret_cast<unsigned short*>(data + 4));
		particle.mPos.y = cos(rads) * static_cast<float>(*reinterpret_cast<unsigned short*>(data + 4));
	}
	else
	{
		std::uniform_int_distribution<unsigned short> distr2{ 0, *reinterpret_cast<unsigned short*>(data + 4) };
		particle.mPos.x = sin(rads) * static_cast<float>(distr2(gen));
		particle.mPos.y = cos(rads) * static_cast<float>(distr2(gen));
	}
}

void Dystopia::LocationAffector::Rectangle(Emitter& _emitter)
{
	auto& particle = _emitter.GetSpawnDefaults();
	std::random_device rDev;
	std::mt19937 gen{ rDev() };

	if (reserved[0] & (1 << 0)) // if surface only
	{
		int len = (*reinterpret_cast<unsigned short*>(data + 8) * 2) + (*reinterpret_cast<unsigned short*>(data + 6) * 2);
		std::uniform_real_distribution<float> disL{ 0.f,  static_cast<float>(len) };
		float lineP = disL(gen);
		if (lineP <= static_cast<float>(*reinterpret_cast<unsigned short*>(data + 6)))
		{
			particle.mPos.x = 0 - (static_cast<float>(*reinterpret_cast<unsigned short*>(data + 8)) * 0.5f);
			particle.mPos.y = static_cast<float>(lineP) - (static_cast<float>(*reinterpret_cast<unsigned short*>(data + 6)) * 0.5f);
		}
		else if (lineP <= static_cast<float>(*reinterpret_cast<unsigned short*>(data + 6) + *reinterpret_cast<unsigned short*>(data + 8)))
		{
			particle.mPos.x = static_cast<float>(lineP) - (static_cast<float>(*reinterpret_cast<unsigned short*>(data + 8)) * 0.5f);
			particle.mPos.y = 0 - (static_cast<float>(*reinterpret_cast<unsigned short*>(data + 6)) * 0.5f);
		}
		else if (lineP <= static_cast<float>((*reinterpret_cast<unsigned short*>(data + 6) * 2) + *reinterpret_cast<unsigned short*>(data + 8)))
		{
			particle.mPos.x = static_cast<float>(*reinterpret_cast<unsigned short*>(data + 8)) * 0.5f;
			particle.mPos.y = static_cast<float>(lineP) - (static_cast<float>(*reinterpret_cast<unsigned short*>(data + 6)) * 0.5f);
		}
		else if (lineP <= len)
		{
			particle.mPos.x = static_cast<float>(lineP) - (static_cast<float>(*reinterpret_cast<unsigned short*>(data + 8)) * 0.5f);
			particle.mPos.y = static_cast<float>(*reinterpret_cast<unsigned short*>(data + 6)) * 0.5f;
		}
	}
	else
	{
		std::uniform_int_distribution<unsigned short> disX{ 0, *reinterpret_cast<unsigned short*>(data + 8) };
		std::uniform_int_distribution<unsigned short> disY{ 0, *reinterpret_cast<unsigned short*>(data + 6) };
		particle.mPos.x = static_cast<float>(disX(gen)) - (static_cast<float>(*reinterpret_cast<unsigned short*>(data + 8)) * 0.5f);
		particle.mPos.y = static_cast<float>(disY(gen)) - (static_cast<float>(*reinterpret_cast<unsigned short*>(data + 6)) * 0.5f);
	}
}

void Dystopia::LocationAffector::Triangle(Emitter&)
{
	if (reserved[0] & (1 << 0)) // if surface only
	{

	}
	else
	{

	}
}

void Dystopia::LocationAffector::Star(Emitter&)
{
	if (reserved[0] & (1 << 0)) // if surface only
	{

	}
	else
	{

	}
}

const char * Dystopia::LocationAffector::EditorDisplayLabel(void) const
{
	return "Location";
}

void Dystopia::LocationAffector::EditorUI(void)
{
#if EDITOR
	EGUI::PushLeftAlign(80.f);
	EGUI::PushID(52);

	bool surfaceOnly = reserved[0] & (1 << 0);
	if (EGUI::Display::CheckBox("Surface", &surfaceOnly))
		SetOnSurface(surfaceOnly);

	bool circ = reserved[0] & (1 << 1);
	if (EGUI::Display::CheckBox("Circle", &circ))
		ToggleCircle();

	int out = static_cast<int>(*reinterpret_cast<unsigned short*>(data + 4));
	if (EGUI::Display::DragInt("Radius", &out, 1.f, 0, 0xffff))
		SetRadius(static_cast<short>(out));

	bool rect = reserved[0] & (1 << 2);
	if (EGUI::Display::CheckBox("Rectangle", &rect))
		ToggleRectangle();

	out = static_cast<int>(*reinterpret_cast<unsigned short*>(data + 8));
	if (EGUI::Display::DragInt("X", &out, 1.f, 0, 0xffff))
		SetWidth(static_cast<short>(out));

	out = static_cast<int>(*reinterpret_cast<unsigned short*>(data + 6));
	if (EGUI::Display::DragInt("Y", &out, 1.f, 0, 0xffff))
		SetHeight(static_cast<short>(out));

	bool tri = reserved[0] & (1 << 3);
	if (EGUI::Display::CheckBox("Triangle", &tri))
		ToggleTriangle();

	out = static_cast<int>(*reinterpret_cast<unsigned short*>(data + 10));
	if (EGUI::Display::DragInt("T Edge", &out, 1.f, 0, 0xffff))
		SetTriangleEdge(static_cast<short>(out));

	bool star = reserved[0] & (1 << 4);
	if (EGUI::Display::CheckBox("Star", &star))
		ToggleStar();

	out = static_cast<int>(*reinterpret_cast<unsigned short*>(data + 12));
	if (EGUI::Display::DragInt("S Edge", &out, 1.f, 0, 0xffff))
		SetStarEdge(static_cast<short>(out));

	bool point = !(reserved[0] & (1 << 1) || reserved[0] & (1 << 2) || reserved[0] & (1 << 3) || reserved[0] & (1 << 4));
	if (EGUI::Display::CheckBox("Point", &point))
		TogglePoint();

	EGUI::PopID();
	EGUI::PopLeftAlign();
#endif 
}