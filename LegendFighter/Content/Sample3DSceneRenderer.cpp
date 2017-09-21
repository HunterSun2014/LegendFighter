#include "pch.h"
#include "Sample3DSceneRenderer.h"
#include "Log.h"

#include "..\Common\DirectXHelper.h"


using namespace LegendFighter;
using namespace DirectX;
using namespace Windows::Foundation;
using namespace std;

// Loads vertex and pixel shaders from files and instantiates the cube geometry.
Sample3DSceneRenderer::Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources, MoveLookController^ moveLookController) :
	m_loadingComplete(false),
	m_degreesPerSecond(45),
	m_indexCount(0),
	m_tracking(false),
	m_deviceResources(deviceResources),
	m_moveLookController(moveLookController)
{
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();

}

// Initializes view parameters when the window size changes.
void Sample3DSceneRenderer::CreateWindowSizeDependentResources()
{
	Size outputSize = m_deviceResources->GetOutputSize();
	float aspectRatio = outputSize.Width / outputSize.Height;
	float fovAngleY = 90.0f * XM_PI / 180.0f;
	aspectRatio = 1 / aspectRatio;

	// This is a simple example of change that can be made when the app is in
	// portrait or snapped view.
	if (aspectRatio < 1.0f)
	{
		//fovAngleY *= 2.0f;
	}
	/*
	DXGI_MODE_ROTATION rotation = m_deviceResources->;//DXGI_MODE_ROTATION_UNSPECIFIED;
	switch (m_orientation)
	{
	case DisplayOrientations::Portrait: rotation = DXGI_MODE_ROTATION_IDENTITY;  break;
	case DisplayOrientations::Landscape: rotation = DXGI_MODE_ROTATION_ROTATE90; break;
	case DisplayOrientations::PortraitFlipped: rotation = DXGI_MODE_ROTATION_ROTATE180; break;
	case DisplayOrientations::LandscapeFlipped: rotation = DXGI_MODE_ROTATION_ROTATE270; break;
	}
	spriteBatch->SetRotation(rotation);
	*/
	// Note that the OrientationTransform3D matrix is post-multiplied here
	// in order to correctly orient the scene to match the display orientation.
	// This post-multiplication step is required for any draw calls that are
	// made to the swap chain render target. For draw calls to other targets,
	// this transform should not be applied.

	// This sample makes use of a right-handed coordinate system using row-major matrices.
	XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovRH(
		fovAngleY,
		aspectRatio,
		0.01f,
		1000.0f
		);

	XMFLOAT4X4 orientation = m_deviceResources->GetOrientationTransform3D();

	XMMATRIX orientationMatrix = XMLoadFloat4x4(&orientation);

	XMStoreFloat4x4(
		&m_constantBufferData.projection,
		XMMatrixTranspose(perspectiveMatrix * orientationMatrix)
		);

	// Eye is at (0,0.7,1.5), looking at point (0,-0.1,0) with the up-vector along the y-axis.
	static const XMVECTORF32 eye = { 0.0f, 0.7f, 10.5f, 0.0f };
	static const XMVECTORF32 at = { 0.0f, 0.7f, 0.0f, 0.0f };
	static const XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };

	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixLookAtRH(eye, at, up)));

	//m_camera.reset(outputSize.Height < outputSize.Width ? new Camera(outputSize.Height, outputSize.Width) : new Camera(outputSize.Width, outputSize.Height ));
	m_camera.reset(new Camera(outputSize.Height, outputSize.Width));

	m_camera->SetPosition(XMVectorSet(0.0f, 2.50f, -16.0f, 0.0f));	//0.0f, 2.0f, -9.0f, 0.0f

	XMMATRIX projection = XMMatrixMultiply(perspectiveMatrix, orientationMatrix);

	//m_batchEffect->SetProjection(m_camera->GetProjectionMatrix());

	XMStoreFloat4x4(
		&m_projection,
		perspectiveMatrix
		);

	m_skybox->CreateWindowSizeDependentResources(outputSize.Width, outputSize.Height);
}

