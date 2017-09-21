#pragma once
#include "Actor.h"
#include "Collision.h"
#include "ScriptedStateMachine.h"

class Character :
	public LegendFighter::CActor,
	public ICollision
{
public:
	Character( const std::wstring & name);
	Character(const wchar_t* name);
	~Character();

	void Reset();

	jsrt::object JsRegister(jsrt::object parent);
	
	virtual void Update(double totalTime, double elapsedTime);
	
	void SetVelocity(float x, float y, float z) 
	{ 
		m_Velocity = DirectX::XMFLOAT3(x, y, z); 
	}
		
	void Turn(float elapsedTime);

	bool Intersect(Character* pCharacter);

	void SetTargetPos(float x, float y, float z);

	//Split an animation to a new shorter animation
	std::shared_ptr<DirectX::Animation> CreateAnimation(const wchar_t * baseAnim, const wchar_t * name, double beginTime, double endTime);

	bool CreateAnimationEvent(const wchar_t * baseAnim, UINT keyFrame, DirectX::Animation::AnimationEvent event);

	std::wstring  GetName()
	{
		return nameN;
	}

	bool IsFatigued()
	{
		return true;
	}

	//ScriptedStateMachine<Character>* GetFSM() { return m_spStateMachine.get(); }

	const jsrt::object & GetFSM();
	const jsrt::object & JsObject() { return m_obj; }
		
	//
	//	Collision events
	//
	virtual void OnCollisionEnter(Character* pCharacter) ;

	virtual void OnCollisionExcute(Character* pCharacter);

	virtual void OnCollisionExit(Character* pCharacter);

	//
	//	Trigger events
	//
	virtual void OnTriggerEnter(Character* pCharacter);

	virtual void OnTriggerExcute(Character* pCharacter);

	virtual void OnTriggerExit(Character* pCharacter);

protected:
	DirectX::XMVECTOR Turncate(DirectX::FXMVECTOR value,  float maxLength);

	void Move(float elapsedTime);
protected:
	//
	// the all functions named with postfix Wrap are registried and exposed to JavaScript engine
	//
	static JsValueRef CALLBACK SetVelocityWrap(JsValueRef callee, bool isConstructCall,
		JsValueRef *arguments, unsigned short argumentCount, void *callbackState)
	{
		Character* pThis = static_cast<Character*>(callbackState);

		double x, y, z;
		jsrt::marshal::to_native(arguments[1], &x); 
		jsrt::marshal::to_native(arguments[2], &y);
		jsrt::marshal::to_native(arguments[3], &z);

		pThis->SetVelocity((float)x, (float)y, (float)z);

		return JS_INVALID_REFERENCE;
	}

	static JsValueRef CALLBACK TurnWrap(JsValueRef callee, bool isConstructCall,
		JsValueRef *arguments, unsigned short argumentCount, void *callbackState)
	{
		Character* pThis = static_cast<Character*>(callbackState);

		double elapsedTime;
		jsrt::marshal::to_native(arguments[1], &elapsedTime); 

		pThis->Turn((float)elapsedTime);

		return JS_INVALID_REFERENCE;
	}

	static JsValueRef CALLBACK GetFSMWrap(JsValueRef callee, bool isConstructCall,
		JsValueRef *arguments, unsigned short argumentCount, void *callbackState)
	{
		JsValueRef ret;
		JsDoubleToNumber(0, &ret);

		Character* pThis = static_cast<Character*>(callbackState);

		return pThis->GetFSM().handle();
	}

	static JsValueRef CALLBACK IsFatiguedWrapper(JsValueRef callee, bool isConstructCall,
		JsValueRef *arguments, unsigned short argumentCount, void *callbackState)
	{
		Character* pThis = static_cast<Character*>(callbackState);

		jsrt::boolean ret = pThis->IsFatigued() ? jsrt::boolean::true_value() : jsrt::boolean::false_value();

		return ret.handle();
	}

	static JsValueRef CALLBACK SetModelWrapper(JsValueRef callee, bool isConstructCall,
		JsValueRef *arguments, unsigned short argumentCount, void *callbackState)
	{
		Character* pThis = static_cast<Character*>(callbackState);

		assert(argumentCount >= 2);

		if (argumentCount >= 2)
		{
			//wstring name = jsrt::string(jsrt::value(arguments[1])).data;

			//pThis->SetModel(name);
		}

		return JS_INVALID_REFERENCE; // jsrt::context::null
	}
	
	static JsValueRef CALLBACK SetScaleWrapper(JsValueRef callee, bool isConstructCall,
		JsValueRef *arguments, unsigned short argumentCount, void *callbackState)
	{
		Character* pThis = static_cast<Character*>(callbackState);
				
		assert(argumentCount >= 4);

		if (argumentCount >=4 )
		{
			double x, y, z;

			jsrt::marshal::to_native(arguments[1], &x);
			jsrt::marshal::to_native(arguments[2], &y);
			jsrt::marshal::to_native(arguments[3], &z);

			pThis->SetScale(DirectX::XMVectorSet((float)x, (float)y, (float)z, 0.f));
		}
		
		return JS_INVALID_REFERENCE; // jsrt::context::null
	}

	static JsValueRef CALLBACK SetRotationWrapper(JsValueRef callee, bool isConstructCall,
		JsValueRef *arguments, unsigned short argumentCount, void *callbackState)
	{
		Character* pThis = static_cast<Character*>(callbackState);

		assert(argumentCount >= 4);

		if (argumentCount >= 4)
		{
			double x, y, z;

			jsrt::marshal::to_native(arguments[1], &x);
			jsrt::marshal::to_native(arguments[2], &y);
			jsrt::marshal::to_native(arguments[3], &z);

			pThis->SetRotation(DirectX::XMVectorSet((float)x, (float)y, (float)z, 0.f));
		}

		return JS_INVALID_REFERENCE; // jsrt::context::null
	}

	static JsValueRef CALLBACK SetPositionWrapper(JsValueRef callee, bool isConstructCall,
		JsValueRef *arguments, unsigned short argumentCount, void *callbackState)
	{
		Character* pThis = static_cast<Character*>(callbackState);

		assert(argumentCount >= 4);

		if (argumentCount >= 4)
		{
			double x, y, z;

			jsrt::marshal::to_native(arguments[1], &x);
			jsrt::marshal::to_native(arguments[2], &y);
			jsrt::marshal::to_native(arguments[3], &z);

			pThis->SetPosition(DirectX::XMVectorSet((float)x, (float)y, (float)z, 0.f));
		}

		return JS_INVALID_REFERENCE; // jsrt::context::null
	}

	static JsValueRef CALLBACK SetTargetPosWrapper(JsValueRef callee, bool isConstructCall,
		JsValueRef *arguments, unsigned short argumentCount, void *callbackState);

	static  JsValueRef CALLBACK SetAnimationWrapper(JsValueRef callee, bool isConstructCall,
		JsValueRef *arguments, unsigned short argumentCount, void *callbackState);

	static  JsValueRef CALLBACK CreateAnimationWrapper(JsValueRef callee, bool isConstructCall,
		JsValueRef *arguments, unsigned short argumentCount, void *callbackState);

	static  JsValueRef CALLBACK CreateAnimationEventWrapper(JsValueRef callee, bool isConstructCall,
		JsValueRef *arguments, unsigned short argumentCount, void *callbackState);

private:
	jsrt::object m_obj;

	double velocity;
	double face;

	DirectX::XMFLOAT3	m_TargetPos;
	DirectX::XMFLOAT3 m_preTargetPos;
	DirectX::XMFLOAT3	m_targetDir;	//radian
	float							m_targetFacing;
	bool							bTurn;
	bool							bCW;	//cw or cww for turn

	DirectX::XMFLOAT3	m_Velocity;
	DirectX::XMFLOAT3	m_Force;
	float				m_Mass;	
	float				m_MaxSpeed;
	float				m_MaxForce;
	float				m_MaxTurnRate;
	
	std::wstring nameN;

	std::shared_ptr<ScriptedStateMachine<Character>>  m_spStateMachine;
};