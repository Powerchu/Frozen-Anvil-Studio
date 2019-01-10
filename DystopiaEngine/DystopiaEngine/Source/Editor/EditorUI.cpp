/* HEADER *********************************************************************************/
/*!
\file	EditorUI.cpp
\author Digipen (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#include "Editor/EditorUI.h"
#include "Editor/EditorMain.h"
#include "Editor/EInput.h"
#include "Editor/EGUI.h"

#include "GL/glew.h"

#include "../../Dependancies/ImGui/imgui.h"
#include "../../Dependancies/ImGui/imgui_internal.h"

#include "System/Driver/Driver.h"
#include "System/Input/InputSystem.h"
#include "System/Window/Window.h"
#include "System/Window/WindowManager.h"

#include <Windows.h>
#include <windef.h>
#include <WinUser.h>

#define FONT_LARGE 68
#define FONT_MEDIUM 34
#define FONT_SMALL 20
#define FONT_MINI 16

HCURSOR g_CursorTypes[8];
const char* const g_mainDockName = "MainDock";
const char* const g_fontType = "../Dependancies/ImGui/Fonts/RalewayLight.ttf";
const char* const g_fontType_alt = "../Dependancies/ImGui/Fonts/Montserrat.ttf";
const char* const g_fontType_mono = "../Dependancies/ImGui/Fonts/RobotoMono-Regular.ttf";

Editor::EditorUI::EditorUI(void)
	: mGLState{}, mMouseJustPressed{ false, false ,false }, mShaderHandle{}, mVertHandle{}, mFragHandle{}, 
	mAttribLocationTex{}, mAttribLocationProjMtx{}, mAttribLocationPosition{}, mAttribLocationUV{}, 
	mAttribLocationColor{}, mVboHandle{}, mElementsHandle{}, mFont{}, mbLauncherMode{ false }
{
}

Editor::EditorUI::~EditorUI(void)
{
}

void Editor::EditorUI::Load(void)
{
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	mGlslVersion = "#version 150\n";

	// Load cursors
	g_CursorTypes[ImGuiMouseCursor_Arrow] = LoadCursor(NULL, IDC_ARROW);
	g_CursorTypes[ImGuiMouseCursor_TextInput] = LoadCursor(NULL, IDC_IBEAM);
	g_CursorTypes[ImGuiMouseCursor_ResizeAll] = LoadCursor(NULL, IDC_SIZEALL);
	g_CursorTypes[ImGuiMouseCursor_ResizeNS] = LoadCursor(NULL, IDC_SIZENS);
	g_CursorTypes[ImGuiMouseCursor_ResizeEW] = LoadCursor(NULL, IDC_SIZEWE);
	g_CursorTypes[ImGuiMouseCursor_ResizeNESW] = LoadCursor(NULL, IDC_SIZENESW);
	g_CursorTypes[ImGuiMouseCursor_ResizeNWSE] = LoadCursor(NULL, IDC_SIZENWSE);
	g_CursorTypes[ImGuiMouseCursor_Hand] = LoadCursor(NULL, IDC_HAND);

	ImGuiIO& io = ImGui::GetIO();
	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;   // We can honor GetMouseCursor() values (optional)
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;    // We can honor io.WantSetMousePos requests (optional, rarely used)
															// Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.

	io.KeyMap[ImGuiKey_Backspace] = static_cast<int>(eButton::KEYBOARD_BACKSPACE);
	io.KeyMap[ImGuiKey_Tab] = static_cast<int>(eButton::KEYBOARD_TAB);
	io.KeyMap[ImGuiKey_Enter] = static_cast<int>(eButton::KEYBOARD_ENTER);
	io.KeyMap[ImGuiKey_Escape] = static_cast<int>(eButton::KEYBOARD_ESCAPE);
	io.KeyMap[ImGuiKey_Space] = static_cast<int>(eButton::KEYBOARD_SPACEBAR);
	io.KeyMap[ImGuiKey_PageUp] = static_cast<int>(eButton::KEYBOARD_PAGEUP);
	io.KeyMap[ImGuiKey_PageDown] = static_cast<int>(eButton::KEYBOARD_PAGEDOWN);
	io.KeyMap[ImGuiKey_End] = static_cast<int>(eButton::KEYBOARD_END);
	io.KeyMap[ImGuiKey_Home] = static_cast<int>(eButton::KEYBOARD_HOME);
	io.KeyMap[ImGuiKey_LeftArrow] = static_cast<int>(eButton::KEYBOARD_LEFT);
	io.KeyMap[ImGuiKey_UpArrow] = static_cast<int>(eButton::KEYBOARD_UP);
	io.KeyMap[ImGuiKey_RightArrow] = static_cast<int>(eButton::KEYBOARD_RIGHT);
	io.KeyMap[ImGuiKey_DownArrow] = static_cast<int>(eButton::KEYBOARD_DOWN);
	io.KeyMap[ImGuiKey_Insert] = static_cast<int>(eButton::KEYBOARD_INSERT);
	io.KeyMap[ImGuiKey_Delete] = static_cast<int>(eButton::KEYBOARD_DELETE);

	auto& win = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::WindowManager>()->GetMainWindow();

	io.ClipboardUserData = win.GetDeviceContext(); // pointer to both a windows and context
#ifdef _WIN32
	io.ImeWindowHandle = win.GetWindowHandle();
#endif

	DefaultFont();
	EGUI::Docking::InitTabs();
}

bool Editor::EditorUI::Init(void)
{
	return true;
}

void Editor::EditorUI::StartFrame(void)
{
	ImGuiIO& io = ImGui::GetIO();

	// Setup display size (every frame to accommodate for window resizing)
	int w, h, display_w, display_h;

	const auto winMgr = Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::WindowManager>();
	auto& win	= winMgr->GetMainWindow();

	if (win.IsFullscreen())
	{
		w = display_w = win.GetWidth() - GetSystemMetrics(SM_CXBORDER) + 1;
		h = display_h = win.GetHeight() - GetSystemMetrics(SM_CYSIZE) - GetSystemMetrics(SM_CXBORDER);
	}
	else
	{
		const auto t = GetSystemMetrics(SM_CXSIZEFRAME) * GetSystemMetrics(SM_CXPADDEDBORDER);
		if (mbLauncherMode)
		{
			w = display_w = win.GetWidth() - t;
			h = display_h = win.GetHeight() - GetSystemMetrics(SM_CYSIZE) + 3 * GetSystemMetrics(SM_CYEDGE) - GetSystemMetrics(SM_CYCAPTION);
		}
		else
		{
			w = display_w = win.GetWidth();
			h = display_h = win.GetHeight() + GetSystemMetrics(SM_CYSIZE) + GetSystemMetrics(SM_CYEDGE) - GetSystemMetrics(SM_CYCAPTION);
		}
		
	}

	io.DisplaySize = ImVec2{ static_cast<float>(w), static_cast<float>(h) };
	io.DisplayFramebufferScale = ImVec2{ w > 0 ? static_cast<float>(display_w / w) : 0,
										 h > 0 ? static_cast<float>(display_h / h) : 0 };
	// Setup time step
	io.DeltaTime = EditorMain::GetInstance()->GetDeltaTime();

	// Setup inputs
	auto input = EditorMain::GetInstance()->GetSystem<EInput>();
	if (win.GetWindowHandle() == GetActiveWindow()) // should check if this window is the focused window
	{
		float x, y;
		x = input->GetInputManager()->GetMousePosition().x;
		y = input->GetInputManager()->GetMousePosition().y;
		io.MousePos = ImVec2{ x, y };
	}
	else
	{
		io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
	}

	io.MouseDown[0] = mMouseJustPressed[0] || input->GetInputManager()->GetKey(eButton::MOUSE_LEFT);
	io.MouseDown[1] = mMouseJustPressed[1] || input->GetInputManager()->GetKey(eButton::MOUSE_RIGHT);
	io.MouseDown[2] = mMouseJustPressed[2] || input->GetInputManager()->GetKey(eButton::MOUSE_MIDDLE);
	mMouseJustPressed[0] = mMouseJustPressed[1] = mMouseJustPressed[2] = false;

	// Update cursor icon
	if ((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) == 0)
	{
		SetCursor(g_CursorTypes[ImGui::GetMouseCursor()]);
	}

	// Start the frame. This call will update the io.WantCaptureMouse, io.WantCaptureKeyboard flag that you can use to dispatch inputs (or not) to your application.
	ImGui::NewFrame();
	if (!mbLauncherMode)
		StartDock();
	glViewport(0, 0, display_w, display_h);
	glClearColor(0.2f, 0.3f, 0.3f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Editor::EditorUI::Update(float)
{}

void Editor::EditorUI::EndFrame(void)
{
	if (!mbLauncherMode)
		EndDock();
	ImGui::Render();
	ImDrawData *mpDrawData = ImGui::GetDrawData();

	// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
	ImGuiIO& io = ImGui::GetIO();
	int fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
	int fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
	if (fb_width == 0 || fb_height == 0)
		return;
	mpDrawData->ScaleClipRects(io.DisplayFramebufferScale);

	// Backup GL state
	mGLState.BackUp();

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
	mGLState.Restore();

	SwapBuffers(Dystopia::EngineCore::GetInstance()->GetSystem<Dystopia::WindowManager>()->GetMainWindow().GetDeviceContext());
}

void Editor::EditorUI::Shutdown(void)
{
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

	if (mFont)
	{
		glDeleteTextures(1, &mFont);
		ImGui::GetIO().Fonts->TexID = 0;
		mFont = 0;
	}
	ImGui::DestroyContext(ImGui::GetCurrentContext());
	EGUI::Docking::ShutdownTabs();
}

void Editor::EditorUI::Message(eEMessage)
{
}

void Editor::EditorUI::SaveSettings(Dystopia::TextSerialiser&) const
{
}

void Editor::EditorUI::LoadSettings(Dystopia::TextSerialiser&)
{
}

void Editor::EditorUI::StartDock(void)
{
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoInputs;
	static constexpr float offsetH = 18;
	ImGui::SetNextWindowPos(ImVec2{ 0, offsetH });
	ImGui::SetNextWindowSize(ImVec2{ ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y - offsetH });
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 2,2 });
	ImGui::Begin(g_mainDockName, nullptr, flags);
	ImGui::PopStyleVar();
	ImGui::SetNextWindowBgAlpha(0.f);
	EGUI::Docking::BeginDockableSpace();
}

void Editor::EditorUI::EndDock(void)
{
	EGUI::Docking::EndDockableSpace();
	ImGui::End();
}

void Editor::EditorUI::DefaultFont(void)
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

	// Build texture atlas
	ImGuiIO& io = ImGui::GetIO();
	unsigned char* pixels;
	int width, height;
	io.Fonts->AddFontFromFileTTF(g_fontType_mono, FONT_MINI);
	io.Fonts->AddFontFromFileTTF(g_fontType_alt, FONT_SMALL);
	io.Fonts->AddFontFromFileTTF(g_fontType, FONT_MEDIUM);
	io.Fonts->AddFontFromFileTTF(g_fontType, FONT_LARGE);
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);		// Load as RGBA 32-bits (75% of the memory is wasted, but default font is so small) because it is 
																// more likely to be compatible with user's existing shaders. If your ImTextureId represent a higher-level 
																// concept than just a GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory.
																// Upload texture to graphics system
	GLint last_texture2;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture2);
	glGenTextures(1, &mFont);
	glBindTexture(GL_TEXTURE_2D, mFont);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	// Store our identifier
	io.Fonts->TexID = (void *)(intptr_t)mFont;

	// Restore state
	glBindTexture(GL_TEXTURE_2D, last_texture2);
	/* end create font */

	// Restore modified GL state
	glBindTexture(GL_TEXTURE_2D, last_texture);
	glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
	glBindVertexArray(last_vertex_array);
}

void Editor::EditorUI::SetLauncherMode(bool _b)
{
	mbLauncherMode = _b;
}

void Editor::EditorUI::PushFontSize(unsigned _i)
{
	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[_i]);
}

void Editor::EditorUI::PopFontSize(void)
{
	ImGui::PopFont();
}

#endif 