// Called once per frame, rotates the cube and calculates the model and view matrices.
void Sample3DSceneRenderer::Update(DX::StepTimer const& timer)
{
	//if (!m_audEngine->Update())
	{
		// No audio device is active
		//if (m_audEngine->IsCriticalError())
		{

		}
	}

	//Update Physica Engine 
	if (m_dynamicsWorld)
	{
		m_dynamicsWorld->stepSimulation((btScalar)timer.GetElapsedSeconds());

#ifdef _DEBUG
		for (int j = m_dynamicsWorld->getNumCollisionObjects() - 1; j >= 0; j--)
		{
			btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[j];
			btRigidBody* body = btRigidBody::upcast(obj);
			btTransform trans;
			if (body && body->getMotionState())
			{
				body->getMotionState()->getWorldTransform(trans);
			}
			else
			{
				trans = obj->getWorldTransform();
			}

			/*Log().Get() << "world pos object " << j
			<< " = " << float(trans.getOrigin().getX())
			<< ", " << float(trans.getOrigin().getY())
			<< ", " << float(trans.getOrigin().getZ())
			<< endl;*/

			//printf("world pos object %d = %f,%f,%f\n", j, float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ()));
		}
#endif // DEBUG

		
	}

	if (!m_tracking)
	{
		// Convert degrees to radians, then convert seconds to rotation angle
		float radiansPerSecond = XMConvertToRadians(m_degreesPerSecond);
		double totalRotation = timer.GetTotalSeconds() * radiansPerSecond;
		float radians = static_cast<float>(fmod(totalRotation, XM_2PI));

		Rotate(radians);
	}

	m_camera->Update((float)timer.GetElapsedSeconds());
	//m_camera->MoveCamera(XMLoadFloat3(&m_moveLookController->Velocity()));

	XMVECTOR eye = XMVectorSet(0.0f, 2.0f, 10.0f, 0.0f);
	XMVECTOR at = XMVectorSet(0.0f, 2.0f, 0.0f, 0.0f);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	//eye = XMLoadFloat3(&m_moveLookController->Velocity());
	//at = XMLoadFloat3(&m_moveLookController->get_LookPoint());
	
	XMMATRIX view = XMMatrixLookAtRH(eye, at, up);
	//view = XMMatrixLookToRH(eye, XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f), up);
	//XMMATRIX world = XMMatrixRotationY(float(timer.GetTotalSeconds() * XM_PIDIV4));
	
	XMMATRIX world = XMMatrixIdentity();

	m_batchEffect->SetView(m_camera->GetViewMatrix());
	m_batchEffect->SetWorld(XMMatrixIdentity());

	XMStoreFloat4x4(&m_view, view);
	XMStoreFloat4x4(&m_world, world);
	
	jsrt::context::scope scope(m_jsContext);
	EntityManager::Update(timer.GetTotalSeconds(), timer.GetElapsedSeconds());
	//EntityManager::Update(0, 0);

	JsValueRef global;
	JsGetGlobalObject(&global);

	// execute async tasks stored in callback
	while (callback!= JS_INVALID_REFERENCE) {
		task = callback;		
		callback = JS_INVALID_REFERENCE;
		Log().Get() << L"before execute async tasks stored in callback" << endl;
		JsCallFunction(task, &global, 1, &result);
		Log().Get() << L"after execute async tasks stored in callback" << endl;
		JsRelease(task, nullptr);		
	}

	JsRelease(global, nullptr);
}

// Rotate the 3D cube model a set amount of radians.
void Sample3DSceneRenderer::Rotate(float radians)
{
	// Prepare to pass the updated model matrix to the shader
	XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMMatrixRotationY(radians)));
}

void Sample3DSceneRenderer::StartTracking()
{
	m_tracking = true;
}

// When tracking, the 3D cube can be rotated around its Y axis by tracking pointer position relative to the output screen width.
void Sample3DSceneRenderer::TrackingUpdate(float positionX)
{
	if (m_tracking)
	{
		float radians = XM_2PI * 2.0f * positionX / m_deviceResources->GetOutputSize().Width;
		Rotate(radians);
	}
}

void Sample3DSceneRenderer::StopTracking()
{
	m_tracking = false;
}

