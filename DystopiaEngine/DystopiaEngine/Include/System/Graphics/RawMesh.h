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
#include "Utility/Utility.h"
#include "Utility/MetaDataStructures.h"


namespace Dystopia
{
	using VertexBuffer = Ut::Indexer<0, Gfx::Vertex>;
	using NormalBuffer = Ut::Indexer<1, Gfx::Normal>;
	using UVBuffer     = Ut::Indexer<2, Gfx::UV>;
	using IndexBuffer  = Ut::Indexer<3, short>;

	class RawMesh
	{
	public:
		RawMesh(void) noexcept;
		~RawMesh(void);

		void BindMesh  (void) const;
		void UnbindMesh(void) const;

		RawMesh* RequestDuplicate(unsigned _nVtxCount, size_t _nOffset, unsigned _nSkip = -1) const;

		void Build(
			const AutoArray<VertexBuffer::type>&,
			const AutoArray<NormalBuffer::type>&,
			const AutoArray<UVBuffer::type>&,
			const AutoArray<IndexBuffer::type>&
		);

		template <typename ... T>
		void BuildEmpty(void);

		template <typename T>
		void UpdateBuffer(AutoArray<typename T::type> const&, bool _bFreq = false);
		template <typename T>
		void UpdateBufferRange(AutoArray<typename T::type> const&, ptrdiff_t _nOffset);

		void IncRef(void);
		void DecRef(void);
		unsigned GetVAO(void) const;
		unsigned GetVtxCount(void) const;
		unsigned GetRefCount(void) const;
		void AppendVertexCount(unsigned);

	private:

		unsigned mVAO, mVtxBuffer, mNmlBuffer, mUVBuffer, mEBO;
		unsigned mnVtxCount, mnRefCount;

		void UpdateBuffer(unsigned, unsigned, void*, ptrdiff_t sz, bool _bFreq);
		void UpdateBufferRange(unsigned, unsigned, void*, ptrdiff_t sz, ptrdiff_t _offset);

		void InitBuffersEmpty(bool, bool, bool, bool);
	};
}

template <typename ... T>
void Dystopia::RawMesh::BuildEmpty(void)
{
	InitBuffersEmpty(
		Ut::MetaFind<Ut::Type<VertexBuffer>, Ut::Type<T>...>::value,
		Ut::MetaFind<Ut::Type<NormalBuffer>, Ut::Type<T>...>::value,
		Ut::MetaFind<Ut::Type<UVBuffer    >, Ut::Type<T>...>::value,
		Ut::MetaFind<Ut::Type<IndexBuffer >, Ut::Type<T>...>::value
	);
}

template <typename T>
void Dystopia::RawMesh::UpdateBuffer(AutoArray<typename T::type> const& _arr, bool _bFreq)
{
	using Real_t = typename T::type;
	if constexpr (T::value == 3)
	{
		// GL_ELEMENT_ARRAY_BUFFER
		UpdateBuffer(0x8893, (&mVtxBuffer)[T::value], _arr.begin(), _arr.size() * sizeof(Real_t), _bFreq);
	}
	else
	{
		// GL_ARRAY_BUFFER
		UpdateBuffer(0x8892, (&mVtxBuffer)[T::value], _arr.begin(), _arr.size() * sizeof(Real_t), _bFreq);
	}
}

template <typename T>
void  Dystopia::RawMesh::UpdateBufferRange(AutoArray<typename T::type> const&, ptrdiff_t _nOffset)
{
	if constexpr (T::value == 3)
	{
		// GL_ELEMENT_ARRAY_BUFFER
		UpdateBufferRange(0x8893, (&mVtxBuffer)[T::value], _arr.begin(), _arr.size(), _nOffset);
	}
	else
	{
		// GL_ARRAY_BUFFER
		UpdateBufferRange(0x8892, (&mVtxBuffer)[T::value], _arr.begin(), _arr.size(), _nOffset);
	}
}



#endif		// INCLUDE GUARD

