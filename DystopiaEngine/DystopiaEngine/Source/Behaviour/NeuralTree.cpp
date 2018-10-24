#include "Behaviour/NeuralTree.h"

namespace Dystopia
{
	using namespace NeuralTree;

	Node::eStatus Node::Tick(const float _dt)
	{
		if (mStatus != eStatus::RUNNING) {
			Init();
		}

		mStatus = Update(_dt);

		if (mStatus != eStatus::RUNNING) {
			Exit(mStatus);
		}

		return mStatus;
	}
}