// Renders one frame using the vertex and pixel shaders.
void Sample3DSceneRenderer::Render()
{
	// Loading is asynchronous. Only draw geometry after it's loaded.
	if (!m_loadingComplete)
	{
		return;
	}

	auto context = m_deviceResources->GetD3DDeviceContext();
		
	//set matrix view and projection for shadow map
	XMVECTOR lightDir = XMVectorSet(0.7198464f, -0.3420201f, 0.6040227f, 0.0f);
	XMVECTOR eye = -lightDir * 10 ;
	XMVECTOR at = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX smView = XMMatrixLookAtLH(eye, at, up);
	XMMATRIX smProjection = XMMatrixOrthographicLH(20.f, 20.f, 0.1f, 100.f);		//parallel light liks sun light
	//XMMATRIX smProjection = XMMatrixPerspectiveFovLH(XM_PIDIV4, 4 / (float)3, 0.5f, 1000.0f);

	//Draw all objects except floor to shadow map
	DrawSenceToShadowMap(smView, smProjection);
		
	context->GSSetShader(NULL, NULL, 0);

	// Draw procedurally generated dynamic grid
	const XMVECTORF32 xaxis = { 20.f, 0.f, 0.f };
	const XMVECTORF32 yaxis = { 0.f, 0.f, 20.f };
	//DrawGrid(xaxis, yaxis, g_XMZero, 20, 20, Colors::Black);

	// Draw all models
	
	XMMATRIX view = XMLoadFloat4x4(&m_view);
	XMMATRIX projection = XMLoadFloat4x4(&m_projection);
		
	//view = m_camera->GetViewMatrix();
	//projection = m_camera->GetProjectionMatrix();
	
	/*
	for (auto & actor : m_actors)
	{
		
		auto update = [&](IEffect* pEffect)
		{
			SkinnedEffect * pSkinned = dynamic_cast<SkinnedEffect*>(pEffect);
			
			if (pSkinned)
			{				
				pSkinned->SetShadowMapTransform(actor->GetWorldMartix() * smView * smProjection);
			}
			BasicEffect * pBasicEffect = dynamic_cast<BasicEffect*>(pEffect);
			
			if(pBasicEffect)
			{
				pBasicEffect->SetDiffuseColor(XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));
				pBasicEffect->SetShadowMapTransform(actor->GetWorldMartix() * smView * smProjection);
				pBasicEffect->SetShadowMap(false);
			}

			IEffectLights * pLights = dynamic_cast<IEffectLights*>(pEffect);
			if (pLights)
			{
				pLights->SetLightEnabled(1, true);
				//2015.11.03,方向光设置错误，地面阴影计算将被优化掉，无法在图形分析工具中看见PS state. 
				pLights->SetLightDirection(1, lightDir);		
				pLights->SetLightEnabled(2, true);
			}

			IEffectShadowMap* pEffectShadowMap = dynamic_cast<IEffectShadowMap*>(pEffect);
			if (pEffectShadowMap)
				pEffectShadowMap->SetRenderingShadowMap(false);

		};

		actor->UpdateEffects(update);

		actor->Render(context, *m_states, view, projection, false, false);

		//actor->RenderBoundingVolume(view, projection, Colors::White);
	}
	*/
	// set shadow map shader resource
	ID3D11ShaderResourceView * pSRV[1] = { m_shadowMap->GetShadowMapSRV() };
	context->PSSetShaderResources(1, 1, pSRV);

	view = m_camera->GetViewMatrix();
	projection = m_camera->GetProjectionMatrix();
	projection = projection * XMLoadFloat4x4(&m_deviceResources->GetOrientationTransform3D());

	for (auto & actor : mCharacters)
	{

		auto update = [&](IEffect* pEffect)
		{
			IEffectLights * pLights = dynamic_cast<IEffectLights*>(pEffect);
			if (pLights)
			{
				pLights->SetPerPixelLighting(true);
				pLights->SetLightEnabled(1, true);
				//2015.11.03,方向光设置错误，地面阴影计算将被优化掉，无法在图形分析工具中看见PS state. 
				pLights->SetLightDirection(1, lightDir);
				pLights->SetLightEnabled(2, true);
			}

			BasicEffect * pBasicEffect = dynamic_cast<BasicEffect*>(pEffect);
			if (pBasicEffect)
			{
				pBasicEffect->SetDiffuseColor(XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f));	//默认WindMillStage模型Diffuse color很低，造成场景黑暗 
			}

			DGSLEffect * pDgslEffect = dynamic_cast<DGSLEffect*>(pEffect);
			if (pDgslEffect)
			{
				pDgslEffect->SetTextureEnabled(true);				
				pDgslEffect->SetTexture2(pSRV[0]);		//set shadow map texture to slot 2
			}

			IEffectShadowMap* pEffectShadowMap = dynamic_cast<IEffectShadowMap*>(pEffect);
			if (pEffectShadowMap)
			{
				pEffectShadowMap->SetRenderingShadowMap(false);
				pEffectShadowMap->SetShadowMapTransform(actor->GetWorldMartix() * smView * smProjection);
			}
		};

		actor->UpdateEffects(update);

		actor->Render(context, *m_states, view, projection);

		//actor->RenderBoundingVolume(view, projection, Colors::White);

		//DrawSkeleton(actor->GetModel().get(), actor->GetWorldMartix(), view, projection);
	}

	pSRV[0] = NULL;
	context->PSSetShaderResources(1, 1, pSRV);
	ID3D11ShaderResourceView* txt[8] = { 0 };
	context->PSSetShaderResources(0, 8, txt);


	//reset draw FillMode with solid 
	context->RSSetState(m_states->CullCounterClockwise());
	XMMATRIX w = XMMatrixTranslationFromVector(m_camera->GetPosition());
	
	//draw skybox
	m_skybox->Render(w*view*projection, context);

}

