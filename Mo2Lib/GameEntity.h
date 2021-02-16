#pragma once

#include <assert.h>
#include <map>

class GameEntity
{
private:
	int ID = 0;

	void SetID()
	{
		static int next_validID = 0;
		assert(next_validID >= ID && "trying to set invalid ID");
		ID = next_validID;
		next_validID = ID + 1;
	}

public:

	GameEntity()
	{
		SetID();
	}
	GameEntity(int id)
	{
		ID = id;
	}

	int GetID() { return ID; }

	virtual ~GameEntity(){}

	virtual void Update(float elapsedtime) = 0;
	virtual void Draw() = 0;
};

