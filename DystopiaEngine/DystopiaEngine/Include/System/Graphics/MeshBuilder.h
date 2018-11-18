/* HEADER *********************************************************************************/
/*!
\file	MeshBuilder.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Proxy class that uses the builder pattern to create a mesh

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _MESHBUILDER_H_
#define _MESHBUILDER_H_


namespace Dystopia
{
	class RawMesh;

	class MeshBuilder
	{
	public:

		MeshBuilder(RawMesh&) noexcept;


	private:
		RawMesh& mMesh;
	};
}



#endif		// INCLUDE GUARD