void Sample3DSceneRenderer::CreateDeviceDependentResources()
{
	//Initialize Audio Engine
	AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
	//eflags = eflags | AudioEngine_Debug;
#endif
	m_audEngine = std::make_unique<AudioEngine>(eflags);

	// Load shaders asynchronously.
	auto loadVSTask = DX::ReadDataAsync(L"SampleVertexShader.cso");
	auto loadPSTask = DX::ReadDataAsync(L"SamplePixelShader.cso");

	// After the vertex shader file is loaded, create the shader and input layout.
	auto createVSTask = loadVSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateVertexShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_vertexShader
				)
			);

		static const D3D11_INPUT_ELEMENT_DESC vertexDesc [] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		m_inputLayout.Reset();
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateInputLayout(
				vertexDesc,
				ARRAYSIZE(vertexDesc),
				&fileData[0],
				fileData.size(),
				&m_inputLayout
				)
 			);
	});

	// After the pixel shader file is loaded, create the shader and constant buffer.
	auto createPSTask = loadPSTask.then([this](const std::vector<byte>& fileData) {
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreatePixelShader(
				&fileData[0],
				fileData.size(),
				nullptr,
				&m_pixelShader
				)
			);

		CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer) , D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&constantBufferDesc,
				nullptr,
				&m_constantBuffer
				)
			);
	});

	// Once both shaders are loaded, create the mesh.
	auto createCubeTask = (createPSTask && createVSTask).then([this] () {

		// Load mesh vertices. Each vertex has a position and a color.
		static const VertexPositionColor cubeVertices[] = 
		{
			{XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, 0.0f)},
			{XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f)},
			{XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f)},
			{XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT3(0.0f, 1.0f, 1.0f)},
			{XMFLOAT3( 0.5f, -0.5f, -0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f)},
			{XMFLOAT3( 0.5f, -0.5f,  0.5f), XMFLOAT3(1.0f, 0.0f, 1.0f)},
			{XMFLOAT3( 0.5f,  0.5f, -0.5f), XMFLOAT3(1.0f, 1.0f, 0.0f)},
			{XMFLOAT3( 0.5f,  0.5f,  0.5f), XMFLOAT3(1.0f, 1.0f, 1.0f)},
		};

		D3D11_SUBRESOURCE_DATA vertexBufferData = {0};
		vertexBufferData.pSysMem = cubeVertices;
		vertexBufferData.SysMemPitch = 0;
		vertexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(cubeVertices), D3D11_BIND_VERTEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&vertexBufferDesc,
				&vertexBufferData,
				&m_vertexBuffer
				)
			);

		// Load mesh indices. Each trio of indices represents
		// a triangle to be rendered on the screen.
		// For example: 0,2,1 means that the vertices with indexes
		// 0, 2 and 1 from the vertex buffer compose the 
		// first triangle of this mesh.
		static const unsigned short cubeIndices [] =
		{
			0,2,1, // -x
			1,2,3,

			4,5,6, // +x
			5,7,6,

			0,1,5, // -y
			0,5,4,

			2,6,7, // +y
			2,7,3,

			0,4,6, // -z
			0,6,2,

			1,3,7, // +z
			1,7,5,
		};

		m_indexCount = ARRAYSIZE(cubeIndices);

		D3D11_SUBRESOURCE_DATA indexBufferData = {0};
		indexBufferData.pSysMem = cubeIndices;
		indexBufferData.SysMemPitch = 0;
		indexBufferData.SysMemSlicePitch = 0;
		CD3D11_BUFFER_DESC indexBufferDesc(sizeof(cubeIndices), D3D11_BIND_INDEX_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&indexBufferDesc,
				&indexBufferData,
				&m_indexBuffer
				)
			);
	});

	// Once the cube is loaded, the object is ready to be rendered.
	createCubeTask.then([this]() {		
	});
		//
		//	Create DirectXTK objects
		//
		HRESULT hr = S_OK;
		auto device = m_deviceResources->GetD3DDevice();
		auto context = m_deviceResources->GetD3DDeviceContext();

		m_states.reset(new CommonStates(device));
		m_keyboard.reset(new Keyboard());
		//m_keyboard->SetWindow(m_deviceResources->ge)

		auto fxDgsl = new DGSLEffectFactory(device);

		m_dgslFactory.reset(fxDgsl);

		auto fx = new EffectFactory(device);
		fx->SetDirectory(L"Assets");
		m_fxFactory.reset(fx);

		m_batch.reset(new PrimitiveBatch<DirectX::VertexPositionColor>(context));

		m_batchEffect.reset(new BasicEffect(device));

		m_batchEffect->SetVertexColorEnabled(true);

		{
			void const * shaderByteCode;
			size_t byteCodeLength;

			m_batchEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

			hr = device->CreateInputLayout(DirectX::VertexPositionColor::InputElements,
				DirectX::VertexPositionColor::InputElementCount,
				shaderByteCode, byteCodeLength,
				m_batchInputLayout.ReleaseAndGetAddressOf());
			DX::ThrowIfFailed(hr);
		}

		//create objects for drawing grid with texture
		m_batchTex.reset(new PrimitiveBatch<DirectX::VertexPositionNormalTexture>(context));

		//	load texture for drawing road of batch Effect 
		hr = CreateDDSTextureFromFile(device, L"assets\\layingrock.dds", NULL, m_roadSRV.ReleaseAndGetAddressOf());
		DX::ThrowIfFailed(hr);

		//	create shadow map
		m_shadowMap.reset(new ShadowMap(device, 1024, 1024));

		hr = CreateDDSTextureFromFile(device, L"assets\\sky_cube.dds", NULL, &m_skyboxSRV);
		DX::ThrowIfFailed(hr);

		// create sky box
		m_skybox.reset(new Skybox());
		m_skybox->CreateDeviceDependentResources(device, 50.0f, NULL, m_skyboxSRV.Get());

		//
		//InitializeEntities();//

		//Initialize sence script instead of initializing entities
		InitializeSenceScript(L"Assets\\JavaScript\\sence.js");	//sence.js Level01.js

		InitializePhysicsEngine();

		m_loadingComplete = true;	
}

