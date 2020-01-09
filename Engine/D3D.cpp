#include "D3D.h"

D3D::D3D()
{
	m_swapChain = 0;
	_device = 0;
	_deviceContext = 0;
	m_renderTargetView = 0;
	m_depthStencilBuffer = 0;
	m_depthStencilState = 0;
	DSLessEqual = 0;
	m_depthStencilView = 0;
	m_rasterState = 0;
}


D3D::~D3D()
{
}


bool D3D::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, 
						  float screenDepth, float screenNear)
{
	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, i, numerator, denominator, stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	D3D11_BLEND_DESC blendDesc;

	// Store the vsync setting.
	m_vsync_enabled = vsync;

	// Create a DirectX graphics interface factory.
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if(FAILED(result)){
		return false;
	}

	// Use the factory to create an adapter for the primary graphics interface (video card).
	result = factory->EnumAdapters(0, &adapter);
	if(FAILED(result)){
		return false;
	}

	// Enumerate the primary adapter output (monitor).
	result = adapter->EnumOutputs(0, &adapterOutput);
	if(FAILED(result)){
		return false;
	}

	// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if(FAILED(result)){
		return false;
	}

	// Create a list to hold all the possible display modes for this monitor/video card combination.
	displayModeList = new DXGI_MODE_DESC[numModes];
	if(!displayModeList){
		return false;
	}

	// Now fill the display mode list structures.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if(FAILED(result)){
		return false;
	}

	// Now go through all the display modes and find the one that matches the screen width and height.
	// When a match is found store the numerator and denominator of the refresh rate for that monitor.
	for(i=0; i<numModes; i++){
		if(displayModeList[i].Width == (unsigned int)screenWidth){
			if(displayModeList[i].Height == (unsigned int)screenHeight){
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	// Get the adapter (video card) description.
	result = adapter->GetDesc(&adapterDesc);
	if(FAILED(result)){
		return false;
	}

	// Store the dedicated video card memory in megabytes.
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// Convert the name of the video card to a character array and store it.
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if(error != 0){
		return false;
	}

	// Release the display mode list.
	delete [] displayModeList;
	displayModeList = 0;

	// Release the adapter output.
	adapterOutput->Release();
	adapterOutput = 0;

	// Release the adapter.
	adapter->Release();
	adapter = 0;

	// Release the factory.
	factory->Release();
	factory = 0;

	// Initialize the swap chain description.
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// Set to a single back buffer.
    swapChainDesc.BufferCount = 1;

	// Set the width and height of the back buffer.
    swapChainDesc.BufferDesc.Width = screenWidth;
    swapChainDesc.BufferDesc.Height = screenHeight;

	// Set regular 32-bit surface for the back buffer.
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// Set the refresh rate of the back buffer.
	if(m_vsync_enabled){
	    swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else{
	    swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// Set the usage of the back buffer.
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// Set the handle for the window to render to.
    swapChainDesc.OutputWindow = hwnd;

	// Turn multisampling off.
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;

	// Set to full screen or windowed mode.
	swapChainDesc.Windowed = !fullscreen;

	// Set the scan line ordering and scaling to unspecified.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Discard the back buffer contents after presenting.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Don't set the advanced flags.
	swapChainDesc.Flags = 0;

	// Set the feature level to DirectX 11.
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	// Create the swap chain, Direct3D device, and Direct3D device context.	//@TODO DELETE THE DEBUG FLAG ONCE IT'S NO LONGER NEEDED
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_DEBUG, &featureLevel, 1,
										   D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &_device, NULL, &_deviceContext);
	if(FAILED(result)){
		return false;
	}

	// Get the pointer to the back buffer.
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if(FAILED(result)){
		return false;
	}

	// Create the render target view with the back buffer pointer.
	result = _device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	if(FAILED(result)){
		return false;
	}

	// Release pointer to the back buffer as we no longer need it.
	backBufferPtr->Release();
	backBufferPtr = 0;

	// Initialize the description of the depth buffer.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// Set up the description of the depth buffer.
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Create the texture for the depth buffer using the filled out description.
	result = _device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if(FAILED(result)){
		return false;
	}

	// Initialize the description of the stencil state.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create the depth stencil state.
	result = _device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if(FAILED(result))
		return false;

	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	result = _device->CreateDepthStencilState(&depthStencilDesc, &DSLessEqual);
	if (FAILED(result))
		return false;

	// Set the depth stencil state.
	_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	// Initialize the depth stencil view.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	result = _device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if(FAILED(result)){
		return false;
	}

	// Bind the render target view and depth stencil buffer to the output render pipeline.
	_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

	// Setup the raster description which will determine how and what polygons will be drawn.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;	//D3D11_FILL_SOLID	D3D11_FILL_WIREFRAME
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	result = _device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if (FAILED(result)) {
		return false;
	}

	rasterDesc.CullMode = D3D11_CULL_NONE;

	result = _device->CreateRasterizerState(&rasterDesc, &m_rasterStateNoCull);
	if (FAILED(result)) {
		return false;
	}

	// Now set the rasterizer state.
	_deviceContext->RSSetState(m_rasterState);


	//blending code @TODO see why it messes with texture.... turn off until then
	ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));

	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f; //D3D11_COLOR_WRITE_ENABLE_ALL;

	result = _device->CreateBlendState(&blendDesc, &m_blendState);
	if (FAILED(result)) {
		return false;
	}
	

	// Modify the description to create an alpha disabled blend state description.
	blendDesc.RenderTarget[0].BlendEnable = FALSE;
	// Create the blend state using the description.
	result = _device->CreateBlendState(&blendDesc, &m_noBlendState);
	if (FAILED(result)){
		return false;
	}
	
	// Setup the viewport for rendering.
    viewport.Width = (float)screenWidth;
    viewport.Height = (float)screenHeight;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;

	// Create the viewport.
    _deviceContext->RSSetViewports(1, &viewport);

    return true;
}





