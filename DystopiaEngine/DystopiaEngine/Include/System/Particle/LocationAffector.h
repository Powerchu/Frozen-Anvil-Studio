/* HEADER *********************************************************************************/
/*!
\file	LocationAffector.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _LOCATION_AFFECTOR_H_
#define _LOCATION_AFFECTOR_H_
#include "ParticleAffector.h"

namespace Dystopia
{
	/*
		location affector bytes and bits:

		data[0,1,2,3] used for float dt
		data[4,5] used for dimensions (radius) - circle 
		data[6,7] used for dimensions (height) - rect
		data[8,9] used for dimensions (width) - rect
		data[10,11] used for dimensions (triangle edge) - triangle
		data[12,13] used for dimensions (star edge) - star

		reserved[0] used for flags
			- 1 << 0 use surface of shapes
			- 1 << 1 use circle
			- 1 << 2 use rectangle
			- 1 << 3 use triangle
			- 1 << 4 use star
	*/

	struct LocationAffector : ParticleAffector
	{
		LocationAffector(void);
		~LocationAffector(void);

		/* surface */
		void SetOnSurface(bool _surfaceOnly);

		/* circle */
		void ToggleCircle(void);
		void SetRadius(unsigned short);

		/* rectangle */
		void ToggleRectangle(void);
		void SetHeight(unsigned short);
		void SetWidth(unsigned short);

		/* triangle */
		void ToggleTriangle(void);
		void SetTriangleEdge(unsigned short);

		/* star */
		void ToggleStar(void);
		void SetStarEdge(unsigned short);

		/* override */
		void TogglePoint(void);

		void AffectorSpawn(Emitter&, float);
		void Circle(Emitter&);
		void Rectangle(Emitter&);
		void Triangle(Emitter&);
		void Star(Emitter&);

		const char * EditorDisplayLabel(void) const;
		void EditorUI(void);
	};

}

#endif 






