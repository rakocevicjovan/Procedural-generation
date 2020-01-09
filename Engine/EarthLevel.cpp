#include "EarthLevel.h"


void EarthLevel::init(Systems& sys)
{
	skybox.LoadModel(device, "../Models/Skysphere.fbx");
	skyboxCubeMapper.LoadFromFiles(device, "../Textures/night.dds");

	will.LoadModel(device, "../Models/crystal/source/model/model.dae");
	Math::Scale(will.transform, SVec3(10.f));
	Math::Translate(will.transform, SVec3(2, 35, 60));

	LightData lightData(SVec3(0.1f, 0.7f, 0.9f), .03f, SVec3(0.8f, 0.8f, 1.0f), .2f, SVec3(0.3f, 0.5f, 1.0f), 0.7f);
	pointLight = PointLight(lightData, SVec4(333.f, 666.f, 999.f, 1.0f));	//old moon position SVec4(50.0f, 250.f, 250.0f, 1.0f)

	proceduralTerrain = Procedural::Terrain(320, 320);
	proceduralTerrain.setTextureData(device, 16, 16, { "../Textures/Lava/diffuse.jpg", "../Textures/Lava/normal.jpg" });
	proceduralTerrain.SetUp(device);

	mazeDiffuseMap = Texture(device, "../Textures/Rock/diffuse.jpg");
	mazeNormalMap = Texture(device, "../Textures/Rock/normal.jpg");

	maze.Init(10, 10, 32);
	maze.CreateModel(device);

	sys._colEngine.registerModel(maze.model, BoundingVolumeType::BVT_AABB);

	SMatrix goalMat = SMatrix::CreateTranslation(maze.GetRandCellPos());
	pSys.init(&will, 10, goalMat);
	goal = goalMat.Translation();
	will.transform = goalMat;

	particleUpdFunc = [this](ParticleUpdateData* _pud) -> void
	{
		for (int i = 0; i < pSys._particles.size(); ++i)
		{
			SVec3 translation(2, 2, 2);
			pSys._particles[i]->age += _pud->dTime;
			SMatrix rotMat = SMatrix::CreateRotationY(PI * pSys._particles[i]->age);

			translation.x *= sin(pSys._particles[i]->age * 0.2f * (float)(i + 1));
			translation.y *= cos(pSys._particles[i]->age  * ((float)pSys._particles.size() - (float)i));
			translation.z *= cos(pSys._particles[i]->age * 0.2f * (float)(i + 1));
			pSys._particles[i]->transform = rotMat;
			Math::SetTranslation(pSys._particles[i]->transform, translation * (float)i * -0.33f);
		}
	};
	pSys.setUpdateFunction(particleUpdFunc);
	instanceData.resize(pSys._numParticles);
	shady.instanced._instanceCount = 10;

	randy._cam.SetTranslation(SVec3(16, 10, 16));
	_sys._controller.toggleFly();
}



void EarthLevel::update(const RenderContext & rc)
{
	updateCam(rc.dTime);
	win(rc.cam->GetPosition(), 10.f);

	if (!_sys._controller.isFlying())
	{
		SVec3 oldPos = _sys._renderer._cam.GetCameraMatrix().Translation();
		float newHeight = proceduralTerrain.getHeightAtPosition(rc.cam->GetPosition());
		if (rc.cam->GetPosition().y < newHeight)
			rc.cam->SetTranslation(SVec3(oldPos.x, newHeight, oldPos.z));
	}
	
	pSys.update(rc.dTime * .33f);
	for (int i = 0; i < pSys._particles.size(); ++i)
		instanceData[i]._m = pSys._particles[i]->transform.Transpose();
}



void EarthLevel::draw(const RenderContext& rc)
{
	rc.d3d->SetBackBufferRenderTarget();

	proceduralTerrain.Draw(context, rc.shMan->terrainNormals, *rc.cam, pointLight, rc.elapsed);

	shady.instanced.UpdateInstanceData(instanceData);
	shady.instanced.SetShaderParameters(context, will, *rc.cam, pointLight, rc.elapsed);
	will.DrawInstanced(context, shady.instanced);


	rc.shMan->dynamicHeightMaze.SetShaderParameters(context, maze.model, *rc.cam, pointLight, rc.elapsed, mazeDiffuseMap, mazeNormalMap);
	maze.model.Draw(context, rc.shMan->dynamicHeightMaze);
	rc.shMan->dynamicHeightMaze.ReleaseShaderParameters(context);

	randy.RenderSkybox(*rc.cam, skybox, skyboxCubeMapper);

	rc.d3d->EndScene();

	ProcessSpecialInput(rc.dTime);
}