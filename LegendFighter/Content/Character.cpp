#include "pch.h"
#include "Character.h"

using namespace std;
using namespace DirectX;
using namespace LegendFighter;

Character::Character(const std::wstring & name) : nameN(name)
{
	Reset();
}

Character::Character( const wchar_t* name) : nameN(name)
{
	Reset();
}

Character::~Character()
{
}

const jsrt::object & Character::GetFSM()
{ 
	return m_spStateMachine->JsObject(); 
}

void Character::Reset()
{
	m_spStateMachine = std::make_shared<ScriptedStateMachine<Character>>(this);

	m_Mass = 70;
	ZeroMemory(&m_Force, sizeof(m_Force));
	ZeroMemory(&m_Velocity, sizeof(m_Velocity));
	ZeroMemory(&m_MaxSpeed, sizeof(m_MaxSpeed));	
	ZeroMemory(&m_targetDir, sizeof(m_targetDir));
	ZeroMemory(&m_preTargetPos, sizeof(m_preTargetPos));	

	m_MaxSpeed = 5.0;
}

void Character::Update(double totalTime, double elapsedTime)
{
	CActor::Update(totalTime, elapsedTime);	//update animation

	Move((float)elapsedTime);
	
	m_spStateMachine->Update(totalTime, elapsedTime);	

	return;
}

jsrt::object Character::JsRegister(jsrt::object parent)
{
	m_obj = jsrt::object::create();
	
	/*auto callback = jsrt::function<double, double, double>::create(Character::update);
	m_obj.set_property(jsrt::property_id::create(L"update"), callback);*/
	JsValueRef jsvrFun = nullptr;
	jsrt::object obj;
	UINT iRefCount = 0;
	JsErrorCode errorCode = JsNoError;

	errorCode = JsCreateFunction(SetVelocityWrap, this, &jsvrFun);
	JsSetProperty(m_obj.handle(), jsrt::property_id::create(L"SetVelocity").handle(), jsvrFun, true);
	JsRelease(jsvrFun, &iRefCount);

	errorCode = JsCreateFunction(TurnWrap, this, &jsvrFun);
	JsSetProperty(m_obj.handle(), jsrt::property_id::create(L"Turn").handle(), jsvrFun, true);
	JsRelease(jsvrFun, &iRefCount);

	errorCode = JsCreateFunction(GetFSMWrap, this, &jsvrFun);
	JsSetProperty(m_obj.handle(), jsrt::property_id::create(L"GetFSM").handle(), jsvrFun, true);
	JsRelease(jsvrFun, &iRefCount);

	errorCode = JsCreateFunction(IsFatiguedWrapper, this, &jsvrFun);
	JsSetProperty(m_obj.handle(), jsrt::property_id::create(L"Fatigued").handle(), jsvrFun, true);
	JsRelease(jsvrFun, &iRefCount);

	//register function SetScale
	errorCode = JsCreateFunction(SetScaleWrapper, this, &jsvrFun);
	errorCode = JsSetProperty(m_obj.handle(), jsrt::property_id::create(L"SetScale").handle(), jsvrFun, true);
	JsRelease(jsvrFun, &iRefCount);

	//register method SetRotation
	errorCode = JsCreateFunction(SetRotationWrapper, this, &jsvrFun);
	errorCode = JsSetProperty(m_obj.handle(), jsrt::property_id::create(L"SetRotation").handle(), jsvrFun, true);
	JsRelease(jsvrFun, &iRefCount);

	//register method SetTranslation
	errorCode = JsCreateFunction(SetPositionWrapper, this, &jsvrFun);
	errorCode = JsSetProperty(m_obj.handle(), jsrt::property_id::create(L"SetPosition").handle(), jsvrFun, true);
	JsRelease(jsvrFun, &iRefCount);


	//register method SetTranslation
	errorCode = JsCreateFunction(SetTargetPosWrapper, this, &jsvrFun);
	errorCode = JsSetProperty(m_obj.handle(), jsrt::property_id::create(L"SetTargetPos").handle(), jsvrFun, true);
	JsRelease(jsvrFun, &iRefCount);

	errorCode = JsCreateFunction(SetAnimationWrapper, this, &jsvrFun);
	errorCode = JsSetProperty(m_obj.handle(), jsrt::property_id::create(L"SetAnimation").handle(), jsvrFun, true);
	JsRelease(jsvrFun, &iRefCount);

	//register method CreateAnimationWrapper
	errorCode = JsCreateFunction(CreateAnimationWrapper, this, &jsvrFun);
	errorCode = JsSetProperty(m_obj.handle(), jsrt::property_id::create(L"CreateAnimation").handle(), jsvrFun, true);
	JsRelease(jsvrFun, &iRefCount);

	//register method CreateAnimationEventWrapper
	errorCode = JsCreateFunction(CreateAnimationEventWrapper, this, &jsvrFun);
	errorCode = JsSetProperty(m_obj.handle(), jsrt::property_id::create(L"CreateAnimationEvent").handle(), jsvrFun, true);
	JsRelease(jsvrFun, &iRefCount);

	m_spStateMachine->ResigterJsObject(m_obj);

	return m_obj;
}

