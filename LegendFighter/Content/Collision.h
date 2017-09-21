#pragma once

class Character;
class ICollision
{
public:
	//
	//	Collision events
	//
	virtual void OnCollisionEnter(Character* pCharacter) = 0;

	virtual void OnCollisionExcute(Character* pCharacter) = 0;
	
	virtual void OnCollisionExit(Character* pCharacter) = 0;

	//
	//	Trigger events
	//
	virtual void OnTriggerEnter(Character* pCharacter) = 0;

	virtual void OnTriggerExcute(Character* pCharacter) = 0;

	virtual void OnTriggerExit(Character* pCharacter) = 0;

};