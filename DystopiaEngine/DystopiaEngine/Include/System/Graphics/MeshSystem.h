/* HEADER *********************************************************************************/
/*!
\file	MeshSystem.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Manages the creation, loading and destruction of Meshes.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _MESHSYS_H_
#define _MESHSYS_H_

#include "DataStructure\AutoArray.h"

#include <string>


namespace Dystopia
{
	class Mesh;

	class MeshSystem final
	{
	public:

		void Init(void);
		void Shutdown(void) noexcept;

		void StartMesh(void);
		void LoadMesh(const std::string&);
		Mesh EndMesh(void);

		void FreeMeshes(void);


	private:

		AutoArray<struct Vertex>  vtx;
		AutoArray<struct Normal>  normals;
		AutoArray<struct UV>      uvs;
		AutoArray<struct RawMesh> mpMeshes;
	};
}





#endif		// INCLUDE GUARD

