#include "pch.h"
#include "Actor.h"

using namespace DirectX;
using namespace LegendFighter;
using namespace std;

//CActor::CActor(
//	_In_ ID3D11DeviceContext* deviceContext,
//	std::shared_ptr<DirectX::Model> model, 
//	DirectX::FXMVECTOR scale, 
//	DirectX::FXMVECTOR rotation, 
//	DirectX::FXMVECTOR translation)
CActor::CActor()
{
	//Set(deviceContext, model, scale, rotation, translation);
}

CActor::CActor(const CActor &actor)
{
	Set(actor.m_model, XMLoadFloat3(&actor.m_scale), XMLoadFloat3(&actor.m_rotation), XMLoadFloat3(&actor.m_position));
}

CActor::~CActor()
{
}


void CActor::Set(
	std::shared_ptr<DirectX::Model> model, 
	DirectX::FXMVECTOR scale, 
	DirectX::FXMVECTOR rotation, 
	DirectX::FXMVECTOR translation)
{
	m_model = model;
	XMStoreFloat3(&m_scale, scale);
	XMStoreFloat3(&m_rotation, rotation);
	XMStoreFloat3(&m_position, translation);
}

void CActor::SetModel(std::shared_ptr<DirectX::Model> model)
{
	m_model = model;
}

void CActor::SetScale(DirectX::FXMVECTOR scale)
{
	XMStoreFloat3(&m_scale, scale);
}

void CActor::SetRotation(DirectX::FXMVECTOR rotation)
{
	XMStoreFloat3(&m_rotation, rotation);
}

void CActor::SetPosition(DirectX::FXMVECTOR translation)
{
	XMStoreFloat3(&m_position, translation);
}

void CActor::CreateShapeforBoundingVolume(_In_ ID3D11DeviceContext* deviceContext)
{

	for (auto mesh : m_model->meshes)
	{
		XMFLOAT3 size;
		XMStoreFloat3(&size, XMLoadFloat3(&mesh->boundingBox.Extents) * 2);
		
		unique_ptr<GeometricPrimitive> box = GeometricPrimitive::CreateBox(deviceContext, size);
		
		BoundingVolume bv;
		
		bv.shape = move(box);
		bv.center = mesh->boundingBox.Center;
		
		m_BVShapes.push_back(bv);
	}
}

void CActor::RenderBoundingVolume(CXMMATRIX view, CXMMATRIX projection, FXMVECTOR color)
{
	for (auto & BVShape : m_BVShapes)
	{
		XMMATRIX translation = XMMatrixTranslationFromVector(XMLoadFloat3(&BVShape.center));
		//XMMATRIX translation = XMMatrixTranslation(BVShape.center.x, BVShape.center.y, BVShape.center.z);

		BVShape.shape->Draw(translation*GetWorldMartix(), view, projection, color, false, true);
	}
}

XMMATRIX CActor::GetWorldMartix()
{
	XMVECTOR qid = XMQuaternionIdentity();
	const XMVECTOR scale = XMLoadFloat3(&m_scale);
	const XMVECTOR translate = XMLoadFloat3(&m_position);
	XMVECTOR rotate = XMQuaternionRotationRollPitchYaw(m_rotation.x, m_rotation.y, m_rotation.z);

	return XMMatrixTransformation(g_XMZero, qid, scale, g_XMZero, rotate, translate);
}

void CActor::SetAnimation(const std::wstring & animName)
{
	animationName = animName;
	
	auto anim = m_model->GetAnimation(animationName.c_str());
	anim->SetTotalTime(0);
}

void CActor::Update(double totalTime, double elapsedTime)
{
	//for the model loaded sdkmesh
	m_model->TransformMesh(animationName, elapsedTime);

	//for the model loaded CMO
	m_model->UpdateAnimation(elapsedTime, animationName);
}

void XM_CALLCONV CActor::Render(_In_ ID3D11DeviceContext* deviceContext, CommonStates& states, FXMMATRIX view, CXMMATRIX projection,
	bool wireframe,
	_In_opt_ std::function<void __cdecl()> setCustomState)
{
	m_model->Draw(deviceContext, states, GetWorldMartix(), view, projection, wireframe, setCustomState);
}

// Update all effects used by the model
void __cdecl CActor::UpdateEffects(_In_ std::function<void (IEffect*)> setEffect)
{
	m_model->UpdateEffects(setEffect);
}