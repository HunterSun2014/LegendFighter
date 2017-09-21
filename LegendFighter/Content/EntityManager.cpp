#include "pch.h"
#include "EntityManager.h"
using namespace std;

EntityManager::EntityManager()
{
}

EntityManager::~EntityManager()
{
}

bool EntityManager::IsBlockByOtherEntity(Character* pCharacter)
{
	bool ret = false;

	for (auto entity : mCharacters)
	{
		pCharacter->Intersect( entity.get() );
	}

	return ret;
}

bool EntityManager::IsOutOfBounds(Character* pCharacter)
{
	bool ret = false;

	return ret;
}

Character * EntityManager::CreateActor(const std::wstring & name)
{
	using namespace std;

	shared_ptr<Character> character = make_shared<Character>(name);
	auto model = LoadModel(name);
	assert(model != nullptr);

	character->SetModel(model);
	character->CreateShapeforBoundingVolume(GetDeviceResources()->GetD3DDeviceContext());

	mCharacters.push_back(character);

	return character.get();
}

Character * EntityManager::FindActor(std::wstring name)
{
	using namespace std;
	auto iter = find_if(begin(mCharacters), end(mCharacters), [&name](shared_ptr<Character> pCharacter)
	{
		if (name == pCharacter->GetName())
			return true;
		else
			return false;
	});

	if (iter == end(mCharacters))
		return nullptr;
	else
		return (*iter).get();
}

void EntityManager::JsRegister(jsrt::object parent)
{
	//set object EntityManager
	jsrt::object objEntityManager = jsrt::object::create();
	parent.set_property(jsrt::property_id::create(L"EntityManager"), objEntityManager);

	//set method createActor of object EntityManager
	JsValueRef func;
	JsCreateFunction(CreateAcotrWrap, this, &func);
	JsSetProperty(objEntityManager.handle(), jsrt::property_id::create(L"CreateEntity").handle(), func, true);

	//set method findActor of object EntityManager
	JsValueRef funcFindActor;
	JsCreateFunction(FindActorWrap, this, &funcFindActor);
	JsSetProperty(objEntityManager.handle(), jsrt::property_id::create(L"FindEntity").handle(), funcFindActor, true);
}

void EntityManager::Update(double totalTime, double fTime)
{
	for (auto entity : mCharacters)
	{
		entity->Update(totalTime, fTime);

		/*for (auto entity1 : mCharacters)
		{
			if (entity == entity1)
				continue;

			for (auto mesh1 : entity->GetModel()->meshes)
			{
				for (auto mesh2 : entity1->GetModel()->meshes)
				{
					bool ret = mesh1->boundingBox.Intersects(mesh2->boundingBox);
					if (ret)
					{
						ret = false;
					}
				}
			}
		}
		*/		
	}
}

//wrapper createActor for Registering to JavaScript
JsValueRef CALLBACK EntityManager::CreateAcotrWrap(JsValueRef callee, bool isConstructCall,
	JsValueRef *arguments, unsigned short argumentCount, void *callbackState)
{
	JsValueRef ret = jsrt::context::null;

	wstring name;
	jsrt::marshal::to_native(arguments[1], &name);

	EntityManager * pThis = reinterpret_cast<EntityManager*>(callbackState);
	if (pThis)
	{
		Character* pChar = pThis->CreateActor(name);
				
		jsrt::object objParent;
		jsrt::marshal::to_native(arguments[0], &objParent);

		jsrt::object obj = pChar->JsRegister(objParent);

		return obj.handle();
	}

	return ret;
}

//wrapper FindActor for Registering to JavaScript
JsValueRef CALLBACK EntityManager::FindActorWrap(JsValueRef callee, bool isConstructCall,
	JsValueRef *arguments, unsigned short argumentCount, void *callbackState)
{
	const wchar_t * name;
	size_t length;

	JsStringToPointer(arguments[1], &name, &length);

	EntityManager * pThis = reinterpret_cast<EntityManager*>(callbackState);
	if (pThis)
	{
		auto entity = pThis->FindActor(name);
		if (entity != nullptr)
			return entity->JsObject().handle();
	}

	return jsrt::context::undefined;;
}

//wrapper SetBound for Registering to JavaScript
JsValueRef CALLBACK EntityManager::SetBoundWrap(JsValueRef callee, bool isConstructCall,
	JsValueRef *arguments, unsigned short argumentCount, void *callbackState)
{
	wstring name;// = jsrt::string(jsrt::value(arguments[1])).data();
	jsrt::marshal::to_native(arguments[1], &name);

	EntityManager * pThis = reinterpret_cast<EntityManager*>(callbackState);
	if (pThis)
	{
		auto entity = pThis->FindActor(name);
		if (entity != nullptr)
			return entity->JsObject().handle();
	}

	return jsrt::context::undefined;;
}
