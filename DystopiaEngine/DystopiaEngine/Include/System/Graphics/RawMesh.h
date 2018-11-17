/* HEADER *********************************************************************************/
/*!
\file	RawMesh.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	"Raw" Mesh Data

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _RAWMESH_H_
#define _RAWMESH_H_

#include "System/Graphics/VertexDefs.h"
#include "DataStructure/AutoArray.h"

namespace Dystopia
{
	struct RawMesh
	{
		RawMesh(void) noexcept;
		~RawMesh(void);

		void BindMesh  (void) const;
		void UnbindMesh(void) const;

		void Build(
			const AutoArray<Gfx::Vertex>&,
			const AutoArray<Gfx::Normal>&,
			const AutoArray<Gfx::UV>&,
			const AutoArray<short>&
		);

		void IncRef(void);
		void DecRef(void);
		unsigned GetVAO(void);
		unsigned GetVtxCount(void);

		unsigned mVAO, mVtxBuffer, mNmlBuffer, mUVBuffer, mEBO;
		unsigned mnVtxCount, mnRefCount;
	};
}



#endif		// INCLUDE GUARD

