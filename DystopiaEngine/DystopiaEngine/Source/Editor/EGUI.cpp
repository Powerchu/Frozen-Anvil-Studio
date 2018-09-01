/* HEADER *********************************************************************************/
/*!
\file	EGUI.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
BRIEF HERE

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR

#include "Editor\EGUI.h"
#include "Editor\EditorInputs.h"
#include "System\Window\Window.h"
#include "System\Window\WindowManager.h"
#include "System\Graphics\GraphicsSystem.h"
#include "Math\Vector4.h"
#include "Math\Vector2.h"
#include "GL\glew.h"
#include <iostream>
#include <Windows.h>
#include <windef.h>

HCURSOR	gCursorTypes[8];

static void SetClipBoardText(void *_pUserData, const char *_pText)
{
	// Set the clipboard text somewhere
	_pUserData;
	_pText;
}

static const char* GetClipBoardText(void *_pUserData)
{
	// Return the text
	_pUserData;
	return NULL;
}

namespace Dystopia
{
	// GL state to store previous draw data before render and to restore after render
	class GLState
	{
		GLenum last_active_texture;
		GLint last_program;
		GLint last_texture;
		GLint last_sampler;
		GLint last_array_buffer;
		GLint last_element_array_buffer;
		GLint last_vertex_array;
		GLint last_polygon_mode[2];
		GLint last_viewport[4];
		GLint last_scissor_box[4];
		GLenum last_blend_src_rgb;
		GLenum last_blend_dst_rgb;
		GLenum last_blend_src_alpha;
		GLenum last_blend_dst_alpha;
		GLenum last_blend_equation_rgb;
		GLenum last_blend_equation_alpha;
		GLboolean last_enable_blend;
		GLboolean last_enable_cull_face;
		GLboolean last_enable_depth_test;
		GLboolean last_enable_scissor_test;

	public:
		void BackUp()
		{
			glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint*)&last_active_texture);
			glActiveTexture(GL_TEXTURE0);
			glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
			glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
			glGetIntegerv(GL_SAMPLER_BINDING, &last_sampler);
			glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
			glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
			glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
			glGetIntegerv(GL_POLYGON_MODE, last_polygon_mode);
			glGetIntegerv(GL_VIEWPORT, last_viewport);
			glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
			glGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)&last_blend_src_rgb);
			glGetIntegerv(GL_BLEND_DST_RGB, (GLint*)&last_blend_dst_rgb);
			glGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&last_blend_src_alpha);
			glGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&last_blend_dst_alpha);
			glGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint*)&last_blend_equation_rgb);
			glGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint*)&last_blend_equation_alpha);
			last_enable_blend = glIsEnabled(GL_BLEND);
			last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
			last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
			last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);
		}
		void Restore()
		{
			glUseProgram(last_program);
			glBindTexture(GL_TEXTURE_2D, last_texture);
			glBindSampler(0, last_sampler);
			glActiveTexture(last_active_texture);
			glBindVertexArray(last_vertex_array);
			glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
			glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
			glBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha, last_blend_dst_alpha);
			if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
			if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
			if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
			if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
			glPolygonMode(GL_FRONT_AND_BACK, (GLenum)last_polygon_mode[0]);
			glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
			glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
		}
	};

	GuiSystem::GuiSystem()
		: mpWin{ nullptr }, mpGfx{ nullptr }, mpInput{ nullptr }, mpDrawData{ nullptr }, mpGLState{ nullptr }, mGlslVersion{ "#version 150" },
		mFontTexture{ 0 }, mShaderHandle{ 0 }, mVertHandle{ 0 }, mFragHandle{ 0 }, mAttribLocationTex{ 0 }, mAttribLocationProjMtx{ 0 },
		mAttribLocationPosition{ 0 }, mAttribLocationUV{ 0 }, mAttribLocationColor{ 0 }, mVboHandle{ 0 }, mElementsHandle{ 0 },
		mMouseJustPressed{ false, false, false }, mpCtx{ nullptr }
	{}

	GuiSystem::~GuiSystem()
	{}

	bool GuiSystem::Init(WindowManager *_pWin, GraphicsSystem *_pGfx, EditorInput *_pInput, const char *_pMainDockspaceName)
	{
		if (!_pWin || !_pGfx || !_pInput) return false;

		mpWin = _pWin;
		mpGfx = _pGfx;
		mpInput = _pInput;
		mpMainDockspace = _pMainDockspaceName;

		mpGLState = new GLState{};
		mpCtx = ImGui::CreateContext();
		ImGui::StyleColorsDark();

		// Store GL version string so we can refer to it later in case we recreate shaders.
		mGlslVersion = "#version 150";
		mGlslVersion += "\n";

		// Setup back-end capabilities flags
		ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;   // We can honor GetMouseCursor() values (optional)
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;    // We can honor io.WantSetMousePos requests (optional, rarely used)
																// Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.

		io.KeyMap[ImGuiKey_Tab]			= static_cast<int>(eButton::KEYBOARD_TAB);
		io.KeyMap[ImGuiKey_LeftArrow]	= static_cast<int>(eButton::KEYBOARD_LEFT);
		io.KeyMap[ImGuiKey_RightArrow]	= static_cast<int>(eButton::KEYBOARD_RIGHT);
		io.KeyMap[ImGuiKey_UpArrow]		= static_cast<int>(eButton::KEYBOARD_UP);
		io.KeyMap[ImGuiKey_DownArrow]	= static_cast<int>(eButton::KEYBOARD_DOWN);
		io.KeyMap[ImGuiKey_PageUp]		= static_cast<int>(eButton::KEYBOARD_PAGEUP);
		io.KeyMap[ImGuiKey_PageDown]	= static_cast<int>(eButton::KEYBOARD_PAGEDOWN);
		io.KeyMap[ImGuiKey_Home]		= static_cast<int>(eButton::KEYBOARD_HOME);
		io.KeyMap[ImGuiKey_End]			= static_cast<int>(eButton::KEYBOARD_END);
		io.KeyMap[ImGuiKey_Insert]		= static_cast<int>(eButton::KEYBOARD_INSERT);
		io.KeyMap[ImGuiKey_Delete]		= static_cast<int>(eButton::KEYBOARD_DELETE);
		io.KeyMap[ImGuiKey_Backspace]	= static_cast<int>(eButton::KEYBOARD_BACKSPACE);
		io.KeyMap[ImGuiKey_Space]		= static_cast<int>(eButton::KEYBOARD_SPACEBAR);
		io.KeyMap[ImGuiKey_Enter]		= static_cast<int>(eButton::KEYBOARD_ENTER);
		io.KeyMap[ImGuiKey_Escape]		= static_cast<int>(eButton::KEYBOARD_ESCAPE);
		io.KeyMap[ImGuiKey_A]			= static_cast<int>(eButton::KEYBOARD_A);
		io.KeyMap[ImGuiKey_C]			= static_cast<int>(eButton::KEYBOARD_C);
		io.KeyMap[ImGuiKey_V]			= static_cast<int>(eButton::KEYBOARD_V);
		io.KeyMap[ImGuiKey_X]			= static_cast<int>(eButton::KEYBOARD_X);
		io.KeyMap[ImGuiKey_Y]			= static_cast<int>(eButton::KEYBOARD_Y);
		io.KeyMap[ImGuiKey_Z]			= static_cast<int>(eButton::KEYBOARD_Z);

		io.SetClipboardTextFn = SetClipBoardText;
		io.GetClipboardTextFn = GetClipBoardText;
		io.ClipboardUserData = mpWin->GetMainWindow().GetDeviceContext(); // pointer to both a windows and context
	#ifdef _WIN32
		io.ImeWindowHandle = mpWin->GetMainWindow().GetWindowHandle();
	#endif
		// Load cursors
		gCursorTypes[ImGuiMouseCursor_Arrow]		= LoadCursor(NULL, IDC_ARROW);
		gCursorTypes[ImGuiMouseCursor_TextInput]	= LoadCursor(NULL, IDC_IBEAM);
		gCursorTypes[ImGuiMouseCursor_ResizeAll]	= LoadCursor(NULL, IDC_SIZEALL);
		gCursorTypes[ImGuiMouseCursor_ResizeNS]		= LoadCursor(NULL, IDC_SIZENS);
		gCursorTypes[ImGuiMouseCursor_ResizeEW]		= LoadCursor(NULL, IDC_SIZEWE);
		gCursorTypes[ImGuiMouseCursor_ResizeNESW]	= LoadCursor(NULL, IDC_SIZENESW);
		gCursorTypes[ImGuiMouseCursor_ResizeNWSE]	= LoadCursor(NULL, IDC_SIZENWSE);
		gCursorTypes[ImGuiMouseCursor_Hand]			= LoadCursor(NULL, IDC_HAND);

		DefaultColorSettings();
		return true;
	}

	void GuiSystem::StartFrame(const float& _dt)
	{
		if (!mFontTexture) CreateDefaultFont();

		ImGui::SetCurrentContext(mpCtx);
		ImGuiIO& io = ImGui::GetIO();

		// Setup display size (every frame to accommodate for window resizing)
		int w, h, display_w, display_h;
		// glfwGetWindowSize(mpWin, &w, &h);
		// glfwGetFramebufferSize(mpWin, &display_w, &display_h);
		w = display_w = 1600;
		h = display_h = 900;
		io.DisplaySize = ImVec2{ static_cast<float>(w), static_cast<float>(h) };
		io.DisplayFramebufferScale = ImVec2{ w > 0 ? static_cast<float>(display_w / w) : 0, 
											 h > 0 ? static_cast<float>(display_h / h) : 0 };
		// Setup time step
		io.DeltaTime = _dt;

		// Setup inputs
		if (mpWin) // should check if this window is the focused window
		{
			float x, y;
			x = mpInput->GetMousePosition().x;
			y = mpInput->GetMousePosition().y;
			io.MousePos = ImVec2{ x, y };
		}
		else
		{
			io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
		}

		for (int i = 0; i < 3; i++)
		{
			io.MouseDown[i] = mMouseJustPressed[i] || mpInput->IsKeyPressed(static_cast<eEditorButton>(static_cast<int>(eEditorButton::KEY_LMOUSE) + i));
			mMouseJustPressed[i] = false;
		}

		// Update cursor icon
		if ((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) == 0 )
		{
			SetCursor(gCursorTypes[ImGui::GetMouseCursor()]);
		}

		// Start the frame. This call will update the io.WantCaptureMouse, io.WantCaptureKeyboard flag that you can use to dispatch inputs (or not) to your application.
		ImGui::NewFrame();
		StartFullDockableSpace();
		glViewport(0, 0, 1600, 900);
		glClearColor(0.2f, 0.3f, 0.3f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void GuiSystem::EndFrame()
	{
		ImGui::SetCurrentContext(mpCtx);
		EndFullDockableSpace();
		ImGui::Render();
		mpDrawData = ImGui::GetDrawData();

		// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
		ImGuiIO& io = ImGui::GetIO();
		int fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
		int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
		if (fb_width == 0 || fb_height == 0)
			return;
		mpDrawData->ScaleClipRects(io.DisplayFramebufferScale);

		// Backup GL state
		mpGLState->BackUp();

		// Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, polygon fill
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_SCISSOR_TEST);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		// Setup viewport, orthographic projection matrix
		glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
		const float ortho_projection[4][4] =
		{
		{ 2.0f / io.DisplaySize.x, 0.0f,                     0.0f, 0.0f },
		{ 0.0f,                    2.0f / -io.DisplaySize.y, 0.0f, 0.0f },
		{ 0.0f,                    0.0f,                    -1.0f, 0.0f },
		{ -1.0f,                   1.0f,                     0.0f, 1.0f },
		};
		glUseProgram(mShaderHandle);
		glUniform1i(mAttribLocationTex, 0);
		glUniformMatrix4fv(mAttribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);
		glBindSampler(0, 0); // Rely on combined texture/sampler state.

							 // Recreate the VAO every time 
							 // (This is to easily allow multiple GL contexts. VAO are not shared among GL contexts, and we don't track creation/deletion of windows so we don't have an obvious key to use to cache them.)
		GLuint vao_handle = 0;
		glGenVertexArrays(1, &vao_handle);
		glBindVertexArray(vao_handle);
		glBindBuffer(GL_ARRAY_BUFFER, mVboHandle);
		glEnableVertexAttribArray(mAttribLocationPosition);
		glEnableVertexAttribArray(mAttribLocationUV);
		glEnableVertexAttribArray(mAttribLocationColor);
		glVertexAttribPointer(mAttribLocationPosition, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, pos));
		glVertexAttribPointer(mAttribLocationUV, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, uv));
		glVertexAttribPointer(mAttribLocationColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, col));

		// Draw
		for (int n = 0; n < mpDrawData->CmdListsCount; n++)
		{
			const ImDrawList* cmd_list = mpDrawData->CmdLists[n];
			const ImDrawIdx* idx_buffer_offset = 0;

			glBindBuffer(GL_ARRAY_BUFFER, mVboHandle);
			glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.Size * sizeof(ImDrawVert), (const GLvoid*)cmd_list->VtxBuffer.Data, GL_STREAM_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mElementsHandle);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx), (const GLvoid*)cmd_list->IdxBuffer.Data, GL_STREAM_DRAW);

			for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
			{
				const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
				if (pcmd->UserCallback)
				{
					pcmd->UserCallback(cmd_list, pcmd);
				}
				else
				{
					glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
					glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
					glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);
				}
				idx_buffer_offset += pcmd->ElemCount;
			}
		}
		glDeleteVertexArrays(1, &vao_handle);

		// Restore modified GL state
		mpGLState->Restore();

		SwapBuffers(mpWin->GetMainWindow().GetDeviceContext());
	}

	void GuiSystem::Shutdown()
	{
		ImGui::SetCurrentContext(mpCtx);

		// Destroy OpenGL objects
		if (mVboHandle) glDeleteBuffers(1, &mVboHandle);
		if (mElementsHandle) glDeleteBuffers(1, &mElementsHandle);
		mVboHandle = mElementsHandle = 0;

		if (mShaderHandle && mVertHandle) glDetachShader(mShaderHandle, mVertHandle);
		if (mVertHandle) glDeleteShader(mVertHandle);
		mVertHandle = 0;

		if (mShaderHandle && mFragHandle) glDetachShader(mShaderHandle, mFragHandle);
		if (mFragHandle) glDeleteShader(mFragHandle);
		mFragHandle = 0;

		if (mShaderHandle) glDeleteProgram(mShaderHandle);
		mShaderHandle = 0;

		if (mFontTexture)
		{
			glDeleteTextures(1, &mFontTexture);
			ImGui::GetIO().Fonts->TexID = 0;
			mFontTexture = 0;
		}

		delete mpGLState;

		// Look for DefaultColorSetting() function and MATCH the integer with the same amount of PushStyleColor
		ImGui::PopStyleColor(9);

		ImGui::DestroyContext(mpCtx);
	}

	void GuiSystem::CreateDefaultFont()
	{
		// Backup GL state
		GLint last_texture, last_array_buffer, last_vertex_array;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
		glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
		glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);

		const GLchar* vertex_shader =
			"uniform mat4 ProjMtx;\n"
			"in vec2 Position;\n"
			"in vec2 UV;\n"
			"in vec4 Color;\n"
			"out vec2 FramUV;\n"
			"out vec4 FramColor;\n"
			"void main()\n"
			"{\n"
			"	FramUV = UV;\n"
			"	FramColor = Color;\n"
			"	gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
			"}\n";

		const GLchar* fragment_shader =
			"uniform sampler2D Texture;\n"
			"in vec2 FramUV;\n"
			"in vec4 FramColor;\n"
			"out vec4 Out_Color;\n"
			"void main()\n"
			"{\n"
			"	Out_Color = FramColor * texture( Texture, FramUV.st);\n"
			"}\n";

		const GLchar* vertex_shader_with_version[2] = { mGlslVersion.c_str(), vertex_shader };
		const GLchar* fragment_shader_with_version[2] = { mGlslVersion.c_str(), fragment_shader };

		mShaderHandle = glCreateProgram();
		mVertHandle = glCreateShader(GL_VERTEX_SHADER);
		mFragHandle = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(mVertHandle, 2, vertex_shader_with_version, NULL);
		glShaderSource(mFragHandle, 2, fragment_shader_with_version, NULL);
		glCompileShader(mVertHandle);
		glCompileShader(mFragHandle);
		glAttachShader(mShaderHandle, mVertHandle);
		glAttachShader(mShaderHandle, mFragHandle);
		glLinkProgram(mShaderHandle);

		mAttribLocationTex = glGetUniformLocation(mShaderHandle, "Texture");
		mAttribLocationProjMtx = glGetUniformLocation(mShaderHandle, "ProjMtx");
		mAttribLocationPosition = glGetAttribLocation(mShaderHandle, "Position");
		mAttribLocationUV = glGetAttribLocation(mShaderHandle, "UV");
		mAttribLocationColor = glGetAttribLocation(mShaderHandle, "Color");

		glGenBuffers(1, &mVboHandle);
		glGenBuffers(1, &mElementsHandle);

		/* create font */
		// Build texture atlas
		ImGuiIO& io = ImGui::GetIO();
		unsigned char* pixels;
		int width, height;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);		// Load as RGBA 32-bits (75% of the memory is wasted, but default font is so small) because it is 
																	// more likely to be compatible with user's existing shaders. If your ImTextureId represent a higher-level 
																	// concept than just a GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory.
																	// Upload texture to graphics system
		GLint last_texture2;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture2);
		glGenTextures(1, &mFontTexture);
		glBindTexture(GL_TEXTURE_2D, mFontTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

		// Store our identifier
		io.Fonts->TexID = (void *)(intptr_t)mFontTexture;

		// Restore state
		glBindTexture(GL_TEXTURE_2D, last_texture2);
		/* end create font */

		// Restore modified GL state
		glBindTexture(GL_TEXTURE_2D, last_texture);
		glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
		glBindVertexArray(last_vertex_array);
	}

	void GuiSystem::StartFullDockableSpace()
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | 
								 ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoCollapse |	
								 ImGuiWindowFlags_NoInputs;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::SetNextWindowPos(ImVec2{ 0, 18 });
		ImGui::SetNextWindowSize(ImVec2{ ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y - 18 });
		ImGui::SetNextWindowBgAlpha(1.f);
		ImGui::Begin(mpMainDockspace, nullptr, flags);
		EGUI::Docking::BeginDockableSpace();
		ImGui::PopStyleVar(2);
	}

	void GuiSystem::EndFullDockableSpace()
	{
		EGUI::Docking::EndDockableSpace();
		ImGui::End();
	}

	void GuiSystem::DefaultColorSettings()
	{
		ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4{ 0.0f, 0.7f, 1.f, 0.9f });			// 1
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4{ 0.0f, 0.6f, 0.9f, 0.8f });		// 2
		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4{ 0.5f, 0.5f, 0.5f, 0.6f });				// 3
		ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4{ 0.0f, 0.7f, 1.f, 0.9f });			// 4
		ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4{ 0.0f, 0.6f, 0.9f, 0.8f });		// 5
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4{ 0.5f, 0.5f, 0.5f, 0.6f });				// 6
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.0f, 0.7f, 1.f, 0.9f });			// 7
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.0f, 0.6f, 0.9f, 0.8f });		// 8
		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.0f, 0.6f, 0.9f, 0.5f });				// 9
	}

	void GuiSystem::UpdateChar(unsigned short _c)
	{
		ImGuiIO& io = ImGui::GetIO();
		if (_c > 0 && _c < 0x10000) io.AddInputCharacter(_c);
	}

	void GuiSystem::UpdateScroll(double _xOffset, double _yOffset)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseWheelH += _xOffset;
		io.MouseWheel += _yOffset;
	}

	void GuiSystem::UpdateMouse(int _index, bool _state)
	{
		if (_index >= 0 < 3) mMouseJustPressed[_index] = _state;
	}

	const char* GuiSystem::GetMainDockspaceName() const
	{
		return mpMainDockspace;
	}

}		// namespace Dystopia

#endif // EDITOR ONLY



