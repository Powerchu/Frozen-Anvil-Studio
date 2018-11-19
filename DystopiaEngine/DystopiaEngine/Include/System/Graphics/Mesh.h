/* HEADER *********************************************************************************/
/*!
\file	Mesh.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Mesh

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _MESH_H_
#define _MESH_H_

#include "System/Graphics/RawMesh.h"

#include <string>


namespace Dystopia
{
	class MeshSystem;

	class Mesh
	{
	public:

		Mesh(void) noexcept;
		explicit Mesh(unsigned, unsigned, size_t) noexcept;

		void DrawMesh(int _nMode) const;
		void DrawMesh(int _nMode, unsigned _nCount) const;

		void SetName(const std::string&);
		const std::string& GetName(void) const;

		void SetIndices(unsigned, size_t);

		template <typename T>
		void UpdateBuffer(AutoArray<typename T::type> const&, bool _bFreq = false);
		template <typename T>
		void UpdateBufferRange(AutoArray<typename T::type> const&, void* _nOffset);

	private:

		unsigned mVAO, mnVertices;
		void* mnOffset;
		
		std::string mName;

		void UpdateMesh(RawMesh*);
	};
}






// ============================================ FUNCTION DEFINITIONS ============================================ // 


template <typename T>
void Dystopia::Mesh::UpdateBuffer(AutoArray<typename T::type> const& _buf, bool _bFreq)
{
	auto pRaw = EngineCore::GetInstance()->Get<MeshSystem>()->GetRawFromBuffer(mVAO);

	if (1 < pRaw->GetRefCount())
	{
		pRaw->DecRef();
		pRaw = pRaw->RequestDuplicate(mnVertices, mnOffset, T::value);
		UpdateMesh(pRaw);
	}

	pRaw->UpdateBuffer<T>(_buf, _bFreq);
}

template <typename T>
void Dystopia::Mesh::UpdateBufferRange(AutoArray<typename T::type> const& _buf, void* _nOffset)
{
	auto pMeshSys = EngineCore::GetInstance()->Get<MeshSystem>();
	auto pRaw = pMeshSys->GetRawFromBuffer(mVAO);

	if (1 < pRaw->GetRefCount())
	{
		pRaw->DecRef();
		pRaw = pRaw->RequestDuplicate(mnVertices, mnOffset, T::value);
		UpdateMesh(pRaw);
	}

	pRaw->UpdateBufferRange<T>(_buf, _nOffset);
}



#endif		// INCLUDE GUARD

