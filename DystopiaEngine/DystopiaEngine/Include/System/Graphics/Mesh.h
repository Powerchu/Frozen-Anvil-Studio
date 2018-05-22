/* HEADER *********************************************************************************/
/*!
\file	Mesh.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Mesh

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _MESH_H_
#define _MESH_H_

#include <string>


namespace Dystopia
{
	class Mesh
	{
	public:

		Mesh(void) noexcept;

		void UseMesh(int _nMode) const;

		
	private:

		unsigned mVAO, mnVertices, mnOffset;
	};
}



#endif		// INCLUDE GUARD

