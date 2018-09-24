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

#include "System/Graphics/VertexDefs.h"
#include "System/Graphics/Mesh.h"
#include "System/Graphics/RawMesh.h"
#include "DataStructure/AutoArray.h"
#include "DataStructure/MagicArray.h"

#include <string>


namespace Dystopia
{
	class MeshSystem final
	{
	public:

		void Init(void);
		void Shutdown(void) noexcept;

		void StartMesh(void);
		void LoadMesh(const std::string&);
		void AddVertex(float x, float y, float z, float u = 0, float v = 0);
		Mesh* AddIndices(const std::string& _strName, const AutoArray<short>&);
		void EndMesh(void);

		void FreeMeshes(void);

		// Not implemented
		void ExportMeshes(void);

		Mesh* GetMesh(const std::string&) noexcept;

	private:

		AutoArray<UV>      mUVs;
		AutoArray<Vertex>  mVtx;
		AutoArray<short>   mIndex;

		MagicArray<Mesh>   mpMeshes;
		AutoArray<RawMesh> mpRawMeshes;
	};
}



#endif		// INCLUDE GUARD