void Sample3DSceneRenderer::InitializeEntities()
{
	HRESULT hr = S_OK;
	auto device = m_deviceResources->GetD3DDevice();
	auto context = m_deviceResources->GetD3DDeviceContext();

	//EffectFactory* fx = static_cast<EffectFactory*>(m_fxFactory.get());
	auto fx = new DGSLEffectFactory(device);
	//fx->SetDirectory(L"Assets");

	m_dgslFactory.reset(fx);

	// create character soldier model
	//fx->SetDirectory(L"Assets\\Soldier");
	//auto temp = Model::CreateFromSDKMESH(device, L"assets\\Soldier\\soldier.sdkmesh", *m_fxFactory, true);	//ccw cull mode
	//shared_ptr<Model> model = move(temp);

	//model->LoadAnimation(L"assets\\Soldier\\soldier.sdkmesh_anim");
	//model->TransformBindPose(XMMatrixIdentity());
	//model->meshes[1]->boundingBox.Extents.x -= 0.15f;
	//m_models.push_back(model);

	////note that to use counterclockwise cull mode the z axis must be negative  
	//const XMVECTORF32 scale = { 4.0f, 4.0f, 4.0f };
	//const XMVECTORF32 translation = { 1.5f, 0.f, -3.f };
	//XMVECTORF32 rotation = { 0.f, XM_PI, 0.f };

	//auto actor = make_shared<CActor>();
	//actor->Set(model, scale, rotation, translation);
	//actor->CreateShapeforBoundingVolume(m_deviceResources->GetD3DDeviceContext());
	//m_actors.push_back(actor);
	
	auto model = Model::CreateFromCMO(device, L"ThirdPersonWalk.cmo", *m_dgslFactory);		//gamelevel.cmo ThirdPersonWalk.fbx

	auto actor = make_shared<CActor>();
	actor->Set(move(model), XMVectorSet(0.02f, 0.02f, -0.02f, 0.f), XMVectorSet(XM_PIDIV2, 0.f, 0.f, 0.f), XMVectorSet(3.f, 0.f, 0.f, 0.f));
	actor->CreateShapeforBoundingVolume(m_deviceResources->GetD3DDeviceContext());
	actor->SetAnimation(wstring(L"Unreal Take"));

	m_actors.push_back(actor);

	model = Model::CreateFromCMO(device, L"ThirdPersonRun.cmo", *m_dgslFactory);		//gamelevel.cmo ThirdPersonWalk.fbx

	actor = make_shared<CActor>();
	//actor->Set(move(model), XMVectorSet(5.f, 5.f, -5.f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 0.f), XMVectorSet(10.f, 0.f, 0.f, 0.f));
	actor->Set(move(model), XMVectorSet(0.02f, 0.02f, -0.02f, 0.f), XMVectorSet(XM_PIDIV2, 0.f, 0.f, 0.f), XMVectorSet(-5.f, 0.f, 0.f, 0.f));
	actor->CreateShapeforBoundingVolume(m_deviceResources->GetD3DDeviceContext());
	actor->SetAnimation(wstring(L"Unreal Take"));
	m_actors.push_back(actor);

	//model = Model::CreateFromCMO(device, L"JumpingJacks.cmo", *m_dgslFactory);		//gamelevel.cmo ThirdPersonWalk.fbx

	//actor = make_shared<CActor>();
	//actor->Set(move(model), XMVectorSet(5.f, 5.f, -5.f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 0.f), XMVectorSet(10.f, 0.f, 0.f, 0.f));
	//actor->Set(move(model), XMVectorSet(0.05f, 0.05f, 0.05f, 0.f), XMVectorSet(-XM_PIDIV2, 0.f, 0.f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 0.f));
	//actor->CreateShapeforBoundingVolume(m_deviceResources->GetD3DDeviceContext());
	//actor->SetAnimation(L"Unreal Take");

	//m_actors.push_back(actor);

/*	
	const XMVECTORF32 translation1 = { -1.5f, 0.f, -3.f };
	XMVECTORF32 rotation1 = { 0.f, 0.f, 0.f };

	actor = make_shared<CActor>();
	actor->Set(model, scale, rotation1, translation1);
	actor->CreateShapeforBoundingVolume(m_deviceResources->GetD3DDeviceContext());
	m_actors.push_back(actor);

	
	fx->SetDirectory(L"Assets\\Warrior");
	temp = Model::CreateFromSDKMESH(device, L"assets\\Warrior\\WindMillStage.sdkmesh", *m_fxFactory, false);
	//m_model->LoadAnimation(L"assets\\Warrior\\Warrior.sdkmesh_anim");
	model = shared_ptr<Model>(temp.release());

	actor = make_shared<CActor>();
	actor->Set(model, XMVectorSet(2.0f, 2.0f, -2.0f, 0.0f), XMVectorSet(0.0f, XM_PI, 0.0f, 0.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f));
	actor->CreateShapeforBoundingVolume(m_deviceResources->GetD3DDeviceContext());
	m_actors.push_back(actor);

	model = move(Model::CreateFromSDKMESH(device, L"assets\\Warrior\\fan.sdkmesh", *m_fxFactory, false));
	//m_model->LoadAnimation(L"assets\\Warrior\\Warrior.sdkmesh_anim");
	m_ModelMap[model->name] = model;

	actor = make_shared<CActor>();
	actor->Set(model, XMVectorSet(1.0f, 1.0f, -1.0f, 0.0f), XMVectorSet(0.0f, XM_PI, 0.0f, 0.0f), XMVectorSet(0.0f, 7.24f, 0.0f, 4.0f));
	actor->CreateShapeforBoundingVolume(m_deviceResources->GetD3DDeviceContext());
	m_actors.push_back(actor);
	*/
}
static void CALLBACK PromiseContinuationCallback(JsValueRef task, void *callbackState)
{
	// save async task in the callback.
	*(void **)callbackState = task;
	JsAddRef(task, nullptr);
}
void Sample3DSceneRenderer::InitializeSenceScript(const wchar_t * scriptFile)
{
	//
	//	Initialize Java Scirpt Engine
	//

	m_jsRuntime = jsrt::runtime::create(JsRuntimeAttributeDisableNativeCodeGeneration);
	m_jsContext = m_jsRuntime.create_context();
	
	jsrt::context::scope scope(m_jsContext);

#ifdef _DEBUG
	JsStartDebugging();
#endif

	//Reove Enable Debugging for Win10
	//Chakra::EnableDebugging();

	//m_entityManager = make_shared<EntityManager>();
	//m_entityManager->JsRegister(jsrt::context::global());
	JsRegister(jsrt::context::global());
	
	m_moveLookController->JsRegister(m_jsContext, jsrt::context::global());

	//Registry audio engine
	jsrt::object jsAudioEngine = jsrt::object::create();
	(jsrt::context::global()).set_property(jsrt::property_id::create(L"AudioEngine"), jsAudioEngine);

	//Create Sound Effect method for AudioEngine JS object
	auto CreateSoundEffect = [](_In_ JsValueRef callee, _In_ bool isConstructCall, _In_ JsValueRef *arguments, _In_ unsigned short argumentCount, _In_opt_ void *callbackState)->JsValueRef 
	{ 
		wstring fileName;
		jsrt::marshal::to_native(arguments[1], &fileName);

		auto pThis = reinterpret_cast<Sample3DSceneRenderer*>(callbackState);
		auto soundEffect = new SoundEffect(pThis->m_audEngine.get(), fileName.c_str()); //std::make_shared<SoundEffect>(pThis->m_audEngine.get(), fileName.c_str());
	
		jsrt::object jsSoundEffect = jsrt::object::create();

		//Play method for Sound Effect JS object
		auto Play = [](_In_ JsValueRef callee, _In_ bool isConstructCall, _In_ JsValueRef *arguments, _In_ unsigned short argumentCount, _In_opt_ void *callbackState)->JsValueRef
		{
			auto soundEffect = reinterpret_cast<SoundEffect*>(callbackState);
			soundEffect->Play();

			return 0; //jsrt::context::undefined;
		};

		//auto jsPlay = jsrt::function<void>::create(Play);
		//jsSoundEffect.set_property(jsrt::property_id::create(L"Play"), jsPlay);
		JsValueRef func;
		JsCreateFunction(Play, soundEffect, &func);
		JsSetProperty(jsSoundEffect.handle(), jsrt::property_id::create(L"Play").handle(), func, true);

		return jsSoundEffect.handle();
	};

	JsValueRef func;
	JsCreateFunction(CreateSoundEffect, this, &func);
	JsSetProperty(jsAudioEngine.handle(), jsrt::property_id::create(L"CreateSoundEffect").handle(), func, true);

	//read java script text from a file
	wifstream in(scriptFile);
	assert(in);
	wstring script((istreambuf_iterator<wchar_t>(in)), istreambuf_iterator<wchar_t>());
	
	try
	{
		//funCallback = make_shared<std::function<void(jsrt::function<void >(std::bind(PromiseCallback, this));
		//jsrt::context::set_promise_continuation_callback(funCallback);
		JsErrorCode code= JsSetPromiseContinuationCallback(PromiseContinuationCallback, &callback);

		jsrt::context::run(script.c_str());
	}
	catch (jsrt::script_exception & e)
	{
		jsrt::error t(e.error());

		Log().Get() << L"runtime exception : " << t.message();		
	}
	catch (jsrt::script_compile_exception & e)
	{		
		Log().Get() << L"compile exception : "<< e.error().message() << L", at line " << int(e.error().line()) << L" colume " << e.error().column();
	}
	catch (jsrt::exception & e)
	{
		string name = typeid(e).name();

		Log().Get()  <<  L"unknown exception : "<< wstring(begin(name), end(name));
	}
	
}
//
//	Initialize Bullet Physics Engine
//
void Sample3DSceneRenderer::InitializePhysicsEngine()
{
	BulletPhysicsEngine::Initialize();		
	
	//create a static rigid bodies as ground
	{
		
		auto groundShape = make_shared<btBoxShape>(btVector3(btScalar(50.), btScalar(50.), btScalar(50.)));

		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin(btVector3(0, -50, 0));

		btScalar mass(0.);
		auto body = CreateRigidBody(mass, groundTransform, groundShape, btVector4(0, 0, 1, 1));
	}
	
	//create a few basic rigid bodies
	{		
		auto sphereShape = make_shared<btSphereShape>(btScalar(1.));

		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin(btVector3(0, 10, 0));

		btScalar mass(1.);
		auto body = CreateRigidBody(mass, groundTransform, sphereShape, btVector4(0, 0, 1, 1));
	}
	

}

