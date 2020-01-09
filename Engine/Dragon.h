#pragma once
#include "Systems.h"



struct SpringMass
{
	SVec3 vel;
	SVec3 accel;
	float distToNeighbour;

	SMatrix transform;

	SpringMass() {}

	SpringMass(const SVec3& pos)
	{
		transform = SMatrix::CreateTranslation(pos);
	}
};



struct DragonUpdateData
{
	SVec3 playerPos;
	SVec3 wind;
	float speed;
	float dTime;
};



class Dragon
{
	void massSpring();
	SVec3 resolveSpring(SVec3 toNeighbour, float dTime);
	
public:

	Dragon() {}
	~Dragon() {}

	void init(UINT segments, SVec3 initPos);
	void update(const RenderContext& rc, const SVec3& wind, SVec3 target);
	
	DragonUpdateData dragonUpdData;
	std::vector<SpringMass> springs;

	float restLength = 15.f, mass = 5.0, k = 1.f, friction = .94f, flyingSpeed = 55.f, agility = 1.f;
	float invMass = 1.f / mass;
};