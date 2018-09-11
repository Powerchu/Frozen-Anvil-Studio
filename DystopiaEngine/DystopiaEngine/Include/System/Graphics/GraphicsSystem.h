/* HEADER *********************************************************************************/
/*!
\file	GraphicsSystem.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _GRAPHICS_SYS_H_
#define _GRAPHICS_SYS_H_

#include "System\Base\Systems.h"	// System

#include <string>
#include <map>


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

		void PreInit(void);
		bool Init(void);
		void PostInit(void);

		void Update(float);		// Draws the currently bounded window
		void Shutdown(void);

		// Sets up Window for openGL rendering
		bool InitOpenGL(Window&);
		void BindOpenGL(Window&) noexcept;

		void LoadDefaults(void);
		void LoadSettings(TextSerialiser&);

		void     LevelLoad(TextSerialiser&);
		void     LoadMesh(const std::string&);
		Texture* LoadTexture(const std::string&);
		Shader*	 LoadShader(const std::string&);

		static const int& GetDrawMode(void) noexcept;
		static void SetDrawMode(int) noexcept;

	private:

		void* mOpenGL;
		int mPixelFormat, mAvailable;
		Window* mCurrent;

		// Temporary
		std::map<std::string, Shader*> shaderlist;
		std::map<std::string, Texture*> texturelist;

		static int DRAW_MODE;

		void StartFrame(void);
		void EndFrame(void);

		void DrawSplash(void);

		bool SelectOpenGLVersion(Window&) noexcept;
	};
}



#endif		// INCLUDE GUARD