std::shared_ptr<DirectX::Model> Sample3DSceneRenderer::LoadModel(const std::wstring & fileName)
{	
	wstring extName;
	size_t pos = fileName.rfind(L'.');
	
	//get file extention name
	if (pos != string::npos)
		extName.append(begin(fileName) + pos, end(fileName));
	
	auto device = m_deviceResources->GetD3DDevice();

	auto iter = m_ModelMap.find(fileName);
	if (iter != end(m_ModelMap))
		return iter->second;
	else if(_wcsicmp(extName.c_str(), L".sdkmesh") == 0)
	{		
		size_t pos = fileName.rfind(L'\\');
		if (pos != string::npos)
		{
			EffectFactory* pEffectFactory = static_cast<EffectFactory*>(m_fxFactory.get());
			wstring path(fileName, 0, pos);

			pEffectFactory->SetDirectory(path.c_str());
		}
		
		shared_ptr<DirectX::Model> model = move(Model::CreateFromSDKMESH(device, fileName.c_str(), *m_fxFactory, true));
		assert(model);

		wstring anim = fileName + L"_anim";
		HANDLE handle = CreateFile2(anim.c_str(), GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, nullptr);
		if (handle != INVALID_HANDLE_VALUE)
		{
			// The animation file exsits
			CloseHandle(handle);
			handle = INVALID_HANDLE_VALUE;

			model->LoadAnimation(anim.c_str());
			model->TransformBindPose(XMMatrixIdentity());
		}
		
		m_ModelMap[fileName] = model;

		return model;
	} 
	else if (_wcsicmp(extName.c_str(), L".cmo") == 0)
	{
		shared_ptr<DirectX::Model> model = Model::CreateFromCMO(device, fileName.c_str(), *m_dgslFactory);

		//auto actor = make_shared<CActor>();
		//actor->Set(move(model), XMVectorSet(0.02f, 0.02f, -0.02f, 0.f), XMVectorSet(XM_PIDIV2, 0.f, 0.f, 0.f), XMVectorSet(3.f, 0.f, 0.f, 0.f));
		//actor->CreateShapeforBoundingVolume(m_deviceResources->GetD3DDeviceContext());
		//actor->SetAnimation(L"Unreal Take");

		m_ModelMap[fileName] = model;

		//model->TransformBindPose(XMMatrixIdentity());

		return model;
	}

	return nullptr;
}

