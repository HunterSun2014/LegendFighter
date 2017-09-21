#pragma once
#include "actor.h"
#include "Character.h"
#include "..\Common\DeviceResources.h"

class EntityManager 
{
public:
	EntityManager();
	~EntityManager();

	virtual std::shared_ptr<DirectX::Model> LoadModel(const std::wstring & name) = 0;
	virtual std::shared_ptr<DX::DeviceResources> GetDeviceResources() = 0;

	Character * CreateActor(const std::wstring & name);

	Character * FindActor(std::wstring name);

	bool IsBlockByOtherEntity(Character* pCharacter);

	bool IsOutOfBounds(Character* pCharacter);

	void JsRegister(jsrt::object parent);

	void Update(double totalTime, double fTime);

private:
	//wrapper CreateActor for Registering to JavaScript
	static JsValueRef CALLBACK CreateAcotrWrap(JsValueRef callee, bool isConstructCall,
		JsValueRef *arguments, unsigned short argumentCount, void *callbackState);

	//wrapper FindActor for Registering to JavaScript
	static JsValueRef CALLBACK FindActorWrap(JsValueRef callee, bool isConstructCall,
		JsValueRef *arguments, unsigned short argumentCount, void *callbackState);

	//wrapper SetBound for Registering to JavaScript
	static JsValueRef CALLBACK SetBoundWrap(JsValueRef callee, bool isConstructCall,
		JsValueRef *arguments, unsigned short argumentCount, void *callbackState);

protected:
	std::vector<std::shared_ptr<Character>> mCharacters;

	EntityManager(const EntityManager & s)
	{

	}

	EntityManager& operator= (const EntityManager& right)
	{

	}
};