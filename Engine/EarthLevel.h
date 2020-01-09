#pragma once
#include "Level.h"



class EarthLevel : public Level
{
public:
	EarthLevel(Systems& sys) : Level(sys) {};
	~EarthLevel() {};

	///each level probably contains these
	PointLight pointLight;
	Model skybox;
	CubeMapper skyboxCubeMapper;
	Model  will;
	std::vector<Procedural::Terrain> procTerrains;

	//specific to the level
	Texture mazeDiffuseMap, mazeNormalMap;

	//sounds
	Audio audio;

	//procedural stuff
	Procedural::Terrain proceduralTerrain;
	Procedural::Perlin perlin;
	Procedural::Maze maze;

	ParticleSystem pSys;
	std::vector<InstanceData> instanceData;
	std::function<void(ParticleUpdateData*)> particleUpdFunc;

	//load and draw all that jazz
	void init(Systems& sys);
	void procGen() {};
	void update(const RenderContext& rc);
	void draw(const RenderContext& rc);
	void demolish()
	{
		this->~EarthLevel();
	};
};