/* HEADER *********************************************************************************/
/*!
\file	VertexDefs.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Contains the definitions for Mesh Vertices

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _VERTEX_DEFS_H_
#define _VERTEX_DEFS_H_

#include "IO/Serialiser.h"

namespace Dystopia::Gfx
{
	struct Vertex
	{
		float x, y, z;
	};

	using Normal = Vertex;

	struct UV
	{
		float u, v;
	};
}

inline std::fstream& operator >> (std::fstream& _lhs, Dystopia::Gfx::Vertex& _rhs)
{
	_lhs >> _rhs.x;
	_lhs.ignore(1) >> _rhs.y;
	_lhs.ignore(1) >> _rhs.z;

	return _lhs;
}

inline std::fstream& operator >> (std::fstream& _lhs, Dystopia::Gfx::UV& _rhs)
{
	_lhs >> _rhs.u;
	_lhs.ignore(1) >> _rhs.v;

	return _lhs;
}

inline std::fstream& operator << (std::fstream& _lhs, Dystopia::Gfx::Vertex& _rhs)
{
	_lhs << _rhs.x << ',' << _rhs.y << ',' << _rhs.z;

	return _lhs;
}

inline std::fstream& operator << (std::fstream& _lhs, Dystopia::Gfx::UV& _rhs)
{
	_lhs << _rhs.u << ',' << _rhs.v;

	return _lhs;
}



#endif		// INCLUDE GUARD

