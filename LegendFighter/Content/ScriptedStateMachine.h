#pragma once
//
//	Scripted State machine via Java Script
//

template <class entity_type>
class ScriptedStateMachine
{

private:

	//pointer to the agent that owns this instance
	entity_type*      m_pOwner;

	//the current state 
	jsrt::object   m_jsCurrentState;

	//js object for self
	jsrt::object m_jsStateMachine;

public:

	ScriptedStateMachine(entity_type* owner) :m_pOwner(owner) {}

	//use these methods to initialize the FSM
	void SetCurrentState(const jsrt::object s) 
	{ 
		m_jsCurrentState = s; 

		//call the entry method of the new state
		//m_jsCurrentState["Enter"](m_pOwner);
		auto Enter = m_jsCurrentState.get_property<jsrt::function<void, jsrt::object>>(jsrt::property_id::create(L"Enter"));
		Enter(m_jsCurrentState, m_pOwner->JsObject());
	}

	static JsValueRef CALLBACK SetCurrentStateJsWrapper(JsValueRef callee, bool isConstructCall,
		JsValueRef *arguments, unsigned short argumentCount, void *callbackState)
	{
		ScriptedStateMachine<entity_type> * pThis = reinterpret_cast<ScriptedStateMachine<entity_type>*>(callbackState);

		jsrt::object obj;
		jsrt::marshal::to_native(arguments[1], &obj);
		pThis->SetCurrentState(obj);
		
		//return jsrt::context::null;
		return JS_INVALID_REFERENCE;
	}

	//call this to update the FSM
	void  Update(double totalTime, double elapsedTime)
	{
		//make sure the state is valid before calling its Execute 'method'
		if (m_jsCurrentState.is_valid())  //this could also be written as 'if(m_CurrentState)'
		{
			jsrt::property_id id = jsrt::property_id::create(L"Execute");
			//try
			{
				auto Execute = m_jsCurrentState.get_property<jsrt::function<void, jsrt::object, double, double>>(jsrt::property_id::create(L"Execute"));
				Execute(m_jsCurrentState, m_pOwner->JsObject(), totalTime, elapsedTime);
			}
			//catch (...)
			{

			}
		}
	}

	//change to a new state
	void  ChangeState(const jsrt::object& new_state)
	{
		//call the exit method of the existing state
		//m_jsCurrentState["Exit"](m_pOwner);
		auto Exit = m_jsCurrentState.get_property<jsrt::function<void, jsrt::object>>(jsrt::property_id::create(L"Exit"));
		Exit(m_jsCurrentState, m_pOwner->JsObject());

		//change state to the new state
		m_jsCurrentState = new_state;

		//call the entry method of the new state
		//m_jsCurrentState["Enter"](m_pOwner);
		auto Enter = m_jsCurrentState.get_property<jsrt::function<void, jsrt::object>>(jsrt::property_id::create(L"Enter"));
		Enter(m_jsCurrentState, m_pOwner->JsObject());
	}

	static JsValueRef CALLBACK ChangeStateJsWrapper(JsValueRef callee, bool isConstructCall,
		JsValueRef *arguments, unsigned short argumentCount, void *callbackState)
	{
		ScriptedStateMachine<entity_type> * pThis = reinterpret_cast<ScriptedStateMachine<entity_type>*>(callbackState);

		jsrt::object obj;
		jsrt::runtime::translate_error_code( jsrt::marshal::to_native(arguments[1], &obj) );
		
		pThis->ChangeState(obj);

		return JS_INVALID_REFERENCE;
	}

	void ResigterJsObject(const jsrt::object& parent)
	{
		m_jsStateMachine = jsrt::object::create();

		JsValueRef jsvrFun;
		jsrt::object obj;

		JsCreateFunction(SetCurrentStateJsWrapper, this, &jsvrFun);
		jsrt::marshal::to_native(jsvrFun, &obj);

		m_jsStateMachine.set_property(jsrt::property_id::create(L"SetCurrentState"), obj);
		//JsRelease(jsvrFun);


		JsCreateFunction(ChangeStateJsWrapper, this, &jsvrFun);
		jsrt::marshal::to_native(jsvrFun, &obj);

		m_jsStateMachine.set_property(jsrt::property_id::create(L"ChangeState"), obj);


	}

	//retrieve the current state
	const jsrt::object&  CurrentState()const { return m_jsCurrentState; }

	//retireve js object of state machine 
	const jsrt::object& JsObject() const { return m_jsStateMachine; }
};