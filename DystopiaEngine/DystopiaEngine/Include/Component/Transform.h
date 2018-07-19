/* HEADER *********************************************************************************/
/*!
\file	Transform2D.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Component to store the information about the object's transformations in space.

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include "Component\Component.h"		// Base Class
#include "Component\ComponentList.h"	// TRANSFORM
#include "Math\Vector4.h"				// Vector4
#include "Math\Matrix4.h"				// Matrix4
#include "DataStructure\AutoArray.h"	// AutoArray

namespace Dystopia
{
	class Transform : public Component
	{
	public:
		static constexpr eComponents TYPE = eComponents::TRANSFORM;
		const eComponents GetComponentType(void) const { return TYPE; };


		// ====================================== CONSTRUCTORS ======================================= // 

		Transform(void);
		Transform(const Transform&);


		// ===================================== MEMBER FUNCTIONS ==================================== // 

		void SetGlobalPosition(const Math::Point3D&);
		void SetGlobalPosition(const float _x, const float _y, const float _z);

//		void SetRotation(const float _fRadians);
//		void SetRotationDeg(const float _fDegrees);

		void SetScale(const Math::Vec4& _vScale);
		void SetScale(const float _fScaleX, const float _fScaleY, const float _fScaleZ);

		void SetPosition(const Math::Point3D&);
		void SetPosition(const float _x, const float _y, const float _z);

		Math::Vec4 GetGlobalRotation(void) const;
//		Math::Vec4 GetGlobalRotationDeg(void) const;
		Math::Vec4 GetGlobalScale(void) const;
		Math::Point3D GetGlobalPosition(void) const;

		Math::Vec4 GetRotation(void) const;
//		Math::Vec4 GetRotationDeg(void) const;
		Math::Vec4 GetScale(void) const;
		Math::Point3D GetPosition(void) const;

		void SetParent(Transform*);
		void OnParentRemove(Transform*);

		Math::Matrix4 GetTransformMatrix(void);
		const Math::Matrix4& GetLocalTransformMatrix(void);

		Transform* Duplicate(void) const;

		void Serialise(TextSerialiser&) const;
		void Unserialise(TextSerialiser&);

	private:

		bool mbChanged;

		Transform* mpParent;
		AutoArray<Transform*> mChildren;

		Math::Matrix4 mMatrix;

		Math::Vec4 mRotation;
		Math::Vec4 mScale;
		Math::Point3D mPosition;
	};
}



#endif

