/*
 * BaonKickState.cpp
 *
 *  Created on: 27/05/2015
 *      Author: Pedro2
 */

#include "BaonKickState.h"

#include "../Baon.h"
#include "../Engine/Timer.h"
#include "BaonStateManager.h"

BaonKickState::BaonKickState(bool flipped) {
	this->flipped = flipped;
	t = new Timer();
	t->Restart();
	nextRequested = false;
	executed = false;
	id = "KICK";
}

void BaonKickState::Update(float dt) {
	if(!executed){
		baon->Kick();
		executed = true;
	}

	if(t->Get() >= 4*0.1){
		nextRequested = true;
		sm->GetPreviousState()->Reset();
		next = sm->GetPreviousState()->GetID();
		nextFlipped = flipped;
	}
	else{
		t->Update(dt);
	}
}

void BaonKickState::NotifyTileCollision(Body* previousBody, float dt) {
}

bool BaonKickState::Is(std::string state) {
	return state.compare("KICKING") == 0;
}
