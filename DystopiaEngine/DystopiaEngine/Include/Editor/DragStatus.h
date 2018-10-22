/* HEADER *********************************************************************************/
/*!
\file	DragStatus.h
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

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _DRAG_STATUS_H_
#define _DRAG_STATUS_H_

namespace EGUI
{
	enum eDragStatus
	{
		eNO_CHANGE = 0,
		eEND_DRAG,
		eENTER,
		eTABBED,
		eSTART_DRAG,
		eDEACTIVATED,
		eINSTANT_CHANGE,
		eDRAGGING
	};
}

#endif
#endif
