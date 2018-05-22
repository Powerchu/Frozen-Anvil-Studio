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
#include "System\Graphics\Mesh.h"	// File Header

#include <GL\glew.h>

Dystopia::Mesh::Mesh(void) noexcept
{

}

void Dystopia::Mesh::UseMesh(int _nMode) const
{
	glBindVertexArray(mVAO);
	glDrawElements(_nMode, mnVertices, GL_UNSIGNED_SHORT, 0);
}


