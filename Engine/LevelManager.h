#pragma once
#include <vector>

class Level;
class Systems;
class RenderContext;

class LevelManager
{
	std::vector<Level*> _levels;
	Level* current;

public:
	LevelManager(Systems& systems);
	~LevelManager();

	void advanceLevel(Systems& systems);
	void UpdateDrawCurrent(RenderContext& renderContext);
	void update(Systems& systems, float dTime);

	float sinceLastInput = 0.f;
};