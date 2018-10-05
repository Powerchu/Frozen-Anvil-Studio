/* HEADER *********************************************************************************/
/*!
\file	Collider.h
\author Keith (70%)
Aaron (30%)
\par    email: keith.goh\@digipen.edu
email: m.chu\@digipen.edu
\brief
Collider2D for 2D Sprites.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef CONVEX_H
#define CONVEX_H

#include "Component/Collider.h"

namespace Dystopia
{
	class _DLL_EXPORT Convex : public Collider
	{
	public:

		using SYSTEM = CollisionSystem;

		unsigned GetComponentType(void) const
		{
			return Utility::MetaFind_t<Utility::Decay_t<Collider>, AllComponents>::value;
		};

#if EDITOR
		static const std::string GetCompileName(void) { return "Convex Collider"; }
		const std::string GetEditorName(void) const { return GetCompileName(); }
#endif
		static const eColliderType ColliderType = eColliderType::CONVEX;
		virtual const eColliderType GetColliderType(void) const override { return ColliderType; }

		/*Constructors*/

		/*Convex Default Constructor*/
		Convex(Math::Point3D const & _v3Offset = { 0,0,0,1 });

		/*Convex Constructor. Takes in a Container of Vertice*/
		template<typename Container = AutoArray<Vertice>>
		Convex(Container & _Container)
		{
			for (Vertice const & elem : _Container)
				mVertices.push_back(elem);
		}

		/*Load the Component*/
		void Load(void) override;
		/*Initialise the Component*/
		void Init(void) override;

		/* Update Check */
		void Update(float) override;

		/*Unload the Component*/
		void Unload(void) override;
		/*Duplicate the Component*/
		Convex* Duplicate() const override;

		// Editor UI
		void EditorUI(void) noexcept override;

		/*Serialise and Unserialise*/
		void Serialise(TextSerialiser&) const override;
		void Unserialise(TextSerialiser&) override;


		virtual ~Convex();

		Vertice GetFarthestPoint(const Math::Vec3D & _Dir)const;


		// Convex-Convex Collision Detection
		bool isColliding(Convex & _ColB);
		bool isColliding(Convex * const & _pColB);
		bool isColliding(Convex & _pColB, const Math::Vec3D & _v3Dir);

		// Convex-Circle Collision Detection
		bool isColliding(Circle & _ColB);
		bool isColliding(Circle * const & _pColB);


		/*Static Member Functions*/

		/*Support Function for getting the farthest point with relation to a Vector*/
		static Vertice GetFarthestPoint(const Convex & _ColA, const Math::Vec3D & _Dir);

		static Edge	   GetClosestEdge(AutoArray<Vertice> & _Simplex);

		static Math::Point3D Support(const Convex & _ColA,
			                         const Convex & _ColB,
			                         const Math::Vec3D & _Dir);

		static bool ContainOrigin(AutoArray<Vertice> & _Simplex, Math::Vec3D & _v3Dir);

		Math::Point3D Support(const Convex & _ColB,
			                  const Math::Vec3D & _Dir)const;

		AutoArray<Edge> GetConvexEdges() const;

	protected:
		CollisionEvent GetCollisionEvent(AutoArray<Vertice> _Simplex,	const Convex & _ColB);

		/*The vertices of the collider in the Collider Local Coordinate System*/
		AutoArray<Vertice>			mVertices;
		int							mNumPoints;
	private:
		//Math::Vector3D				mLastKnownScale;

		//EDITOR FUNCTIONS
		/*=================Editor Stuff=====================*/
#if EDITOR
		void eIsTriggerCheckBox();
		void ePositionOffsetVectorFields();
		void ePointVerticesVectorArray();
		void eSetScale();
		//INFO
		void eAttachedBodyEmptyBox();
		void eNumberOfContactsLabel();
		void eUseTransformScaleButton();

#endif // EDITOR

	};
}


#endif