void D3D::Shutdown(){

	// Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
	if(m_swapChain){
		m_swapChain->SetFullscreenState(false, NULL);
	}

	if(m_rasterState){
		m_rasterState->Release();
		m_rasterState = 0;
	}

	if(m_depthStencilView){
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}

	if(m_depthStencilState){
		m_depthStencilState->Release();
		m_depthStencilState = 0;
	}

	if(m_depthStencilBuffer){
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}

	if(m_renderTargetView){
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	if(_deviceContext){
		_deviceContext->Release();
		_deviceContext = 0;
	}

	if(_device){
		_device->Release();
		_device = 0;
	}

	if(m_swapChain){
		m_swapChain->Release();
		m_swapChain = 0;
	}

	return;
}


void D3D::ClearColourDepthBuffers(float* clearColour)
{
	_deviceContext->ClearRenderTargetView(m_renderTargetView, clearColour);
	_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}


void D3D::EndScene(){
	// Present the back buffer to the screen since rendering is complete.
	if(m_vsync_enabled)
		m_swapChain->Present(1, 0);	// Lock to screen refresh rate.
	else
		m_swapChain->Present(0, 0);	// Present as fast as possible.
}


ID3D11Device* D3D::GetDevice(){
	return _device;
}


ID3D11DeviceContext* D3D::GetDeviceContext(){
	return _deviceContext;
}

void D3D::GetVideoCardInfo(char* cardName, int& memory){
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
}

ID3D11DepthStencilView* D3D::GetDepthStencilView()
{
	return m_depthStencilView;
}

void D3D::SetBackBufferRenderTarget()
{
	_deviceContext->RSSetViewports(1, &viewport);
	_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);
	ClearColourDepthBuffers(clearColour);
}

void D3D::D3D::TurnOnAlphaBlending()
{
	float blendFactor[4] = { 0.5f, 0.5f, 0.5f, 0.5f };

	_deviceContext->OMSetBlendState(m_blendState, blendFactor, 0xffffffff);
}


void D3D::D3D::TurnOffAlphaBlending()
{
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	_deviceContext->OMSetBlendState(m_noBlendState, blendFactor, 0xffffffff);
}

void D3D::TurnOnCulling() {
	_deviceContext->RSSetState(m_rasterState);
}

void D3D::TurnOffCulling() {
	_deviceContext->RSSetState(m_rasterStateNoCull);
}

void D3D::SwitchDepthToLessEquals()
{
	_deviceContext->OMSetDepthStencilState(DSLessEqual, 1);
}

void D3D::SwitchDepthToDefault(){
	_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);
}
