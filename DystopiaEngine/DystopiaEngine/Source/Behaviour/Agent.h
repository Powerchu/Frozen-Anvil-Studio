/* HEADER *********************************************************************************/
/*!
\file	Agent.h
\author Aaron Chu (100%)
\par    email: m.chu\@digipen.edu
\brief
Base class for all Neural Trees (Behaviour Tree). Referred to
https://www.gamasutra.com/blogs/ChrisSimpson/20140717/221339/Behavior_trees_for_AI_How_they_work.php
for basic understanding. Using Agents to share data context

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/

#ifndef _AGENT_H_
#define _AGENT_H_
#include "Object/GameObject.h"
#include "DataStructure/Pointer.h"

namespace Dystopia
{
	namespace NeuralTree
	{
		struct Agent
		{
			explicit Agent(GameObject * _gObj) 
				: pGobj{_gObj}
			{
				
			}

			Pointer<GameObject> pGobj;
			//struct tree tree;
		};
	}
}


#endif
