/* HEADER *********************************************************************************/
/*!
\file	GraphicsSystem.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	BRIEF HERE

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _GRAPHICS_SYS_H_
#define _GRAPHICS_SYS_H_

#include "System\Base\Systems.h"	// System

#include <string>

namespace Dystopia
{
	class Window;
	class Mesh;
	class Texture;
	class Shader;

	class GraphicsSystem : public Systems
	{
	public :
		// ====================================== CONSTRUCTORS ======================================= // 

		GraphicsSystem(void) noexcept;

		~GraphicsSystem(void);


		// ===================================== MEMBER FUNCTIONS ==================================== // 

		bool Init(void);
		void Update(float);		// Draws the currently bounded window
		void Shutdown(void);

		// Sets up Window for openGL rendering
		bool InitOpenGL(Window&);
		void BindOpenGL(Window&) noexcept;

		void LoadDefaults(void);
		void LoadSettings(TextSerialiser&);

		void	 LevelLoad(TextSerialiser&);
		Mesh*	 LoadMesh(const std::string&);
		Texture* LoadTexture(const std::string&);
		Shader*	 LoadShader(const std::string&);

	private:

		void* mOpenGL;
		int mPixelFormat, mAvailable;
		Window* mCurrent;

		void StartFrame(void);
		void EndFrame(void);

		bool SelectOpenGLVersion(Window&) noexcept;
	};
}



#endif		// INCLUDE GUARD

