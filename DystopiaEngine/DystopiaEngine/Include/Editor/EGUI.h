/* HEADER *********************************************************************************/
/*!
\file	EGUI.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief
This is the Encapsulation of IMGUI functions to be of ease for other programmers to use.
Please read the brief and usage of each function.
If none exist, either I forgot, or I didnt intend for you to use those.

If you need anything specificly, lemme know and I will create a user-friendly one for you.
Else, just hack it - include imgui.h and use more functions.

NOTE: Not updated usage examples for some of the functions. Check components that already
implemented it. Will update when have spare time.

All Content Copyright � 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _EDITOR_GUI_H_
#define _EDITOR_GUI_H_
#include "DataStructure/AutoArray.h"
#include "DataStructure/Array.h"
#include "DataStructure/Stack.h"

#include "Utility/DebugAssert.h"

#include "Editor/DragStatus.h"
#include "Editor/Dock.h"
#include "Editor/Payloads.h"
#include "Editor/Gizmo.h"

#include "../../Dependancies/ImGui/imgui.h"
#include "../../../Dependancies/ImGui/imgui_internal.h"
#include <string>

/* forward declare just becuz */
struct ImDrawData;
struct ImGuiContext;

static float DefaultAlighnmentOffsetY = 3.f;
static float DefaultAlighnmentSpacing = 10.f;
static float DefaultAlignLeft = 0.f;

namespace DysPeekia
{
	class CommandHandler;
	class WindowManager;
	class GraphicsSystem;
	class EditorInput;
	class GLState;
	typedef unsigned int GLuint;
}
/* ========================== */

/* =======================================================================================================================
Brief:
If you call any Start... functions, please remember to call their respective End... functions.
Most Start.. function returns a bool, the return generally(mostly) means if the display is opened or closed.
Meaning if a Tab is opened or closes, a Header is opened or closed
USAGE :
if (Start...)
{
// do your other stuff here
}
End...
======================================================================================================================= */
namespace EGUI
{
	inline const Array<const char*, 60> g_ArrIndexName =
	{
		"00", "01", "02", "03", "04", "05", "06", "07", "08", "09", 
		"10", "11", "12", "13" , "14", "15", "16", "17", "18", "19",
		"20", "21", "22", "23" , "24", "25", "26", "27", "28", "29",
		"30", "31", "32", "33" , "34", "35", "36", "37", "38", "39",
		"40", "41", "42", "43" , "44", "45", "46", "47", "48", "49",
		"50", "51", "52", "53" , "54", "55", "56", "57", "58", "59"
	};

	static constexpr float TabsImageOffsetY = 27.f;

	Stack<float>& GetLeftAlignStack(void);

	void SetContext(DysPeekia::CommandHandler *_pContext);
	//DysPeekia::CommandHandler* GetCommandHND();
	void RemoveContext();
	void ChangeLabelSpacing(float _amount);
	void ChangeLabelSpacing();
	void ChangeAlignmentYOffset(float _amount);
	void ChangeAlignmentYOffset();
	float GetAlignmentYOffset();
	float GetLabelSpacing();

	bool StartMainMenuBar();
	bool StartMenuHeader(const char * _p, bool _enabled = true);
	bool StartMenuBody(const char * _p, const char * _shortcut = "", bool _enabled = true);
	void EndMainMenuBar();
	void EndMenuHeader();
	void Indent(float _spacing = 20.f);
	void UnIndent(float _spacing = 20.f);
	void PushID(int);
	void PopID();
	void PushLeftAlign(float);
	void PopLeftAlign();

	/* =======================================================================================================================
	Brief:
	You can ignore the last 2 parameters. Read the below usage
	Usage:
	if (StartTab("name of the tab", defaulted ignore, defaulted ignore))
	{
	// other GUI stuff inside this Tab window
	// if (EGUI::StartChild(...))
	// {
	// }
	// EGUI::EndChild(...);
	}
	EndTab();
	======================================================================================================================= */
	// Call to Start a new Tab
	bool StartTab(const char*, bool* = nullptr, ImGuiWindowFlags = 0);
	// Call to End the Tab segment
	void EndTab();

