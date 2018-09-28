/* HEADER *********************************************************************************/
/*!
\file	Transform2D.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Component to store the information about the object's transformations in space.

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _TRANSFORM_H_
#define _TRANSFORM_H_

#include "Component/Component.h"		// Base Class
#include "Component/ComponentList.h"	// TRANSFORM

#include "DataStructure/AutoArray.h"	// AutoArray
#include "Utility/MetaAlgorithms.h"		// MetaFind

#include "Math/Angles.h"
#include "Math/Vector4.h"				// Vector4
#include "Math/Matrix4.h"				// Matrix4
#include "Math/Quaternion.h"			// Quaternion

namespace Dystopia
{
	class _DLL_EXPORT Transform : public Component
	{
	public:
		unsigned GetComponentType(void) const
		{
			return Utility::MetaFind_t<Utility::Decay_t<decltype(*this)>, AllComponents>::value; 
		};
		static const std::string GetCompileName(void) { return "Transform"; }
		const std::string GetEditorName(void) const { return GetCompileName(); }

		// ====================================== CONSTRUCTORS ======================================= // 

		explicit Transform(GameObject* _pOwner = nullptr) noexcept;
		Transform(const Transform&);


		// ===================================== MEMBER FUNCTIONS ==================================== // 

		void SetGlobalPosition(const Math::Point3D&);
		void SetGlobalPosition(const float _x, const float _y, const float _z);

		void SetRotation(const Math::Angle _x, const Math::Angle _y = Math::Radians{ 0 }, const Math::Angle _z = Math::Radians{ 0 });

		void SetScale(const Math::Vec4& _vScale);
		void SetScale(const float _fScaleX, const float _fScaleY, const float _fScaleZ);

		void SetPosition(const Math::Point3D&);
		void SetPosition(const float _x, const float _y, const float _z);

		Math::Vec4 GetGlobalScale(void) const;
		Math::Point3D GetGlobalPosition(void) const;
		Math::Quaternion GetGlobalRotation(void) const;

		Math::Vec4 GetScale(void) const;
		Math::Point3D GetPosition(void) const;
		Math::Quaternion GetRotation(void) const;

		void SetParent(Transform*);
		void OnChildRemove(Transform*);
		void OnParentRemove(Transform*);

		Math::Matrix4 GetTransformMatrix(void);
		const Math::Matrix4& GetLocalTransformMatrix(void);

		Transform* Duplicate(void) const;

		void Serialise(TextSerialiser&) const override;
		void Unserialise(TextSerialiser&) override;

		void EditorUI(void) noexcept override;

	private:

		bool mbChanged;

		Transform* mpParent;
		AutoArray<Transform*> mChildren;

		Math::Matrix4 mMatrix;

		Math::Vec3D mScale;
		Math::Point3D mPosition;
		Math::Quaternion mRotation;

		void RemoveChild(Transform*);
		void RemoveParent(void);
	};
}



#endif