void Character::SetTargetPos(float x, float y, float z)
{
	m_TargetPos.x = x;
	m_TargetPos.y = y;
	m_TargetPos.z = z;

	XMVECTOR targetDir = XMVectorSubtract(XMLoadFloat3(&m_TargetPos), XMLoadFloat3(&m_position));
	targetDir = XMVector3Normalize(targetDir);
	m_preTargetPos = m_position;
	XMStoreFloat3(&m_targetDir, targetDir);
	
	m_MaxSpeed = (float)m_obj.get_property<double>(jsrt::property_id::create(L"speed"));
	//XMVECTOR force = XMLoadFloat3(&m_Force);
	XMVECTOR desiredVelocity = targetDir * m_MaxSpeed;
	XMVECTOR velocity = XMLoadFloat3(&m_Velocity);

	//XMVECTOR force = (desiredVelocity - velocity) * m_Mass;
	XMVECTOR force = (desiredVelocity) * m_Mass;

	//XMVECTOR Radian = XMVectorACos(targetDir);		 

	//XMStoreFloat(&m_targetFacing, Radian);
	//bTurn = true;
	//if (m_targetDir.z > 0)
	//	m_targetFacing = m_targetFacing + XM_PI;

	//float dir = (float)m_obj.get_property<double>(jsrt::property_id::create(L"dir"));	// the initilization direction of model
	//float facing = m_rotation.y + dir;

	//if (facing < 0)
	//	facing += XM_2PI;

	//if (facing > XM_2PI)
	//	facing -= XM_2PI;

	//if (m_targetFacing - facing < XM_PI)
	//	bCW = true;
	//else
	//	bCW = false;

	//XMVECTOR force = XMVectorSet(1.0f, 0.f, -1.f, 0.f);
	//force = XMVector3Normalize(force) * m_Mass;
	XMStoreFloat3(&m_Force, force);
	
	DirectX::XMVECTOR radian = XMVectorACos(targetDir);

	if (XMVectorGetZ(targetDir) > 0.f)
		radian = DirectX::XMVectorReplicate(XM_2PI) - radian;

	float dir = (float)m_obj.get_property<double>(jsrt::property_id::create(L"dir"));
	bCW = XMVectorGetX(radian) - (m_rotation.y -dir) > XM_PI ? true : false;  //cw or ccw
}

XMVECTOR Character::Turncate(DirectX::FXMVECTOR value, float maxLength)
{
	
	XMVECTOR lengthSq = XMVector3LengthSq(value);

	if (XMVectorGetX(lengthSq) > maxLength)
	{
		XMVECTOR normalize = XMVector3Normalize(value);

		return normalize * maxLength;
	}
	else
		return value;
}

