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


namespace Dystopia
{
	struct RawMesh
	{
		RawMesh(void) noexcept;
		~RawMesh(void);

		unsigned mVAO, mVtxBuffer, mUVBuffer, mEBO;
		unsigned short mVtxCount;
	};
}



#endif		// INCLUDE GUARD