	/* =======================================================================================================================
	Brief:
	Starts a child UI window inside the window you are currently in. Its like a sub window in a window.
	(By default all Editor UI is the child of a EditorTab window.)
	Usage:
	if (EGUI::StartChild("name of the window", size of the window)
	{
	// whatever UI stuff that is needed in the child.
	// EGUI::Display::Label("start");
	// EGUI::Display::Label("End);
	}
	EGUI::EndChild();
	======================================================================================================================= */
	// Call to Start Child window in the current window you are in 
	bool StartChild(const char*, const Math::Vec2& = { 0, 0 }, bool _showBorder = true, bool _noScroll = false);
	// Call to Start Child window in the current window you are in 
	bool StartChild(const char*, const float& = 0, const float& = 0, bool _showBorder = true, bool _noScroll = false);
	// Call to end the child window and proceed back to the parent window
	void EndChild();

	/* =======================================================================================================================
	Brief:
	Makes the next UI item to be printed on the right side of the previous UI item. If custom offset > 0, starts
	counting from left alignment of the imgui window its being called in
	(Defualt is - below the previous item)
	Usage:
	EGUI::Display::Label("start");
	EGUI::SameLine();
	EGUI::Display::Label("end");
	======================================================================================================================= */
	void SameLine(float _customOffset = 5.f, float _leftOff = 0.f);

