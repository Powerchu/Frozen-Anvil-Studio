/* HEADER *********************************************************************************/
/*!
\file	MetaBind.h
\author Tan Jie Wei Jacky (100%)
\par    email: t.jieweijacky\@digipen.edu
\brief
	Bind arguments to metafunctions. 
	Works like std::bind but for metafunctions

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#ifndef _METAFUNCS_BIND_H_
#define _METAFUNCS_BIND_H_


namespace Ut
{
	template <size_t PARAM>
	struct PlaceHolder;

	namespace Helper
	{

	}

	template <template <typename ...> class MF, typename ... Ty>
	struct MetaBind
	{

	};


}



#endif		// INCLUDE GUARD

