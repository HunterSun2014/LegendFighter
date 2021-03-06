//
//
//
#include "pch.h"
#include "EffectCommon.h"
#include "ShadowMap.h"

using namespace DirectX;

//ShadowMap



// Constructor.

void ShadowMap::CreateShaderResource(_In_ ID3D11Device* device, UINT width, UINT height)
{
	using namespace Windows::System::Profile;

	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;
	m_viewport.Width = static_cast<float>(width);
	m_viewport.Height = static_cast<float>(height);
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;

	DXGI_FORMAT format;
	AnalyticsVersionInfo^ analyticsVersion = AnalyticsInfo::VersionInfo;

	if(analyticsVersion->DeviceFamily == L"Windows.Mobile")
		format = DXGI_FORMAT_R16G16B16A16_FLOAT; 
	else
		format = DXGI_FORMAT_R32G32_FLOAT;

	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width			= width;
	texDesc.Height			= height;
	texDesc.MipLevels	= 1;
	texDesc.ArraySize		= 1;
	texDesc.Format = format;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	//
	//		Create Render Target buffer and view
	//
	HRESULT hr = device->CreateTexture2D(&texDesc, nullptr, m_shadowMapTex.ReleaseAndGetAddressOf());
	ThrowIfFailed(hr);

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Format = format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	hr = device->CreateRenderTargetView(m_shadowMapTex.Get(), &rtvDesc, m_shadowMapRTV.ReleaseAndGetAddressOf());
	ThrowIfFailed(hr);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;

	hr = device->CreateShaderResourceView(m_shadowMapTex.Get(), &srvDesc, m_shadowMapSRV.ReleaseAndGetAddressOf());
	ThrowIfFailed(hr);

	//
	//		Create Depth buffer and view
	//

	texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	hr = device->CreateTexture2D(&texDesc, nullptr, m_depthBufferTexture.ReleaseAndGetAddressOf());
	ThrowIfFailed(hr);

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvd;
	dsvd.Flags = 0;
	dsvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvd.Texture2D.MipSlice = 0;
	dsvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	hr = device->CreateDepthStencilView(m_depthBufferTexture.Get(), &dsvd, m_shadowMapDSV.ReleaseAndGetAddressOf());
	ThrowIfFailed(hr);
}

// Public constructor.
ShadowMap::ShadowMap(_In_ ID3D11Device* device, UINT width, UINT height)
{
	CreateShaderResource(device, width, height);
}

ShadowMap::~ShadowMap()
{

}
// Move constructor.
ShadowMap::ShadowMap(ShadowMap&& /*moveFrom*/)
{
}

void ShadowMap::BindRenderTarget(ID3D11DeviceContext * deviceContext)
{
	ID3D11ShaderResourceView *const pSRV[1] = { NULL };
	deviceContext->PSSetShaderResources(1, 1, pSRV);

	UINT numViewport = 1;
	deviceContext->RSGetViewports(&numViewport, &m_originalViewport);
	deviceContext->RSSetViewports(1, &m_viewport);
	
	ID3D11RenderTargetView* renderTargets[1] = { m_shadowMapRTV.Get() };

	deviceContext->OMGetRenderTargets(1, &m_originalRTV, &m_pOriginalDSV);

	deviceContext->OMSetRenderTargets(1, renderTargets, m_shadowMapDSV.Get());

	FLOAT ClearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	deviceContext->ClearRenderTargetView(m_shadowMapRTV.Get(), ClearColor);
	deviceContext->ClearDepthStencilView(m_shadowMapDSV.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}
//	
//	Unbind Render traget and set it to PS shader resource
//
void ShadowMap::UnbindRenderTarget(ID3D11DeviceContext * deviceContext)
{
	ID3D11RenderTargetView* renderTargets[1] = { m_originalRTV };

	deviceContext->RSSetViewports(1, &m_originalViewport);

	deviceContext->OMSetRenderTargets(1, renderTargets, m_pOriginalDSV);

	//ID3D11ShaderResourceView *const pSRV[1] = { m_shadowMapSRV.Get() };
	//deviceContext->PSSetShaderResources(1, 1, pSRV);
	m_originalRTV->Release();
	m_pOriginalDSV->Release();
}

ID3D11ShaderResourceView * ShadowMap::GetShadowMapSRV()
{
	return m_shadowMapSRV.Get();
}