/* HEADER *********************************************************************************/
/*!
\file	MeshSystem.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Manages the creation, loading and destruction of Meshes.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System\Graphics\MeshSystem.h"		// File Header
#include "System\Graphics\Mesh.h"
#include "System\Graphics\RawMesh.h"
#include "System\Graphics\VertexDefs.h"
#include "IO\TextSerialiser.h"
#include "Math\MathUtility.h"

#include <GL\glew.h>
#include <iostream>


void Dystopia::MeshSystem::Init(void)
{
	Mesh::LinkSystem(this);
//	mpMeshes.reserve(10);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
}

void Dystopia::MeshSystem::Shutdown(void) noexcept
{
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	FreeMeshes();
}

void Dystopia::MeshSystem::StartMesh(void)
{
	mpRawMeshes.EmplaceBack();
}

void Dystopia::MeshSystem::LoadMesh(const std::string& _strPath)
{
	RawMesh& CurrentMesh = mpRawMeshes.back();
	TextSerialiser input = Serialiser::OpenFile<TextSerialiser>(_strPath, Serialiser::MODE_READ);

	unsigned short nVtxCount = 0;
	input.Read(nVtxCount);

	mUVs.reserve(nVtxCount);
	mVtx.reserve(nVtxCount * 2);
	mIndex.reserve(nVtxCount * 3);

	input.ConsumeStartBlock();

	Vertex vtxBuf;
	Vertex normBuf;
	UV uvBuf;

	for (unsigned short n = 0; n < nVtxCount; ++n)
	{
		input >> vtxBuf;
		input >> normBuf;
		input >> uvBuf;

		mVtx.EmplaceBack(vtxBuf);
		mVtx.EmplaceBack(normBuf);
		mUVs.EmplaceBack(uvBuf);
	}

	input.ConsumeStartBlock();

	do
	{
		unsigned nNumIndices = 0, nCurrOffset = static_cast<unsigned>(mIndex.size());
		while (!input.EndOfInput())
		{
			mIndex.EmplaceBack();
			input >> mIndex.back();

			++nNumIndices;
		}

		auto pCurrMesh = mpMeshes.Emplace(CurrentMesh.mVAO, nNumIndices, nCurrOffset);
		CurrentMesh.mVtxCount += nVtxCount;

		input.ConsumeStartBlock();

		input >> const_cast<std::string&>(pCurrMesh->GetName());

		input.ConsumeStartBlock();
	} while (!input.EndOfInput());
}

void Dystopia::MeshSystem::EndMesh(void)
{
	mpRawMeshes.back().BuildMesh(mVtx, mUVs, mIndex);
	
	mVtx.clear();
	mUVs.clear();
	mIndex.clear();
}

void Dystopia::MeshSystem::FreeMeshes(void)
{
	mpMeshes.clear();
	mpRawMeshes.clear();
}

void Dystopia::MeshSystem::ExportMeshes(void)
{

}

Dystopia::Mesh* Dystopia::MeshSystem::GetMesh(const std::string& _strName) noexcept
{
	for (auto& e : mpMeshes)
		if (e.GetName() == _strName)
			return &e;

	return nullptr;
}