//void Character::Move1(float elapsedTime)
//{
//	XMVECTOR pos = XMLoadFloat3(&m_position);
//	XMVECTOR targetPos = XMLoadFloat3(&m_TargetPos);
//
//	float speed = (float)m_obj.get_property<double>(jsrt::property_id::create(L"speed"));
//	float dir = (float)m_obj.get_property<double>(jsrt::property_id::create(L"dir"));	// the initilization direction of model
//
//	//float x = cos(m_rotation.y + dir);
//	//float y = 0;
//	//float z = sin(m_rotation.y + dir);
//
//	XMVECTOR Sin, Cos;
//	XMVectorSinCos(&Sin, &Cos, XMVectorSet(m_rotation.y + dir, 0, m_rotation.y + dir, 0));
//
//	float x, y, z;
//	XMStoreFloat(&x, Cos);
//	y = 0.f;
//	XMStoreFloat(&z, Sin);
//
//	XMVECTOR targetDir = XMLoadFloat3(&m_targetDir);
//
//	if (bTurn)
//	//if(!XMVector3Equal(XMVectorSet(x, y, z, 0), targetDir))
//	{
//		Turn(elapsedTime);
//	}	
//	else if (XMVector3GreaterOrEqual(XMVector3Length(pos),  XMVector3Length(targetPos)) )
//	{
//		XMStoreFloat3(&m_position, targetPos);
//	}
//	else //move to 
//	{
//		//m_rotation.y = m_targetFacing;		
//
//		//XMVECTOR distance = XMVectorScale(XMVectorSet(x, y, z, 0), speed * elapsedTime);
//		XMVECTOR distance = XMVectorScale(XMLoadFloat3(&m_targetDir), speed * elapsedTime);
//
//		XMStoreFloat3(&m_position, XMVectorAdd(pos, distance));
//	}
//
//	//if (m_spCollision->IsOutOfBounds(this))
//	//{
//	//	Turn(elapsedTime);
//	//}
//}

void Character::Move(float elapsedTime)
{
	XMVECTOR force		= XMLoadFloat3(&m_Force);
	XMVECTOR velocity	= XMLoadFloat3(&m_Velocity);
	XMVECTOR position	= XMLoadFloat3(&m_position);

	XMVECTOR acceleration = force / m_Mass;

	velocity += acceleration * elapsedTime;
	
	velocity = Turncate(velocity, m_MaxSpeed);

	position += velocity * elapsedTime;

	XMStoreFloat3(&m_Velocity, velocity);
	XMStoreFloat3(&m_position, position);

	XMVECTOR facing = XMVector3Normalize(velocity);

	XMVECTOR radian = XMVectorACos(facing);
	/*
	if (XMVectorGetZ(facing) > 0.f)
		radian = XMVectorReplicate(XM_2PI) - radian;
	float dir = (float)m_obj.get_property<double>(jsrt::property_id::create(L"dir"));
	float speedTurn = (float)m_obj.get_property<double>(jsrt::property_id::create(L"speedTurn"));

	if (XMVectorGetX(XMVector3LengthSq(velocity)) > 0.00000001f && m_rotation.y != (XMVectorGetX(radian) - dir) )
	{		
		if (bCW)
		{
			m_rotation.y -= (speedTurn * elapsedTime);
			
			if (m_rotation.y < XMVectorGetX(radian) - dir)
				m_rotation.y = XMVectorGetX(radian) - dir;
		}
		else
		{
			m_rotation.y += (speedTurn * elapsedTime);
			
			if (m_rotation.y > XMVectorGetX(radian) - dir)
				m_rotation.y = XMVectorGetX(radian) - dir;
		}

		if (m_rotation.y < 0.f)
			m_rotation.y += XM_2PI;

		if (m_rotation.y > XM_2PI)
			m_rotation.y -= XM_2PI;		
	}
	*/
	XMVECTOR targetPos = XMLoadFloat3(&m_TargetPos);
	XMVECTOR preTargetPos = XMLoadFloat3(&m_preTargetPos);

	if (XMVectorGetX(XMVector3LengthSq(position - preTargetPos)) > XMVectorGetX(XMVector3LengthSq(targetPos - preTargetPos))  )
	{
		//XMStoreFloat3(&m_position, targetPos);
		//XMStoreFloat3(&m_Velocity, XMVectorSet(0, 0, 0, 0));
	}
	
}

