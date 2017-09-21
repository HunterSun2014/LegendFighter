#pragma once
#include "..\Common\StepTimer.h"
//#include "CollisionHelper.h"
//#include "camera.h"

namespace LegendFighter
{
	//
	//	A 3D object in scence
	//
	class CActor
	{
	public:
		//CActor(_In_ ID3D11DeviceContext* deviceContext, std::shared_ptr<DirectX::Model> model, DirectX::FXMVECTOR scale, DirectX::FXMVECTOR rotation, DirectX::FXMVECTOR translation);
		CActor();
		CActor(const CActor &actor);
		virtual ~CActor();

		void Set(std::shared_ptr<DirectX::Model> model, DirectX::FXMVECTOR scale, DirectX::FXMVECTOR rotation, DirectX::FXMVECTOR translation);

		void SetModel(std::shared_ptr<DirectX::Model> model);
		void XM_CALLCONV SetScale(DirectX::FXMVECTOR scale);
		void XM_CALLCONV SetRotation(DirectX::FXMVECTOR rotation);
		void XM_CALLCONV SetPosition(DirectX::FXMVECTOR translation);

		DirectX::XMMATRIX GetWorldMartix();

		virtual void Update(double totalTime, double elapsedTime);

		void XM_CALLCONV Render(_In_ ID3D11DeviceContext* deviceContext, DirectX::CommonStates& states, DirectX::FXMMATRIX view, DirectX::CXMMATRIX projection,
			bool wireframe = false,
			_In_opt_ std::function<void __cdecl()> setCustomState = nullptr);
		
		typedef std::vector<std::shared_ptr<CActor>> Collection;

		void CreateShapeforBoundingVolume(_In_ ID3D11DeviceContext* deviceContext);

		void RenderBoundingVolume(DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection, DirectX::FXMVECTOR color);

		// Update all effects used by the model
		void __cdecl UpdateEffects( _In_ std::function<void __cdecl(DirectX::IEffect*)> setEffect);

		std::shared_ptr<DirectX::Model> GetModel() { return m_model;  }

		void SetAnimation(const std::wstring & animName); 

	protected:
		DirectX::XMFLOAT3 m_position;
		DirectX::XMFLOAT3 m_scale;
		DirectX::XMFLOAT3 m_rotation;
		std::shared_ptr<DirectX::Model> m_model;

		typedef std::shared_ptr<DirectX::GeometricPrimitive> BoundingVolumeShape;
		
		struct BoundingVolume
		{
			BoundingVolumeShape shape;
			DirectX::XMFLOAT3	center;
		};
		
		std::vector<BoundingVolume> m_BVShapes;	//Bounding Volume Shapes
		std::wstring animationName;

		//CActor(const CActor &actor) DIRECTX_CTOR_DELETE
		CActor & operator= (CActor const &);
	};

}