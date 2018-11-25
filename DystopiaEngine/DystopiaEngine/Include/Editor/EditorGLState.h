/* HEADER *********************************************************************************/
/*!
\file	EditorGLState.h
\author Shannon Tan (100%)
\par    email: t.shannon\@digipen.edu
\brief

All Content Copyright © 2018 DigiPen (SINGAPORE) Corporation, all rights reserved.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/* HEADER END *****************************************************************************/
#if EDITOR
#ifndef _EDITOR_GL_STATE_H_
#define _EDITOR_GL_STATE_H_

namespace Editor
{
	class GLState
	{
	public:
		GLState(void);

		void BackUp(void);
		void Restore(void);

	private:
		unsigned int last_active_texture;
		int last_program;
		int last_texture;
		int last_sampler;
		int last_array_buffer;
		int last_element_array_buffer;
		int last_vertex_array;
		int last_polygon_mode[2];
		int last_viewport[4];
		int last_scissor_box[4];
		unsigned int last_blend_src_rgb;
		unsigned int last_blend_dst_rgb;
		unsigned int last_blend_src_alpha;
		unsigned int last_blend_dst_alpha;
		unsigned int last_blend_equation_rgb;
		unsigned int last_blend_equation_alpha;
		unsigned char last_enable_blend;
		unsigned char last_enable_cull_face;
		unsigned char last_enable_depth_test;
		unsigned char last_enable_scissor_test;

	};

}


#endif
#endif