void Character::Turn(float elapsedTime)
{
	using namespace DirectX;

	//XMVECTOR dir;
	float dir = (float)m_obj.get_property<double>(jsrt::property_id::create(L"dir"));
	float speedTurn = (float)m_obj.get_property<double>(jsrt::property_id::create(L"speedTurn"));
	XMVECTOR targetDir = XMLoadFloat3(&m_targetDir);
	float facing = m_rotation.y + dir;		//radian 
	
	//XMVECTOR Sin, Cos;
	//XMVectorSinCos(&Sin, &Cos, XMVectorSet(facing, 0, facing, 0));
	//XMVECTOR v1 = XMVectorSet(XMVectorGetX(Cos), 0, XMVectorGetX(Sin), 0);

	//XMVECTOR pos = XMLoadFloat3(&m_translation);
	//XMVECTOR targetPos = XMLoadFloat3(&m_TargetPos);
	//XMVECTOR v2 = XMVector3Normalize(targetPos - pos);
	//
	//XMVECTOR cross = XMVector3Cross(v1, v2);
	//cross = XMVector3Normalize(cross);
	//XMVECTOR dot = XMVector3Dot(v1, v2);

	if (facing < 0)
		facing += XM_2PI;
	
	if (facing > XM_2PI)
		facing -= XM_2PI;

	if (facing < m_targetFacing) //&& XMVectorGetY(cross) >= 0.f
	{
		m_rotation.y += speedTurn * elapsedTime;

		if (m_rotation.y + dir >= m_targetFacing )
		{
			m_rotation.y = m_targetFacing - dir;
			bTurn = false;
		}	
	}
	else
	{
		m_rotation.y -= speedTurn * elapsedTime;

		if (m_rotation.y + dir <= m_targetFacing )
		{
			m_rotation.y = m_targetFacing - dir;
			bTurn = false;
		}				
	}
/*
	if (m_rotation.y + dir - m_targetFacing < XM_PI)
	{	//cw
		if (m_rotation.y + dir >= m_targetFacing)
		{
			m_rotation.y = m_targetFacing - dir;
			bTurn = false;
		}
		else
			m_rotation.y += speedTurn * elapsedTime;
	}
	else
	{	//ccw
		if (m_rotation.y + dir <= m_targetFacing)
		{
			m_rotation.y = m_targetFacing - dir;
			bTurn = false;
		}
		else
			m_rotation.y -= speedTurn * elapsedTime;
	}
*/
	/*if (m_rotation.y > XM_2PI)
		m_rotation.y -= XM_2PI;

	if (m_rotation.y < -XM_2PI)
		m_rotation.y += XM_2PI;*/

	//XMVectorAdd()
	//XMVectorCos
	//XMStoreFloat3(&m_translation, XMVectorAdd(Pos, distance));
}

bool Character::Intersect(Character* pCharacter)
{
	bool ret = false;

	for (auto mesh : m_model->meshes)
	{
		for (auto newMesh : pCharacter->m_model->meshes)
		{
			ret = mesh->boundingBox.Intersects(newMesh->boundingBox);

			if (ret)
				return ret;
		}
	}

	return ret;
}


//////////////////////////////////////////////////////////////////////////////
JsValueRef CALLBACK  Character::SetTargetPosWrapper(JsValueRef callee, bool isConstructCall,
	JsValueRef *arguments, unsigned short argumentCount, void *callbackState)
{
	Character* pThis = static_cast<Character*>(callbackState);
	using namespace jsrt;

	assert(argumentCount >= 4);

	if (argumentCount >= 4)
	{
		double x, y, z;
		
		runtime::translate_error_code( marshal::to_native(arguments[1], &x) );
		runtime::translate_error_code( marshal::to_native(arguments[2], &y) );
		runtime::translate_error_code( marshal::to_native(arguments[3], &z) );

		pThis->SetTargetPos((float)x, (float)y, (float)z);
	}

	return JS_INVALID_REFERENCE;
}

