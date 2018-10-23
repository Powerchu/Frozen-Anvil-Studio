/* HEADER *********************************************************************************/
/*!
\file	NeuralTree.h
\author Aaron Chu (100%)
\par    email: m.chu\@digipen.edu
\brief
Base class for all Neural Trees (Behaviour Tree). Referred to 
https://www.gamasutra.com/blogs/ChrisSimpson/20140717/221339/Behavior_trees_for_AI_How_they_work.php
for basic understanding.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/


#ifndef _NEURAL_TREE_H_
#define _NEURAL_TREE_H_

#include "Globals.h"
#include "DataStructure/AutoArray.h"
#include "DataStructure/SharedPtr.h"


namespace Dystopia
{
	namespace NeuralTree
	{
		class Node
		{
		public:
			enum class eStatus
			{
				Invalid = -1,
				Ready,
				Running,
				Success,
				Failure
			};

			virtual ~Node() {};

			virtual void Init() {};
			virtual eStatus Update(float) = 0;
			virtual void Terminate(eStatus) {};

			eStatus Tick(float);
		};



	}
}
	

#endif

