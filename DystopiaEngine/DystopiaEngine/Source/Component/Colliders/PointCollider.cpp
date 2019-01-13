#include "Component/PointCollider.h"
#include "IO/TextSerialiser.h"
namespace Dystopia
{
	void PointCollider::Awake(void)
	{
	}
	void PointCollider::Load(void)
	{
	}
	void PointCollider::Init(void)
	{
	}
	void PointCollider::Update(float)
	{
	}
	void PointCollider::OnDestroy(void)
	{
	}
	void PointCollider::Unload(void)
	{
	}
	PointCollider * PointCollider::Duplicate() const
	{
		return nullptr;
	}
	BroadPhaseCircle PointCollider::GenerateBoardPhaseCircle() const
	{
		return BroadPhaseCircle();
	}
	void PointCollider::Serialise(TextSerialiser & _out) const
	{
		_out.InsertStartBlock("Circle_Collider2D");
		Component::Serialise(_out);
		_out << float(mv3Offset[0]);		// offset for colliders
		_out << float(mv3Offset[1]);
		_out << float(mv3Offset[2]);

		_out << static_cast<float>(mPosition[0]);
		_out << static_cast<float>(mPosition[1]);
		_out << static_cast<float>(mPosition[2]);
		_out << mbIsTrigger;

		_out.InsertEndBlock("Circle_Collider2D");

	}
	void PointCollider::Unserialise(TextSerialiser & _in)
	{
		_in.ConsumeStartBlock();
		Component::Unserialise(_in);
		_in >> mv3Offset[0];
		_in >> mv3Offset[1];
		_in >> mv3Offset[2];

		_in >> (mPosition[0]);
		_in >> (mPosition[1]);
		_in >> (mPosition[2]);
		_in >> mbIsTrigger;
		_in.ConsumeEndBlock();

		mDebugVertices.clear();
	}
	bool PointCollider::isColliding(Circle & other_col, Math::Vec3D other_pos)
	{
		return false;
	}
	bool PointCollider::isColliding(Circle * const & other_col)
	{
		return false;
	}
	bool PointCollider::isColliding(const AABB & other_col)
	{
		return false;
	}
	bool PointCollider::isColliding(const AABB * const & other_col)
	{
		return false;
	}
	bool PointCollider::isColliding(Convex & other_col)
	{
		return false;
	}
	bool PointCollider::isColliding(Convex * const & other_col)
	{
		return false;
	}
}