#pragma once

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include "ShaderBase.h"
#include "Camera.h"
#include "ShaderDataStructs.h"

class Model;

class ShaderVolumetric : public ShaderBase {

public:
	ShaderVolumetric();
	~ShaderVolumetric();

	bool Initialize(ID3D11Device*, HWND, const std::vector<std::wstring> filePaths, 
		std::vector<D3D11_INPUT_ELEMENT_DESC> layoutDesc, const D3D11_SAMPLER_DESC& samplerDesc);
	
	bool SetShaderParameters(ID3D11DeviceContext*, const Model& m, const Camera& camera, float elapsed);

private:
	ID3D11Buffer* _viewRayBuffer;
};