	namespace Display
	{
		/* =======================================================================================================================
		Brief:
		Draws a horizontal line in the current imgui window
		======================================================================================================================= */
		void HorizontalSeparator();
		/* =======================================================================================================================
		Brief:
		Creates a text display (non-editable). Used like a printf function.
		The label name can contain C-style formats such as %d and put an integer in the elipse variable.
		Usage:
		EGUI::Display::Label("hello world %d %f", 10, 0.63f);
		======================================================================================================================= */
		void Label(const char *_label, ...);
		void LabelWrapped(const char *_label, ...);
		/* =======================================================================================================================
		Brief:
		Creates a editable text field with a label to the left of the text field. If edited, the _pOutText will be changed
		accordingly. The size limits the number of characters accepted.
		Usage:
		char buffer[size];
		EGUI::Display::TextField("This is an editable text field: ", &buffer, size);
		======================================================================================================================= */
		bool TextField(const char * _label, char *_pOutText, size_t _size, bool _showLabel = true, float _width = 250, bool _onlyEnterReturnsTrue = true);
		bool TextField(const char * _label, HashString& _out, bool _showLabel = true, float _width = 250, bool _onlyEnterReturnsTrue = true);
		/* =======================================================================================================================
		Brief:
		Creates a vertical or horizontal selectable splitter (like a separator), allows you to drag child windows.
		Created by Aaron.
		Usage:
		float h = 200;
		static float sz1 = 300;
		static float sz2 = 300;
		EGUI::Display::Splitter(true, 8.0f, &sz1, &sz2, 8, 8, h);
		======================================================================================================================= */
		bool Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size = -1.0f);
		/* =======================================================================================================================
		Brief:
		Creates a vertical or horizontal selectable splitter (like a separator), allows you to drag child windows.
		Created by Aaron.
		Usage:

		======================================================================================================================= */
		bool ColorButton(const char* desc_id, const Math::Vec4& col, ImGuiColorEditFlags flags, Math::Vec2 size = {0,0});
		/* =======================================================================================================================
		Brief:
		Creates an empty box. Great for using alongside payloads if you unsure. returns if the box is clicked.
		If the last bool param is false, hovering wont show and clicking wont return true
		Usage:
		EGUI::Display::EmptyBox("Accepting field", 100);
		======================================================================================================================= */
		bool EmptyBox(const char * _label, float _width, const char * _anythingToShowInside = nullptr,
			bool _iteractive = false, bool _showLabel = true);
		/* =======================================================================================================================
		Brief:
		Starts a Collaspe-able header that can be clicked to open or close - Which is also the return-ed bool.
		Usage:
		if (EGUI::Display::CollapsingHeader("This function will return true when opened"))
		{
		// UI to do when this header is opened
		// EGUI::Display::Label("Things inside this header");
		}
		======================================================================================================================= */
		bool CollapsingHeader(const char * _label, bool defaultOpen = true);
		/* =======================================================================================================================
		Brief:
		Creates an editable vector field that is pre-arranged for you. Returns true if any of the variable is changed.
		The format is coded to be:
		label of field [editable input] x [editable input] y [editable input] z
		Usage:
		switch (EGUI::Display::VectorFields("Scale", &mScale, 0.5f, -FLT_MAX, FLT_MAX))
		{
		case EGUI::eDragStatus::eSTART_DRAG:
		EGUI::GetCommandHND()->StartRecording<Transform>(GetOwner()->GetID(), &mScale);
		break;
		case EGUI::eDragStatus::eEND_DRAG:
		EGUI::GetCommandHND()->EndRecording();
		break;
		}
		======================================================================================================================= */
		Array<eDragStatus, 3> VectorFields(const char * _label, Math::Vector4 *_outputVec, float _dragSpeed = 1.0f,
			float _min = 0.0f, float _max = 1.0f, float _width = 50.f, bool _wParamEnable = false, unsigned digits = 3);
		Array<eDragStatus, 2> VectorFields(const char * _label, Math::Vector2 *_outputVec, float _dragSpeed = 1.0f,
			float _min = 0.0f, float _max = 1.0f, float _width = 50.f, unsigned digits = 3);
		Array<eDragStatus, 2> VectorFieldsInt(const char *_label, Math::Vector2 *_outputVec, int _dragSpeed = 1,
			int _min = 0, int _max = 1, float _width = 50.f);
		/* =======================================================================================================================
		Brief:
		Creats a check box for a boolean variable. Returns true when the check box is clicked, toggles the _pOutBool
		Usage:
		bool toggleMe = false;
		if (EGUI::Display::CheckBox("This is a checkBox", &toggleMe))
		{
		// The checkbox is clicked (toggleMe if its true or false does not matter). Do something here:
		}
		======================================================================================================================= */
		bool CheckBox(const char * _label, bool *_pOutBool, bool _showLabel = true, const char* _tooltip = nullptr, float _szMod = 2.0f);
		/* =======================================================================================================================
		Brief:
		Creates a radio button for a boolean variable. Returns true when the radiois clicked, toggles the _pOutBool
		Usage:
		static int selected = 0;
		// vertically aligned radio buttons
		ImGui::RadioButton("Number 1", &selected, 1);
		ImGui::RadioButton("Number 2", &selected, 2);
		ImGui::RadioButton("Number 3", &selected, 3);
		{
		// The Radio is clicked (toggleMe if its true or false does not matter). Do something here:
		}
		======================================================================================================================= */
		bool RadioBtn(const char * _label, int* _pValueStorage, int _btnValue, bool _showLabel);
		/* =======================================================================================================================
		Brief:
		Creates a draggable float editable field. Returns if the value is changed
		Usage:
		switch (EGUI::Display::DragFloat("Test Float", &mpFocus->mTestFloat, 0.01f, -FLT_MAX, FLT_MAX))
		{
		case EGUI::eDragStatus::eSTART_DRAG:
		GetCommandHND()->StartRecording<GameObject>(mpFocus->GetID(), &mpFocus->mTestFloat);
		break;
		case EGUI::eDragStatus::eEND_DRAG:
		GetCommandHND()->EndRecording();
		break;
		}
		======================================================================================================================= */
		eDragStatus DragFloat(const char * _label, float *_pOutFloat, float _dragSpeed = 1.0f,
			float _min = 0.0f, float _max = 1.0f, bool _hideText = false, float _width = 100.f, unsigned digits = 3);
		eDragStatus SliderFloat(const char * _label, float *_pOutFloat, float _min = 0.0f, float _max = 1.0f,
			bool _hideText = false, float _width = 100.f);
		eDragStatus SliderInt(const char * _label, int *_pOutInt, int _min = 0, int _max = 1,
			bool _hideText = false, float _width = 100.f);
		/* =======================================================================================================================
		Brief:
		Creates a draggable int editable field. Returns if the value is changed
		Usage:
		switch (EGUI::Display::DragInt("Test Int", &mpFocus->mTestInt, 1.f, -INT_MAX, INT_MAX))
		{
		case EGUI::eDragStatus::eSTART_DRAG:
		GetCommandHND()->StartRecording<GameObject>(mpFocus->GetID(), &mpFocus->mTestInt);
		break;
		case EGUI::eDragStatus::eEND_DRAG:
		GetCommandHND()->EndRecording();
		break;
		}
		======================================================================================================================= */
		eDragStatus DragInt(const char * _label, int *_pOutInt, float _dragSpeed = 1.0f,
			int _min = 0, int _max = 0, bool _hideText = false, float _width = 100.f);
		/* =======================================================================================================================
		Brief:
		Creates a selectable text field. Returns if the text field is clicked is changed
		Usage:
		bool toggleSelection = false;
		if (EGUI::Display::SelectableTxt("I am a selectable text field", &toggleSelection))
		{
		// The textfield is clicked. Do something here:
		}
		if (EGUI::Display::SelectableTxt("I am a selectable text field"))
		{
		// The textfield is clicked. Do something here:
		}
		======================================================================================================================= */
		// Creates a selectable text field with an output bool to toggle
		bool SelectableTxt(const char* _label, bool *_pSelected);
		// Creates a selectable text field with a bool to determine if it is highlighted (selected) or not
		bool SelectableTxt(const char* _label, bool _highlight = false);
		/* =======================================================================================================================
		Brief:
		Creates a double click selectable text field. Returns true only when the text field is double clicked
		Usage:
		bool toggleSelection = false;
		if (EGUI::Display::SelectableTxtDouble("I am a double clickable text field", &toggleSelection))
		{
		// The textfield is double clicked. Do something here:
		}
		if (EGUI::Display::SelectableTxtDouble("I am a double clickable text field"))
		{
		// The textfield is double clicked. Do something here:
		}
		======================================================================================================================= */
		// Creates a selectable text field with an output bool to toggle
		bool SelectableTxtDouble(const char*  _label, bool *_pSelected);
		// Creates a selectable text field with a bool to determine if it is highlighted (selected) or not
		bool SelectableTxtDouble(const char* _label, bool = false);
		/* =======================================================================================================================
		Brief:
		Start of a Tree Node (like the one you see in ResourceView I believe - 3 July 2018). Returns true if tree is opened.
		This is different from the other Start... functions. You must call the End.. function inside the if()
		Usage:
		if (EGUI::Display::StartTreeNode("I am a Tree Node"))
		{
		// Do something inside this tree
		// Tree inside a Tree
		if (EGUI::Display::StartTreeNode("I am inside the first Tree Node"))
		{
		// Tree inside a Tree inside a Tree
		if (EGUI::Display::StartTreeNode("I am inside the first Tree Node inside the first Tree Node"))
		{
		EGUI::Display::EndTreeNode();
		}
		EGUI::Display::EndTreeNode();
		}
		EGUI::Display::EndTreeNode();
		}
		======================================================================================================================= */
		// Start a tree node 
		bool StartTreeNode(const char* _label, bool* _outClicked = nullptr, bool _highlighted = false,
			bool _noArrow = false, bool _defaulPeeken = true, bool _singleClickOpen = false);
		// Set a specific tree node to be collapsed (closed) or not
		void OpenTreeNode(const char* _label, bool _open);
		// opens the next tree node
		void OpenTreeNode(bool _open = true);
		//End a tree Node
		void EndTreeNode();
		/* =======================================================================================================================
		Brief:
		All in one payload creator. Created this function for intended use of resource viewer the custom
		drag and drop icons with its label below an icon. Might extend this some other day 23/7/18
		Usage:
		Very specific as of creation date.
		======================================================================================================================= */
		bool CustomPayload(const char* _uniqueId, const char * _label, const char * _tooltip,
			const Math::Vec2& _displaytSize, ePayloadTags _tagLoad, void* _pData, size_t _dataSize);
		/* =======================================================================================================================
		Brief:
		Sets the previos UI widget/item to be a payload type. Preferably call according to the usage please.
		Usage shows that button will have additional of being a payload type.
		Usage:
		EGUI::Button("Hello", Math::Vec2{ 200, 20 } );
		if (EGUI::Display::StartPay
		EGUI::FILE, &(*_file), sizeof(File), "Hello"))
		{
		EGUI::Display::EndPayload();
		}
		======================================================================================================================= */
		bool StartPayload(ePayloadTags _tagLoad, void* _pData, size_t _dataSize, const char* _toolTip);
		// Call EndPayLoad at the end of StartPayLoad return true. See StartPayLoad usage
		void EndPayload();
		/* =======================================================================================================================
		Brief:
		Sets the previos UI widget/item to be receiving a payload type. Preferably call according to the usage please.
		Usage shows that button will have additional of being a payload reciever type
		Usage:
		EGUI::Display::EmptyBox("Box to accept payload", 150, mDemoName);
		if (File *t = EGUI::Display::StartPayloadReceiver<File>(EGUI::FILE))
		{
		mDemoName = (*t).mName;
		EGUI::Display::EndPayloadReceiver();
		}
		======================================================================================================================= */
		template <typename Specified>
		Specified* StartPayloadReceiver(ePayloadTags _tagLoad)
		{
			if (ImGui::BeginDragDropTarget())
			{
				const ImGuiPayload* payload = nullptr;
				switch (_tagLoad)
				{
				case ALL_IMG:
					payload = ImGui::AcceptDragDropPayload(GetPayloadString(ePayloadTags::PNG));
					if (!payload)
						payload = ImGui::AcceptDragDropPayload(GetPayloadString(ePayloadTags::DDS));
					if (!payload)
						payload = ImGui::AcceptDragDropPayload(GetPayloadString(ePayloadTags::BMP));

					if (payload)
					{
						DEBUG_ASSERT(payload->DataSize != sizeof(Specified), "Error at EGUI");
						return static_cast<Specified*>(payload->Data);
					}
					break;
				case ALL_AUDIO:
					payload = ImGui::AcceptDragDropPayload(GetPayloadString(ePayloadTags::WAV));
					if (!payload)
						payload = ImGui::AcceptDragDropPayload(GetPayloadString(ePayloadTags::MP3));
					if (payload)
					{
						DEBUG_ASSERT(payload->DataSize != sizeof(Specified), "Error at EGUI");
						return static_cast<Specified*>(payload->Data);
					}
					break;
				}
				payload = ImGui::AcceptDragDropPayload(GetPayloadString(_tagLoad));
				if (payload)
				{
					DEBUG_ASSERT(payload->DataSize != sizeof(Specified), "Error at EGUI");
					return static_cast<Specified*>(payload->Data);
				}
				ImGui::EndDragDropTarget();
			}
			return nullptr;
		}
		// Call EndPayloadReceiver at the end of StartPayloadReceiver return true. See StartPayloadReceiver usage
		void EndPayloadReceiver();
		/* =======================================================================================================================
		Brief:
		Opens a little popup window that is passed into the param. BASICALLY - the param is the same as the _uniqueID
		of the StartPopup function you want to call.
		Note : For the second boolean param of OpenPopup is set to false, the popup will be defaulted to the
		bottom left of the last ui item called. In the usage case, it would be the bottom left of the Button("Add Co..")
		Leave it to true if can, or use the DropDownSelection function instead
		Usage:
		if (EGUI::Display::Button("Add Component", btnSize))
		{
		EGUI::Display::OpenPopup("Inspector Component List", false);
		}
		if (EGUI::Display::StartPopup("Inspector Component List"))
		{
		EGUI::Display::Dummy(235, 5);
		EGUI::Display::EndPopup();
		}
		======================================================================================================================= */
		void OpenPopup(const char* _thePopupID, bool _toOpenAtMousePos = true);
		// Creates a popup window that is tied to the _uniqueID. Call OpenPopup with its ID to open it
		bool StartPopup(const char* _uniqueID);
		bool StartPopupModal(const char* _uniqueID, const char* _label);
		void CloseCurrentPopup();
		// call this at the end of the StartPopup function when it is true. Determines the end of the popup
		void EndPopup();
		/* =======================================================================================================================
		Brief:
		Creates a drop down box with selectable strings. The currently selected index will be stored into the second
		parameter variable.
		Usage:
		static int i = 0;
		AutoArray<std::string> arr;
		arr.push_back("");
		arr.push_back("item1");
		arr.push_back("item2");
		EGUI::Display::DropDownSelection("TestDropDown", i, arr);
		======================================================================================================================= */
		bool DropDownSelection(const char* _label, int& _currentIndex, AutoArray<const char *>& _arrOfItems, float _width = 100);
		bool DropDownSelection(const char* _label, int& _currentIndex, AutoArray<std::string>& _arrOfItems, float _width = 100);
		template<unsigned N>
		bool DropDownSelection(const char* _label, int& _currentIndex, unsigned _under60, float _width = 100)
		{
			static_assert(N < 60, "Under 60");

			ImGui::PushItemWidth(_width);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + DefaultAlighnmentOffsetY);
			Label(_label);
			SameLine(DefaultAlighnmentSpacing, GetLeftAlignStack().IsEmpty() ? DefaultAlignLeft : GetLeftAlignStack().Peek());
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - DefaultAlighnmentOffsetY);
			HashString inviLabel{ "##" };
			inviLabel += _label;
			const bool ret = ImGui::Combo(inviLabel.c_str(), &_currentIndex, g_ArrIndexName.cbegin(), Math::Min(_under60, N));
			ImGui::PopItemWidth();
			return ret;
		}
		template<unsigned N>
		bool DropDownSelection(const char* _label, int& _currentIndex, const std::string(&_arrOfItems)[N], float _width = 100)
		{
			const char* arrCharPtr[N];
			for (unsigned i = 0; i < N; ++i)
				arrCharPtr[i] = _arrOfItems[i].c_str();

			ImGui::PushItemWidth(_width);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + DefaultAlighnmentOffsetY);
			Label(_label);
			SameLine(DefaultAlighnmentSpacing, GetLeftAlignStack().IsEmpty() ? DefaultAlignLeft : GetLeftAlignStack().Peek());
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - DefaultAlighnmentOffsetY);
			HashString invi{ "##DropDownList" };
			invi += _label;
			bool ret = ImGui::Combo(invi.c_str(), &_currentIndex, arrCharPtr, N);
			ImGui::PopItemWidth();
			return ret;
		}
		template<unsigned N>
		bool DropDownSelection(const char* _label, int& _currentIndex, const char * const(&_arrOfItems)[N], float _width = 100)
		{
			ImGui::PushItemWidth(_width);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + DefaultAlighnmentOffsetY);
			Label(_label);
			SameLine(DefaultAlighnmentSpacing, GetLeftAlignStack().IsEmpty() ? DefaultAlignLeft : GetLeftAlignStack().Peek());
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - DefaultAlighnmentOffsetY);
			HashString invi{ "##DropDownList" };
			invi += _label;
			bool ret = ImGui::Combo(invi.c_str(), &_currentIndex, _arrOfItems, N);
			ImGui::PopItemWidth();
			return ret;
		}
		/* =======================================================================================================================
		Brief:
		Creates a button
		Usage:
		EGUI::Display::Button("Demo", ImVec2{ 200, 20 });
		======================================================================================================================= */
		bool Button(const char* _label, const Math::Vec2& _size = Math::Vec2{ 50, 24 });
		/* =======================================================================================================================
		Brief:
		Creates a dummy area. Used only to offset ImGui stuff. Does absolutely nothing except for manipulating UI
		arrangements
		Usage:
		EGUI::Display::Dummy(10,10);
		======================================================================================================================= */
		void Dummy(float width = 0.f, float height = 0.f);
		/* =======================================================================================================================
		Brief:
		Creates a small tick icon
		For best display, use the same ratio of w and h
		Usage:
		EGUI::Display::IconTick(10,10);
		======================================================================================================================= */
		void IconTick(float width = 5.f, float height = 5.f, float _thickness = 1.f,
			const Math::Vec4& _colour = Math::Vec4{ 1.f, 1.f, 0.4f, 1.f });
		/* =======================================================================================================================
		Brief:
		Creates a small folder icon. DO NOT have a color with less than 0.3f for any of the RGBA.
		Returns true when clicked
		For best display, use a 10 : 6 ratio for width and height respectively.
		Usage:
		EGUI::Display::IconFolder("something");
		======================================================================================================================= */
		bool IconFolder(const char* _uniqueId, float width = 15.f, float height = 9.f, bool _open = false,
			const Math::Vec4& _colour = Math::Vec4{ 1.f, 1.f, 0.4f, 1.f });
		/* =======================================================================================================================
		Brief:
		Creates a small circle icon thingy. Like the one you see in unity field boxes.
		Returns true when clicked
		For best display, use small radiuses < 10
		Usage:
		EGUI::Display::IconCircle("something");
		======================================================================================================================= */
		bool IconCircle(const char* _uniqueID, float radius = 10.f, float offsetX = 0.f,
			float offsetY = 0.f, const Math::Vec4& _colour = Math::Vec4{ 0.7f, 0.7f, 0.7f, 0.8f });
		/* =======================================================================================================================
		Brief:
		Creates a small circle icon thingy. Like the one you see in unity field boxes.
		Returns true when clicked
		For best display, use small radiuses < 10
		Usage:
		EGUI::Display::IconCross("something");
		======================================================================================================================= */
		bool IconCross(const char* _uniqueID, float radius = 10.f, float offsetX = 0.f,
			float offsetY = 0.f, const Math::Vec4& _colour = Math::Vec4{ 0.7f, 0.7f, 0.7f, 0.8f });
		/* =======================================================================================================================
		Brief:
		Creates a small game object icon. Returns true when clicked
		Usage:
		EGUI::Display::IconGameObj("something");
		======================================================================================================================= */
		bool IconGameObj(const char* _uniqueId, float _width = 10.f, float _height = 10.f);
		/* =======================================================================================================================
		Brief:
		Creates a small File icon. Returns true when clicked
		Usage:
		EGUI::Display::IconFile("something");
		======================================================================================================================= */
		bool IconFile(const char* _uniqueId, float _width = 10.f, float _height = 10.f,
			const Math::Vec4& _colour = Math::Vec4{ 0.2f, 0.8f, 0.7f, 0.8f });
		/* =======================================================================================================================
		Brief:
		Creates a box outline given the width and height
		Usage:
		EGUI::Display::Outline(your ui width, height);
		======================================================================================================================= */
		void Outline(float _x, float _y);
		/* =======================================================================================================================
		Brief:
		Creates a labeled Graph
		Usage:
		static float arr[10] = {1,2,3,4,5,6,7,8,9,10};
		EGUI::Display::LineGrap("MyGraph", arr, 10, 0, 11, Math::Vec2{100,20}, "hovering");
		======================================================================================================================= */
		template<size_t N>
		void LineGraph(const char* _uniqueLabel, const float(&_array)[N], float _min = 0.f, float _max = 1.f,
			const Math::Vec2& _size = Math::Vec2{ 100, 20 }, const char* _overlapText = "")
		{
			EGUI::Display::Label(_uniqueLabel);
			HashString invi{ "##" };
			invi += _uniqueLabel;
			ImGui::PlotLines(invi.c_str(), _array, N, 0, _overlapText, _min, _max, ImVec2{ _size.x, _size.y });
		}
		template<size_t N>
		void LineGraph(const char* _uniqueLabel, const Array<float, N>& _array, float _min = 0.f,
			float _max = 1.f, const Math::Vec2& _size = Math::Vec2{ 100, 20 }, const char* _overlapText = "")
		{
			//EGUI::Display::Label(_uniqueLabel.c_str());
			HashString invi{ "##LG" };
			invi += _uniqueLabel;
			ImGui::PlotLines(invi.c_str(), _array.begin(), static_cast<int>(_array.size()), 0,
				_overlapText, _min, _max, ImVec2{ _size.x, _size.y });
		}

		/* =======================================================================================================================
		Brief:
		Creates aN IMAGE as either a button or not (_interactive)
		Usage:
		size_t id = mpGfxSys->GetFrameBuffer().AsTexture()->GetID();
		mImgSize = Math::Vec2{ Size().x,  Size().y - imageOffsetY };
		if (EGUI::Display::Image(id, mImgSize, true))
		======================================================================================================================= */
		bool Image(const size_t& _imgID, const Math::Vec2& _imgSize = Math::Vec2{ 30, 30 },
			bool _interactive = false, bool _outlineBG = false);

		void ImageEmpty(const char* _str, const Math::Vec2 & _imgSize);

		struct ComboFilterState
		{
			int  activeIdx = 0;         // Index of currently 'active' item by use of up/down keys
			bool selectionChanged = false;  // Flag to help focus the correct item when selecting active item
		};

		/* ====================================================================================================================
		Brief:
		This is a helper function for ComboFilter to draw the popup. 
		Usage:
		*To be written*
		======================================================================================================================= */
		bool ComboFilter_DrawPopup(ComboFilterState& state, int START, const char **ENTRIES, const int ENTRY_COUNT);


		/* =======================================================================================================================
		Brief:
		Creates an ImGUI Combo Field (Like Dropdown but with text)
		Usage:
		{
			// requisite: hints must be alphabetically sorted beforehand
			const char *hints[] = 
			{
				"AnimGraphNode_CopyBone",
				"ce skipaa",
				"ce skipscreen",
				"ce skipsplash",
				"ce skipsplashscreen",
				"client_unit.cpp",
				"letrograd",
				"level",
				"leveler",
				"MacroCallback.cpp",
				"Miskatonic university",
				"MockAI.h",
				"MockGameplayTasks.h",
				"MovieSceneColorTrack.cpp",
				"r.maxfps",
				"r.maxsteadyfps",
				"reboot",
				"rescale",
				"reset",
				"resource",
				"restart",
				"retrocomputer",
				"retrograd",
				"return",
				"slomo 10",
			};

			static ComboFilterState s = {0};
			static char buf[128] = "type text here...";
			
			if( ComboFilter("my combofilter", buf, IM_ARRAYSIZE(buf), hints, IM_ARRAYSIZE(hints), s) ) 
			{
				puts( buf );
			}
		}
		======================================================================================================================= */
		bool ComboFilter(const char *id, char *buffer, int bufferlen, const char **hints, int num_hints, ComboFilterState &s);

		/* ====================================================================================================================
		Brief:
		Vertical Tabs (todo: will update how to use)
		======================================================================================================================= */
		bool GoxTab(const char *text, bool *v);

		struct fuzzy {
			static int score(const char *str1, const char *str2)
			{
				int score = 0, consecutive = 0, maxerrors = 0;
				while (*str1 && *str2) {
					const int is_leading = (*str1 & 64) && !(str1[1] & 64);
					if ((*str1 & ~32) == (*str2 & ~32)) {
						const int had_separator = (str1[-1] <= 32);
						const int x = had_separator || is_leading ? 10 : consecutive * 5;
						consecutive = 1;
						score += x;
						++str2;
					}
					else {
						const int x = -1;
						const int y = is_leading * -3;
						consecutive = 0;
						score += x;
						maxerrors += y;
					}
					++str1;
				}
				return score + (maxerrors < -9 ? -9 : maxerrors);
			}

			static int search(const char *str, int num, const char *words[]) {
				int scoremax = 0;
				int best = -1;
				for (int i = 0; i < num; ++i) {
					const int score = fuzzy::score(words[i], str);
					const int record = (score >= scoremax);
					const int draw = (score == scoremax);
					if (record) {
						scoremax = score;
						if (!draw) best = i;
						else best = best >= 0 && strlen(words[best]) < strlen(words[i]) ? best : i;
					}
				}
				return best;
			}

			static int searchString(const HashString& str, int num, AutoArray<HashString> words) {
				int scoremax = 0;
				int best = -1;
				for (int i = 0; i < num; ++i) {
					const int score = fuzzy::score(words[i].c_str(), str.c_str());
					const int record = (score >= scoremax);
					const int draw = (score == scoremax);
					if (record) {
						scoremax = score;
						if (!draw) best = i;
						else best = best >= 0 && words[best] < words[i] ? best : i;
					}
				}
				return best;
			}
		};

	}
}