JsValueRef CALLBACK Character::SetAnimationWrapper(JsValueRef callee, bool isConstructCall,
	JsValueRef *arguments, unsigned short argumentCount, void *callbackState)
{
	wstring name;
	jsrt::marshal::to_native(arguments[1], &name);

	Character * pThis = reinterpret_cast<Character*>(callbackState);
	if (pThis)
	{
		pThis->SetAnimation(name);
	}

	return JS_INVALID_REFERENCE;
}

//
//	CreateAnimationWrapper
//	Create an animation with name and begin time and end time 
//
JsValueRef CALLBACK Character::CreateAnimationWrapper(JsValueRef callee, bool isConstructCall,
	JsValueRef *arguments, unsigned short argumentCount, void *callbackState)
{
	if (argumentCount < 3)
		return  jsrt::boolean::false_value().handle();

	wstring baseAnim, newAnim;
	double  beginTime, endTime;
	jsrt::marshal::to_native(arguments[1], &baseAnim);
	jsrt::marshal::to_native(arguments[2], &newAnim);
	jsrt::marshal::to_native(arguments[3], &beginTime);
	jsrt::marshal::to_native(arguments[4], &endTime);

	Character * pThis = reinterpret_cast<Character*>(callbackState);
	if (pThis)
	{		
		auto anim = pThis->CreateAnimation(baseAnim.c_str(), newAnim.c_str(), beginTime, endTime);

		if (anim)
		{
			//auto jsAnimation = jsrt::object::create();
			//jsrt::context::global().set_property(jsrt::property_id::create(baseAnim), jsAnimation);

			//anim->m_jsObject = jsAnimation;
			//return jsAnimation.handle();
		}		
	}

	return jsrt::boolean::true_value().handle();
}

//
//	CreateAnimation
//
std::shared_ptr<DirectX::Animation> 
Character::CreateAnimation(const wchar_t * baseAnim, const wchar_t * name, double beginKeyFrame, double numberKeyFrame)
{	
	auto base = m_model->GetAnimation(baseAnim);

	auto anim = make_shared<Animation>();

	anim->numBeginKey = max((UINT)beginKeyFrame, 0);
	anim->numAnimKeys = min((UINT)numberKeyFrame, base->numAnimKeys - anim->numBeginKey);
	anim->animFPS = base->animFPS;
	anim->animFrames = base->animFrames;
	anim->frameTransformType = base->frameTransformType;

	m_model->InsertAnimation(name, anim);

	return anim;
}

bool Character::CreateAnimationEvent(const wchar_t * baseAnim, UINT keyFrame, Animation::AnimationEvent event)
{
	auto anim = m_model->GetAnimation(baseAnim);
	if (anim)
	{
		return anim->CreateAnimationEvent(keyFrame, event);
	}

	return false;
}

JsValueRef CALLBACK Character::CreateAnimationEventWrapper(JsValueRef callee, bool isConstructCall,
	JsValueRef *arguments, unsigned short argumentCount, void *callbackState)
{
	if (argumentCount < 4)
		return  jsrt::boolean::false_value().handle();

	wstring baseAnim;
	double  keyFrame;
	jsrt::value jsCallback(arguments[3]);

	jsrt::marshal::to_native(arguments[1], &baseAnim);
	jsrt::marshal::to_native(arguments[2], &keyFrame);

	Character * pThis = reinterpret_cast<Character*>(callbackState);
	if (pThis)
	{
		bool  ret = pThis->CreateAnimationEvent(baseAnim.c_str(), (UINT)keyFrame, [=](UINT keyFrame )
		{
			auto callback = (jsrt::function<void, double>)(jsCallback);
			callback(jsrt::context::undefined(), keyFrame);
		});
	}

	return  jsrt::boolean::true_value().handle();
}

//
//	Collision events
//
void Character::OnCollisionEnter(Character* pCharacter)
{

}

void Character::OnCollisionExcute(Character* pCharacter)
{

}

void Character::OnCollisionExit(Character* pCharacter)
{

}

//
//	Trigger events
//
void Character::OnTriggerEnter(Character* pCharacter)
{

}

void Character::OnTriggerExcute(Character* pCharacter)
{

}

void Character::OnTriggerExit(Character* pCharacter)
{

}