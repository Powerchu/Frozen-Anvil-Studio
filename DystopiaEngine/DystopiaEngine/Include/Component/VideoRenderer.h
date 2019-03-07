#pragma once
#ifndef _VIDEO_RENDERER_H
#define _VIDEO_RENDERER_H

/*Includes*/

#include "Component/ComponentList.h"           /*Component List*/
#include "Component/Component.h"
namespace Dystopia
{
	class VideoSystem;
	class _DLL_EXPORT VideoRenderer : public Component
	{
		public:

			using SYSTEM = VideoSystem;

			unsigned GetComponentType(void) const
			{
				return Ut::MetaFind_t<Ut::Decay_t<decltype(*this)>, AllComponents>::value;
			};
			unsigned GetRealComponentType(void) const
			{
				return Ut::MetaFind_t<Ut::Decay_t<decltype(*this)>, UsableComponents>::value;
			};

#if EDITOR
			static const std::string GetCompileName(void) { return "VideoRenderer"; }
			const std::string GetEditorName(void) const   { return GetCompileName(); }
#endif

			VideoRenderer();
			VideoRenderer(VideoRenderer const & rhs);
			~VideoRenderer();

			void Awake(void);
			/*Load the Component*/
			virtual void Load(void);
			/*Initialise the Component*/
			virtual void Init(void);

			virtual void Update(float);
			/*OnDestroy*/
			virtual void OnDestroy(void);
			/*Unload the Component*/
			virtual void Unload(void);
			/*Duplicate the Component*/
			virtual VideoRenderer* Duplicate() const;

			virtual void Serialise(TextSerialiser&) const override;
			virtual void Unserialise(TextSerialiser&)     override;

		protected:




		private:
	};
}


#endif