void Sample3DSceneRenderer::ReleaseDeviceDependentResources()
{
	m_loadingComplete = false;
	m_vertexShader.Reset();
	m_inputLayout.Reset();
	m_pixelShader.Reset();
	m_constantBuffer.Reset();
	m_vertexBuffer.Reset();
	m_indexBuffer.Reset();
	m_shape.reset();
}

void XM_CALLCONV Sample3DSceneRenderer::DrawGrid(FXMVECTOR xAxis, FXMVECTOR yAxis, FXMVECTOR origin, size_t xdivs, size_t ydivs, GXMVECTOR color)
{
	auto context = m_deviceResources->GetD3DDeviceContext();
	m_batchEffect->SetTextureEnabled(false);
	m_batchEffect->SetVertexColorEnabled(true);

	m_batchEffect->Apply(context);

	context->IASetInputLayout(m_batchInputLayout.Get());

	m_batch->Begin();

	xdivs = std::max<size_t>(1, xdivs);
	ydivs = std::max<size_t>(1, ydivs);

	for (size_t i = 0; i <= xdivs; ++i)
	{
		float fPercent = float(i) / float(xdivs);
		fPercent = (fPercent * 2.0f) - 1.0f;
		XMVECTOR vScale = XMVectorScale(xAxis, fPercent);
		vScale = XMVectorAdd(vScale, origin);

		DirectX::VertexPositionColor v1(XMVectorSubtract(vScale, yAxis), color);
		DirectX::VertexPositionColor v2(XMVectorAdd(vScale, yAxis), color);
		m_batch->DrawLine(v1, v2);
	}

	for (size_t i = 0; i <= ydivs; i++)
	{
		FLOAT fPercent = float(i) / float(ydivs);
		fPercent = (fPercent * 2.0f) - 1.0f;
		XMVECTOR vScale = XMVectorScale(yAxis, fPercent);
		vScale = XMVectorAdd(vScale, origin);

		DirectX::VertexPositionColor v1(XMVectorSubtract(vScale, xAxis), color);
		DirectX::VertexPositionColor v2(XMVectorAdd(vScale, xAxis), color);
		m_batch->DrawLine(v1, v2);
	}

	m_batch->End();
}

