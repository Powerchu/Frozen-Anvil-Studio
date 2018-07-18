/* HEADER *********************************************************************************/
/*!
\file	Scene.cpp
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Scene

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#include "System\Scene\Scene.h"



Dystopia::Scene::Scene(void)
{
}

Dystopia::Scene::~Scene(void)
{
}

Dystopia::GameObject* Dystopia::Scene::FindGameObject(size_t _id)
{
	for (auto& e : mGameObjs)
		if (_id == e.GetID())
			return &e;

	return nullptr;
}

Dystopia::GameObject* Dystopia::Scene::FindGameObject(const std::string& _strName)
{
	for (auto& e : mGameObjs)
		if (_strName == e.GetName())
			return &e;

	return nullptr;
}
