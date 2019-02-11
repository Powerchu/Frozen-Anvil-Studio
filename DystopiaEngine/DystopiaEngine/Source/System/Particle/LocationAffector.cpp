/* HEADER *********************************************************************************/
/*!
\file	LocationAffector.cpp
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

		bytes and bits:

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

#include "System/Particle/Emitter.h"

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

unsigned short Dystopia::LocationAffector::GetRadius(void) const
{
	return *reinterpret_cast<const unsigned short*>(data + 4);
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

unsigned short Dystopia::LocationAffector::GetHeight(void) const
{
	return *reinterpret_cast<const unsigned short*>(data + 6);
}

unsigned short Dystopia::LocationAffector::GetWidth(void) const
{
	return *reinterpret_cast<const unsigned short*>(data + 8);
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

unsigned short Dystopia::LocationAffector::GetTEdge(void) const
{
	return *reinterpret_cast<const unsigned short*>(data + 10);
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

unsigned short Dystopia::LocationAffector::GetSEdge(void) const
{
	return *reinterpret_cast<const unsigned short*>(data + 12);
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
		particle.mPos.x = sin(rads) * static_cast<float>(GetRadius());
		particle.mPos.y = cos(rads) * static_cast<float>(GetRadius());
	}
	else
	{
		std::uniform_int_distribution<unsigned short> distr2{ 0, GetRadius() };
		particle.mPos.x = sin(rads) * static_cast<float>(distr2(gen));
		particle.mPos.y = cos(rads) * static_cast<float>(distr2(gen));
	}
}

void Dystopia::LocationAffector::Rectangle(Emitter& _emitter)
{
	auto& particle = _emitter.GetSpawnDefaults();
	std::random_device rDev;
	std::mt19937 gen{ rDev() };

	std::uniform_int_distribution<unsigned short> disX{ 0, GetWidth() };
	std::uniform_int_distribution<unsigned short> disY{ 0, GetHeight() };

	if (reserved[0] & (1 << 0)) // if surface only
	{
		std::uniform_int_distribution<unsigned short> distEdge{ 0, 3 };
		unsigned short edge = distEdge(gen);
		float x = 0.f, y = 0.f;
		switch (edge)
		{
		case 0:
			// use x=0, y=?
			x = 0-(static_cast<float>(GetWidth()) * 0.5f);
			y = static_cast<float>(disY(gen)) - (static_cast<float>(GetHeight()) * 0.5f);
			break;
		case 1:
			// use x=1, y=?
			x = static_cast<float>(GetWidth()) * 0.5f;
			y = static_cast<float>(disY(gen)) - (static_cast<float>(GetHeight()) * 0.5f);
			break;
		case 2:
			// use x=?, y=0
			x = static_cast<float>(disX(gen)) - (static_cast<float>(GetWidth()) * 0.5f);
			y = 0-(static_cast<float>(GetHeight()) * 0.5f);
			break;
		case 3:
			// use x=?, y=1
			x = static_cast<float>(disX(gen)) - (static_cast<float>(GetWidth()) * 0.5f);
			y = static_cast<float>(GetHeight()) * 0.5f;
			break;
		}
		particle.mPos.x = x;
		particle.mPos.y = y;
	}
	else
	{
		particle.mPos.x = static_cast<float>(disX(gen)) - (static_cast<float>(GetWidth()) * 0.5f);
		particle.mPos.y = static_cast<float>(disY(gen)) - (static_cast<float>(GetHeight()) * 0.5f);
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
	EGUI::PushLeftAlign(100.f);
	EGUI::PushID(52);

	bool surfaceOnly = reserved[0] & (1 << 0);
	if (EGUI::Display::CheckBox("Surface", &surfaceOnly))
		SetOnSurface(surfaceOnly);

	bool circ = reserved[0] & (1 << 1);
	if (EGUI::Display::CheckBox("Circle", &circ))
		ToggleCircle();

	int out = static_cast<int>(GetRadius());
	if (EGUI::Display::DragInt("Radius", &out, 1.f, 0, 0xffff))
		SetRadius(static_cast<short>(out));

	bool rect = reserved[0] & (1 << 2);
	if (EGUI::Display::CheckBox("Rectangle", &rect))
		ToggleRectangle();

	unsigned short a = GetWidth();
	out = static_cast<int>(a);
	if (EGUI::Display::DragInt("Width", &out, 1.f, 0, 0xffff))
		SetWidth(static_cast<short>(out));

	out = static_cast<int>(GetHeight());
	if (EGUI::Display::DragInt("Height", &out, 1.f, 0, 0xffff))
		SetHeight(static_cast<short>(out));

	bool tri = reserved[0] & (1 << 3);
	if (EGUI::Display::CheckBox("Triangle", &tri))
		ToggleTriangle();

	out = static_cast<int>(GetTEdge());
	if (EGUI::Display::DragInt("T Edge", &out, 1.f, 0, 0xffff))
		SetTriangleEdge(static_cast<short>(out));

	bool star = reserved[0] & (1 << 4);
	if (EGUI::Display::CheckBox("Star", &star))
		ToggleStar();

	out = static_cast<int>(GetSEdge());
	if (EGUI::Display::DragInt("S Edge", &out, 1.f, 0, 0xffff))
		SetStarEdge(static_cast<short>(out));

	bool point = !(reserved[0] & (1 << 1) || reserved[0] & (1 << 2) || reserved[0] & (1 << 3) || reserved[0] & (1 << 4));
	if (EGUI::Display::CheckBox("Point", &point))
		TogglePoint();

	EGUI::PopID();
	EGUI::PopLeftAlign();
#endif 
}