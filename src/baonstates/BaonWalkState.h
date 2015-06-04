/*
 * BaonWalkState.h
 *
 *  Created on: 26/05/2015
 *      Author: Pedro2
 */

#ifndef BAONWALKSTATE_H_
#define BAONWALKSTATE_H_

#include "BaonState.h"

class BaonWalkState : public BaonState{
public:
	BaonWalkState(bool flipped);
	void Update(float dt);
	void NotifyTileCollision(Body* previousBody, float dt);
	bool Is(std::string state);
};

#endif /* BAONWALKSTATE_H_ */