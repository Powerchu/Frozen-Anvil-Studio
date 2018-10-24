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

#include <string>

namespace Dystopia
{
	class MeshSystem;

	class Mesh
	{
	public:

		Mesh(void) noexcept;
		Mesh(unsigned, unsigned, size_t) noexcept;

		void DrawMesh(int _nMode) const;
		void DrawMesh(int _nMode, unsigned _nCount) const;

		void SetName(const std::string&);
		const std::string& GetName(void) const;

		bool IsUnique(void) const;

	private:

		bool mbUnique;
		unsigned mVAO, mnVertices;
		void* mnOffset;
		
		std::string mName;
	};
}



#endif		// INCLUDE GUARD

