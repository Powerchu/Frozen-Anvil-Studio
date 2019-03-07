#pragma once
#ifndef _VIDEO_SYSTEM_H
#define _VIDEO_SYSTEM_H

#include "System/Base/Systems.h"
#include "System/Base/ComponentDonor.h"

namespace Dystopia
{
	class VideoRenderer;
	class VideoSystem : public Systems, public ComponentDonor<VideoRenderer>
	{
	public:

		VideoSystem();
		~VideoSystem();


		virtual void PreInit(void) override;
		virtual bool Init(void) override;
		virtual void PostInit(void) override;

		virtual void FixedUpdate(float) override;
		virtual void Update(float) override;

		virtual void PostUpdate() override;

		virtual void Shutdown(void) override;

		virtual void LoadDefaults(void) override;
		virtual void LoadSettings(TextSerialiser&) override;
		virtual void SaveSettings(DysSerialiser_t&);

#if EDITOR
		void EditorUI(void);
#endif

	protected:



	private:

	};

}




#endif



