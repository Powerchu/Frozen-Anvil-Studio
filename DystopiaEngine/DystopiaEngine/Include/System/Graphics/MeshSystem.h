/* HEADER *********************************************************************************/
/*!
\file	MeshSystem.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Manages the creation, loading and destruction of Meshes.

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _MESHSYS_H_
#define _MESHSYS_H_

#include "System/Graphics/VertexDefs.h"
#include "System/Graphics/Mesh.h"
#include "System/Graphics/RawMesh.h"
#include "System/Graphics/MeshBuilder.h"
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
		void LoadMesh(std::string const&);
		void AddVertex(Gfx::Vertex const&);
		void AddVertex(float x, float y, float z);
		void AddNormal(Gfx::Normal const&);
		void AddNormal(float x, float y, float z);
		void AddUV(float u, float v);
		void AddUV(Gfx::UV const&);
		Mesh* AddIndices(const std::string& _strName, const AutoArray<short>&);
		void EndMesh(void);

		//MeshBuilder BuildMesh(void);

		void FreeMeshes(void);

		// Not implemented
		void ExportMeshes(void);

		Mesh* CreateMesh(RawMesh* _pRaw, unsigned _nIndices, size_t = 0);

		Mesh* GetMesh(const std::string&) noexcept;
		unsigned GenerateRaw(void) noexcept;

		// Warning! Do not keep the pointer!
		RawMesh* GetRaw(unsigned) noexcept;
		// Warning! Do not keep the pointer!
		RawMesh* GetRawFromBuffer(unsigned) noexcept;

	private:

		AutoArray<Gfx::UV>      mUVBuffer;
		AutoArray<Gfx::Vertex>  mVtxBuffer;
		AutoArray<Gfx::Normal>  mNormalBuffer;
		AutoArray<short>   mIndexBuffer;

		MagicArray<Mesh>   mpMeshes;
		AutoArray<RawMesh> mpRawMeshes;
	};
}



#endif		// INCLUDE GUARD