//
// Draw sence except the floor to shadow map
//
void XM_CALLCONV Sample3DSceneRenderer::DrawSenceToShadowMap(FXMMATRIX view, FXMMATRIX projection)
{
	// Draw all models to shadow map
	auto context = m_deviceResources->GetD3DDeviceContext();
		
	m_shadowMap->BindRenderTarget(context);

	auto update = [&](IEffect* pEffect)
	{
		IEffectShadowMap* pEffectShadowMap = dynamic_cast<IEffectShadowMap*>(pEffect);
		if (pEffectShadowMap)
			pEffectShadowMap->SetRenderingShadowMap(true);

		DGSLEffect * pDgslEffect = dynamic_cast<DGSLEffect*>(pEffect);
		if (pDgslEffect)
		{
			pDgslEffect->SetTextureEnabled(false);				
			pDgslEffect->SetTexture2(nullptr);
		}
	};	

	for (auto & actor : m_actors)
	{
		actor->Render(context, *m_states, view, projection, false);
	}

	for (auto & actor : mCharacters)
	{
		actor->UpdateEffects(update);
		actor->Render(context, *m_states, view, projection, false);
	}

	m_shadowMap->UnbindRenderTarget(context);
}

void XM_CALLCONV Sample3DSceneRenderer::DrawSkeleton(DirectX::Model * pModel, DirectX::FXMMATRIX world, DirectX::FXMMATRIX view, DirectX::FXMMATRIX project)
{
	auto device = m_deviceResources->GetD3DDevice();
	auto deviceContext = m_deviceResources->GetD3DDeviceContext();

	CommonStates states(device);
	deviceContext->OMSetBlendState(states.AlphaBlend(), nullptr, 0xFFFFFFFF);
	deviceContext->OMSetDepthStencilState(states.DepthNone(), 0);
	//deviceContext->RSSetState(states.CullCounterClockwise());

	m_batchEffect->SetWorld(world);
	m_batchEffect->SetView(view);
	m_batchEffect->SetProjection(project);
	m_batchEffect->SetAlpha(1);

	m_batchEffect->Apply(deviceContext);
	deviceContext->IASetInputLayout(m_batchInputLayout.Get());

	DirectX::VertexPositionColor v1, v2;
	
	v1.position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	v1.color = XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f);
	v2.position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	v2.color = XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f);

	XMVECTOR v = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

	m_batch->Begin();
	for (auto frame : pModel->frames)
	{	
		UINT parentIndex = frame->ParentFrame;
		if (parentIndex != uint32_t(-1))
		{
			XMMATRIX parentMat = XMLoadFloat4x4(&pModel->frames[parentIndex]->mTransformedPoseFrameMatrix);	
			XMMATRIX bindMat = XMLoadFloat4x4(&pModel->frames[parentIndex]->mBindPoseFrameMatrix);

			XMStoreFloat3(&v1.position, XMVector3Transform(v, bindMat*parentMat));

			XMMATRIX mat = XMLoadFloat4x4(&frame->mTransformedPoseFrameMatrix);
			bindMat = XMLoadFloat4x4(&frame->mBindPoseFrameMatrix);

			XMStoreFloat3(&v2.position, XMVector3Transform(v, bindMat * mat));

#ifdef _DEBUG
			XMVECTOR scale, rot, trans;
			XMMatrixDecompose(&scale, &rot, &trans, bindMat);
#endif 

			m_batch->DrawLine(v1, v2);
		}
	}
	m_batch->End();
	
}