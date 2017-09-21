#pragma once

#include "..\Common\DeviceResources.h"
#include "ShaderStructures.h"
#include "..\Common\StepTimer.h"
#include <Audio.h>
#include "Actor.h"
#include "ShadowMap.h"
#include "skybox.h"
#include "EntityManager.h"
#include "MoveLookController.h"
#include "..\Bullet3\Bullet3.h"

using namespace DirectX;

namespace LegendFighter
{
	// This sample renderer instantiates a basic rendering pipeline.
	class Sample3DSceneRenderer :
		public EntityManager,
		public BulletPhysicsEngine
	{
	public:
		Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources, MoveLookController^ moveLookController);
		//virtual ~Sample3DSceneRenderer();

		void CreateDeviceDependentResources();
		void CreateWindowSizeDependentResources();
		void ReleaseDeviceDependentResources();
		void Update(DX::StepTimer const& timer);
		void Render();

		void StartTracking();
		void TrackingUpdate(float positionX);
		void StopTracking();
		bool IsTracking() { return m_tracking; }

	private:
		void Rotate(float radians);

		void XM_CALLCONV DrawGrid(DirectX::FXMVECTOR xAxis, DirectX::FXMVECTOR yAxis, DirectX::FXMVECTOR origin, size_t xdivs, size_t ydivs, DirectX::GXMVECTOR color);

		void XM_CALLCONV DrawSenceToShadowMap(DirectX::FXMMATRIX view, DirectX::FXMMATRIX project);

		void InitializeEntities();

		void InitializeSenceScript(const wchar_t * scriptFile);		

		void InitializePhysicsEngine();

		void XM_CALLCONV DrawSkeleton(DirectX::Model * pModel, DirectX::FXMMATRIX world, DirectX::FXMMATRIX view, DirectX::FXMMATRIX project);

	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		//
		std::unique_ptr<DirectX::CommonStates>					m_states;		
		std::unique_ptr<DirectX::IEffectFactory>                      m_fxFactory;
		std::unique_ptr<DirectX::IEffectFactory>						m_dgslFactory;
		std::unique_ptr<DirectX::GeometricPrimitive>              m_shape;		
		std::unique_ptr<DirectX::SpriteBatch>							m_sprites;
		std::unique_ptr<DirectX::SpriteFont>							m_font;
		std::unique_ptr<DirectX::Camera>								m_camera;
		std::unique_ptr<DirectX::Keyboard>								m_keyboard;
		
		MoveLookController^										m_moveLookController;

		Model::Collection												m_models;
		CActor::Collection												m_actors;

		//	batch objects
		Microsoft::WRL::ComPtr<ID3D11InputLayout>						m_batchInputLayout;
		std::unique_ptr<DirectX::BasicEffect>							m_batchEffect;
		std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>	m_batch;

		Microsoft::WRL::ComPtr<ID3D11InputLayout>						m_batchTexInputLayout;
		std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionNormalTexture>>	m_batchTex;

		std::unique_ptr<DirectX::ShadowMap>								m_shadowMap;
		std::unique_ptr<DirectX::Skybox>										m_skybox;
			
		//
		std::unique_ptr<DirectX::AudioEngine>                           m_audEngine;
		std::unique_ptr<DirectX::WaveBank>                              m_waveBank;
		std::unique_ptr<DirectX::SoundEffect>                           m_soundEffect;
		std::unique_ptr<DirectX::SoundEffectInstance>              m_effect1;
		std::unique_ptr<DirectX::SoundEffectInstance>              m_effect2;
		
		//
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                m_texture1;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                m_skyboxSRV;
		

		// Direct3D resources for cube geometry.
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_inputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_indexBuffer;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_pixelShader;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_constantBuffer;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>		m_roadSRV;

		// System resources for cube geometry.
		ModelViewProjectionConstantBuffer	m_constantBufferData;
		uint32	m_indexCount;

		DirectX::XMFLOAT4X4 m_world;
		DirectX::XMFLOAT4X4 m_view;
		DirectX::XMFLOAT4X4 m_projection;

		// Variables used with the rendering loop.
		bool	m_loadingComplete;
		float	m_degreesPerSecond;
		bool	m_tracking;

		//touch pointer
		std::unordered_map<unsigned int, XMFLOAT4> m_oldPointer;
		std::unordered_map<unsigned int, XMFLOAT4> m_Pointer;

		jsrt::runtime m_jsRuntime;
		jsrt::context m_jsContext;
		std::shared_ptr<std::function<void(jsrt::function<void>)>> funCallback;
		//std::function<void(jsrt::function<void>) > funCallback;
		/*void PromiseCallback(jsrt::function<void> fun)
		{

		};*/
		JsValueRef result;
		JsValueRef task = JS_INVALID_REFERENCE;
		JsValueRef callback = JS_INVALID_REFERENCE;


		//std::shared_ptr<EntityManager> m_entityManager;

		std::map<std::wstring, std::shared_ptr<DirectX::Model>> m_ModelMap;
		
		virtual std::shared_ptr<DirectX::Model> LoadModel(const std::wstring & name);
		
		virtual std::shared_ptr<DX::DeviceResources> GetDeviceResources() 
		{
			return m_deviceResources;
		};
		
	};
}

