#pragma once

#if defined(_XBOX_ONE) && defined(_TITLE)
#include <d3d11_x.h>
#else
#include <d3d11_1.h>
#endif
namespace DirectX
{

	// Built-in shader supports shadow map.
	class ShadowMap
	{
	public:
		explicit ShadowMap(_In_ ID3D11Device* device, UINT width, UINT height);
		ShadowMap(ShadowMap&& moveFrom);
		ShadowMap& operator= (ShadowMap&& moveFrom);
		virtual ~ShadowMap();

		void BindRenderTarget(ID3D11DeviceContext * deviceContext);

		void UnbindRenderTarget(ID3D11DeviceContext * deviceContext);

		ID3D11ShaderResourceView * GetShadowMapSRV();

	protected:
		void	CreateShaderResource(_In_ ID3D11Device* device, UINT width, UINT height);

	private:
		// Private implementation.
		//class Impl;

		//std::unique_ptr<Impl> pImpl;

		Microsoft::WRL::ComPtr<ID3D11Texture2D> m_shadowMapTex;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_shadowMapRTV;		
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shadowMapSRV;

		Microsoft::WRL::ComPtr<ID3D11Texture2D>                 m_depthBufferTexture;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView>      m_shadowMapDSV;

		ID3D11RenderTargetView* m_originalRTV;
		ID3D11DepthStencilView * m_pOriginalDSV;

		bool skinned;
		int weightsPerVertex;

		D3D11_VIEWPORT m_viewport;
		D3D11_VIEWPORT m_originalViewport;

		// Prevent copying.
		ShadowMap(ShadowMap const&);
		ShadowMap& operator= (ShadowMap const&);
	};

}