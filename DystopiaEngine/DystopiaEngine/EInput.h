#ifndef _E_INPUT_H_
#define _E_INPUT_H_
#include "../EditorSystem.h"

namespace Editor
{
	class EInput : public EditorSystem
	{
	public:
		void Load(void) override;
		bool Init(void) override;
		void StartFrame(void) override;
		void Update(float) override;
		void EndFrame(void) override;
		void Shutdown(void) override;
		void Message(::Editor::eEMessage) override;
		void SaveSettings(Dystopia::TextSerialiser& _out) const override;
		void LoadSettings(Dystopia::TextSerialiser& _in) override;
		void Poll(void) {}
	private:
	};
}
#endif //_E_INPUT_H_