/*========================================================================================================================================================*/
/*====================================================================== GUI SYSTEM ======================================================================*/
/*========================================================================================================================================================*/
/*
namespace DysPeekia
{
class GuiSystem
{
public:
GuiSystem();
~GuiSystem();

bool			Init(WindowManager*, GraphicsSystem*, EditorInput*, const char* = "MainDockable");
void			StartFrame(const float&);
void			EndFrame();
void			Shutdown();

void			UpdateChar(unsigned short);
void			UpdateScroll(float _x, float _y);
void			UpdateMouse(int, bool);
void			UpdateKey(int _k, bool _down);

const char*		GetMainDockspaceName() const;

private:
ImGuiContext	*mpCtx;
WindowManager	*mpWin;
GraphicsSystem	*mpGfx;
EditorInput		*mpInput;
ImDrawData		*mpDrawData;
GLState			*mpGLState;
GLuint			mFontTexture;
std::string		mGlslVersion;
const char		*mpMainDockspace;
int				mShaderHandle;
int				mVertHandle;
int				mFragHandle;
int				mAttribLocationTex;
int				mAttribLocationProjMtx;
int				mAttribLocationPosition;
int				mAttribLocationUV;
int				mAttribLocationColor;
unsigned int	mVboHandle;
unsigned int	mElementsHandle;
bool			mMouseJustPressed[3];	// 0 is left, 1 is right, 2 is middle

void			CreateDefaultFont();
void			StartFullDockableSpace();
void			EndFullDockableSpace();
void			DefaultColorSettings();
};
}
*/
#endif // !_EDITOR_GUI_H_
#endif // EDITOR ONLY













