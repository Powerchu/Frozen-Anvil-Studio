/* HEADER *********************************************************************************/
/*!
\file	Inspector.h
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief
INSERT BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _INSPECTOR_H_
#define _INSPECTOR_H_

namespace Dystopia
{
	class GameObject;
	class Inspector
	{
	public:
		Inspector();
		~Inspector();
		void Init();
		void Update(const float _delta);
		void Shutdown();

		void Window();
		void SetWidth(float _width);
		void SetHeight(float _height);

	private:
		float mWidth;
		float mHeight;
		GameObject *mpFocusGameObj;
	};
}



#endif // _INSPECTOR_H_

#endif // EDITOR 


