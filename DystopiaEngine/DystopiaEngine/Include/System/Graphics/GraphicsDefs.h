/* HEADER *********************************************************************************/
/*!
\file	GraphicsDefs.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Contains the definitions for Graphics

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _GRAPHICS_DEFS_H_
#define _GRAPHICS_DEFS_H_

namespace Dystopia
{
	enum eGfxSettings : unsigned
	{
		GRAPHICS_MSAA				= 1 << 0,
		GRAPHICS_FXAA				= 1 << 1,
		GRAPHICS_COMPUTE			= 1 << 3,
		GRAPHICS_VSYNC				= 1 << 4,

		GRAPHICS_GEOG				= 1 << 12,
		GRAPHICS_TESS				= 1 << 13,

		GRAPHICS_ALL				= 0xFFFFFFFF
	};

	namespace Gfx
	{
		struct Viewport
		{
			float mnX, mnY;
			float mnWidth, mnHeight;

			bool operator== (const Viewport& _rhs) const
			{
				return (mnX == _rhs.mnX && mnY == _rhs.mnY
					&& mnWidth == _rhs.mnWidth && mnHeight == _rhs.mnHeight);
			}
		};

		struct ClippingPlane
		{
			float mnNear, mnFar;

			bool operator== (const ClippingPlane& _rhs) const
			{
				return (mnNear == _rhs.mnNear && mnFar == _rhs.mnFar);
			}
			
		};

		struct ViewAspect
		{
			float mnX, mnY;

			bool operator== (const ViewAspect& _rhs) const
			{
				return (mnX == _rhs.mnX && mnY == _rhs.mnY);
			}
		};

		struct AbsViewport
		{
			int mnX, mnY;
			int mnWidth, mnHeight;
		};
	}
}



#endif		// INCLUDE GUARD

