/*
 * Baon.cpp
 *
 *  Created on: 24/05/2015
 *      Author: Pedro2
 */

#include "Baon.h"

#include <stdio.h>
#include <cwchar>
#include <string>

#include "baonstates/BaonState.h"
#include "baonstates/BaonStateManager.h"
#include "Engine/Camera.h"
#include "Engine/Game.h"
#include "Engine/Geometry/Point.h"
#include "Engine/InputManager.h"
#include "Engine/Physics/Force.h"
#include "Engine/Physics/Physic.h"

int Baon::WALK_SPEED = 0;
int Baon::RUN_SPEED  = 0;
int Baon::JUMP_SPEED = 0;
float Baon::DOUBLECLICK_TIME = 0.2;
int Baon::MAX_HP = 30;

Baon::Baon(int playerScale, float mapMax, int posX) {
	LoadSpriteData();

	SetID(GameObject::BAON);

	scale = playerScale;
	Baon::WALK_SPEED = 50*scale;
	Baon::JUMP_SPEED = -230*scale;
	Baon::RUN_SPEED = 160*scale;
	flipped = false;
	fallUpdateCount = 2;
	isDamage = false;
	closeToEnemy = false;
	bendMode = false;
	takingDamage = false;

	// Cheats
	superJump = false;
	superSpeed = false;

	t = new Timer();
	stateManager = new BaonStateManager(this);
	runStates = NONE;

	sp = new Sprite("img/baon.png", 1, 0.1);
	sp->SetFrameHeight(spriteData[0]);
	sp->SetFrameWidth(spriteData[1]);
	sp->SetScaleX(playerScale);
	sp->SetScaleY(playerScale);

	if (posX != -1) {
		box.SetX(posX);
	} else {
		box.SetX(3 * Game::SCREEN_WIDTH/8);
	}
	box.SetY(200);
	box.SetH(sp->GetFrameHeight());
	box.SetW(sp->GetFrameWidth());

	limitX = mapMax - (box.GetW() * scale);
	cameraLimitX = mapMax - 5 * Game::SCREEN_WIDTH/8;

	b = new Body("baon", box.GetX(), box.GetY());
	b->ApplyForce(new Force("gravity", 0, 900));
	isDead = false;
	dying = false;

	bendHUD = new BendHUD(scale);

	land = new Sound("audio/sfx_char_landGrass.wav");
	jump = new Sound("audio/sfx_char_jumpGrass.wav");
	step1 = new Sound("audio/sfx_char_stepGrass1.wav");
	step2 = new Sound("audio/sfx_char_stepGrass2.wav");
	kicks = new Sound("audio/sfx_char_kick_swing1.wav");
	punchs = new Sound("audio/sfx_char_punch_swing1.wav");
	punchHit = new Sound("audio/sfx_char_punch_hit1.wav");
	rockHit = new Sound("audio/sfx_throwRock_hit.wav");
	hp = MAX_HP;

	trasp = new Sprite("img/bendmask.png", 1, 1);
	trasp->SetScaleX(scale);
	trasp->SetScaleY(scale);

	lifebar = new Lifebar(15 * scale, 215 * scale, scale, MAX_HP);
	levelWon = false;
	isFalling = false;
	isFinished = false;

	lastGivenAttack = BaonAttack::EMPTY;
	lastReceivedAttack = EnemyAttack::EMPTY;
}

Baon::~Baon() {
	delete lifebar;
	delete sp;
	delete t;
	delete stateManager;
	delete land;
	delete jump;
	delete step1;
	delete step2;
	delete kicks;
	delete punchs;
	delete punchHit;
	delete rockHit;
}

Sprite* Baon::GetSprite() {
	return sp;
}

bool Baon::IsCollisionFromRight() {
	return damageDirectionRight;
}

bool Baon::IsCloseToEnemy() {
	return closeToEnemy;
}

void Baon::SetCloseToEnemy(bool isClose) {
	closeToEnemy = isClose;
}

bool Baon::IsDying() {
	return dying;
}

void Baon::SetDying(bool dying) {
	this->dying = dying;
}

void Baon::SetDead(bool dead) {
	this->isDead = dead;
}

bool Baon::GetLevelWon() {
	return levelWon;
}

void Baon::RestoreLife() {
	hp = MAX_HP;
}

bool Baon::IsFalling() {
	return isFalling;
}

bool Baon::IsFlipped() {
	return flipped;
}

void Baon::SetJumpFrame() {
	sp->SetFrameHeight(spriteData[JUMP*3]);
	sp->SetFrameWidth(spriteData[JUMP*3 + 1]);
	sp->SetFrameCount(spriteData[JUMP*3 + 2]);
	sp->SetLine(JUMP, spriteData[0]);
}

void Baon::SetGroundTouchResolver(GroundTouchResolver* resolver) {
	this->resolver = resolver;
}

GroundTouchResolver* Baon::GetGroundTouchResolver() {
	return resolver;
}

Sound* Baon::GetStepSound(unsigned int stepNumber) {
	if (stepNumber == 1) {
		return step1;
	} else if (stepNumber == 2) {
		return step2;
	}
	return NULL;
}

void Baon::ResolveDeadReferences(int id) {
	stateManager->GetCurrentState()->ResolveDeadReferences(id);
}

bool Baon::IsState(std::string name) {
	return stateManager->GetCurrentState()->Is(name);
}

void Baon::RenderTransp() {
	if(GetBendMode()){
		trasp->SetFrameWidth(300);
		trasp->SetFrameHeight(240);
		trasp->Render(0, 0, 0, false);
	}
}

void Baon::SetTransitionFrame(bool flipped) {
	delete sp;
	sp = new Sprite("img/transition.png", 14, 0.1);
	sp->SetScaleX(scale);
	sp->SetScaleY(scale);
	this->flipped = flipped;
}

bool Baon::IsFinished() {
	return isFinished;
}

void Baon::LoadSpriteData() {
	FILE *fp = fopen("data/baon-data.txt", "r");
	fscanf(fp, "%d", &numEst);
	int val;
	while(!feof(fp)){
		fscanf(fp, "%d", &val);
		spriteData.push_back(val);
	}
}

void Baon::Update(float dt) {
	stateManager->Update(dt);
	if(!stateManager->GetCurrentState()->Is("JUMPING")
			&& !stateManager->GetCurrentState()->Is("FALLING")
			&& !stateManager->GetCurrentState()->Is("TAKEHIT")
			&& !stateManager->GetCurrentState()->Is("DYING")
			&& !stateManager->GetCurrentState()->Is("FASTPUNCH")
			&& !stateManager->GetCurrentState()->Is("ATTACK1")
			&& !stateManager->GetCurrentState()->Is("SPIKESTONE")) {

		if (stateManager->GetCurrentState()->Is("TRANSITION") && sp->GetCurrentFrame() >= 13) {
			isFinished = true;
			return;
		}

		sp->Update(dt);
	}

	float previousY = b->GetY();
	Physic::GetInstance()->UpdatePhysic(b, dt);
	float updatedY = b->GetY();

	isFalling = (updatedY - previousY > 2);

	box.SetX(b->GetX());
	box.SetY(b->GetY());

	bendHUD->SetPosX(box.GetX() - (int)(bendHUD->GetBox().GetW()/2) - box.GetW());
	bendHUD->SetPosY(box.GetY() - (int)(bendHUD->GetBox().GetH()/2));

	if(box.GetX() <= 3 * Game::SCREEN_WIDTH/8 || box.GetX() >= cameraLimitX){
		Camera::Unfollow();
		if (box.GetX() >= cameraLimitX) {
			levelWon = true;
		}
	} else if(box.GetX() > 3 * Game::SCREEN_WIDTH/8){
		Camera::Follow(this);
	}

	// Impede o player a sair do mapa pela esquerda
	if (box.GetX() < 0) {
		box.SetX(0);
		b->SetAccelX(0);
		b->SetVelX(0);
		b->SetX(0);
	}

	// Impede o player a sair do mapa pela direita
	if (box.GetX() > limitX) {
		box.SetX(limitX );
		b->SetAccelX(0);
		b->SetVelX(0);
		b->SetX(limitX);
	}

	if(hp <= 0 || box.GetY() > Game::SCREEN_HEIGHT) {
		dying = true;
	}

	// Cheats
	if (InputManager::GetInstance().KeyPress(O_KEY)) {
		SetSuperJump(!superJump);
	}
	if (InputManager::GetInstance().KeyPress(P_KEY)) {
		SetSuperSpeed(!superSpeed);
	}

}

void Baon::Render() {
	sp->Render(box.GetX() + Camera::pos.getX(), box.GetY() + Camera::pos.getY(), 0, stateManager->GetCurrentState()->IsFlipped());
	lifebar->Render(hp);
}

void Baon::NotifyCollision(GameObject* other) {
	if(other->GetID() == GameObject::PEDRA_BASICO_ENEMY){
		takingDamage = true;
		lastReceivedAttack = EnemyAttack::ROCK;
		if(other->GetBox().GetX() > box.GetX()){
			damageDirectionRight = true;
		}
		else{
			damageDirectionRight = false;
		}
	}
}

bool Baon::IsDead() {
	return isDead;
}

bool Baon::Is(std::string type) {
	return Being::Is(type) || type.compare("Baon") == 0;
}

//--------------------------------------------------------
//--------------------------------------------------------
//--------------------------------------------------------


//--------------------------------------------------------
//--------------------------------------------------------
void Baon::Run(bool flipped) {
	sp->SetFrameHeight(spriteData[RUN*3]);
	sp->SetFrameWidth(spriteData[RUN*3 + 1]);
	sp->SetFrameCount(spriteData[RUN*3 + 2]);
	sp->SetLine(RUN, spriteData[0]);
	if(!flipped){
		this->flipped = false;
		if (superSpeed) {
			b->SetVelX(5 * RUN_SPEED);
		} else {
			b->SetVelX(RUN_SPEED);
		}
	}
	else{
		this->flipped = true;
		if (superSpeed) {
			b->SetVelX(4 * -RUN_SPEED);
		} else {
			b->SetVelX(-RUN_SPEED);
		}
	}
}

void Baon::Walk(bool flipped) {
	sp->SetFrameHeight(spriteData[WALK*3]);
	sp->SetFrameWidth(spriteData[WALK*3 + 1]);
	sp->SetFrameCount(spriteData[WALK*3 + 2]);
	sp->SetLine(WALK, spriteData[0]);
	if(!flipped){
		this->flipped = false;
		b->SetVelX(WALK_SPEED);
	}
	else{
		this->flipped = true;
		b->SetVelX(-WALK_SPEED);
	}
	t->Restart();

	state = WALK;
}

void Baon::Stand(bool flipped) {
	sp->SetFrameHeight(spriteData[STAND*3]);
	sp->SetFrameWidth(spriteData[STAND*3 + 1]);
	sp->SetFrameCount(1);
	sp->SetLine(STAND, spriteData[0]);
	b->SetVelX(0);
	b->SetAccelX(0);
	step1->Stop();
}

void Baon::Jump(bool flipped) {

	if (superJump) {
		b->SetVelY(1.5 * JUMP_SPEED );
	} else {
		b->SetVelY(JUMP_SPEED);
	}
	step1->Stop();
	jump->Play(0);
}

void Baon::Land() {
	land->Play(0);
	TakeDamage(EnemyAttack::EMPTY, false, false);
}

void Baon::Fall() {
	if(fallUpdateCount < 5){
		sp->SetFrame(fallUpdateCount);
		fallUpdateCount++;
	}
}

void Baon::Punch(){
	sp->SetFrameHeight(spriteData[6*3]);
	sp->SetFrameWidth(spriteData[6*3 + 1]);
	sp->SetFrameCount(spriteData[6*3 + 2]);
	sp->SetLine(6, spriteData[0]);
	b->SetVelX(0);
	punchs->Play(0);
}

void Baon::Kick(){
	sp->SetFrameTime(0.06);
	sp->SetFrameHeight(spriteData[7*3]);
	sp->SetFrameWidth(spriteData[7*3 + 1]);
	sp->SetFrameCount(spriteData[7*3 + 2]);
	sp->SetLine(7, spriteData[0]);
	b->SetVelX(0);
	kicks->Play(0);
}

void Baon::TakeDamage(EnemyAttack attack, bool damage, bool isFromRight) {
	if (attack != EnemyAttack::EMPTY) {
		lastReceivedAttack = attack;
	}
	takingDamage = damage;
	damageDirectionRight = isFromRight;
}

bool Baon::isTakingDamage() {
	return takingDamage;
}


void Baon::TakeHit(bool flipped) {
	switch(lastReceivedAttack){
	case EnemyAttack::PUNCH:
		hp -= 2;
		punchHit->Play(0);
		break;
	case EnemyAttack::ROCK:
		hp -= 3;
		rockHit->Play(0);
		break;
	case EnemyAttack::SWORD:
		hp -= 2;
		// deveria ser swordhit
		punchHit->Play(0);
		break;
	case EnemyAttack::SHOULDER:
		hp -= 3;
		// deveria ser shoulderHit
		punchHit->Play(0);
		break;
	default:
		break;
	}
	sp->SetFrameHeight(spriteData[13*3]);
	sp->SetFrameWidth(spriteData[13*3 + 1]);
	sp->SetFrameCount(spriteData[13*3 + 2]);
	sp->SetLine(13, spriteData[0]);
	if(!damageDirectionRight){
		GetBody()->SetVelX(scale*1000);
		GetBody()->ApplyForce(new Force("resistance", -5000*scale, 0));
	}
	else{
		GetBody()->SetVelX(-1000*scale);
		GetBody()->ApplyForce(new Force("resistance", 5000*scale, 0));
	}
}

void Baon::MidAir(){

	if (b->GetForce("gravity") == NULL) {
		b->ApplyForce(new Force("gravity", 0, 1200));
	}
	if(InputManager::GetInstance().KeyPress(A_KEY)){
		if(stateManager->GetPreviousState()->Is("WALK")
				|| stateManager->GetPreviousState()->Is("STAND")){
			b->SetVelX(-WALK_SPEED);
			stateManager->GetCurrentState()->SetFlipped(true);
		}
		else{
			b->SetVelX(-RUN_SPEED);
			stateManager->GetCurrentState()->SetFlipped(true);
		}
	}
	else{
		if(InputManager::GetInstance().KeyPress(D_KEY)){
			if(stateManager->GetPreviousState()->Is("WALK")
					|| stateManager->GetPreviousState()->Is("STAND")){
				b->SetVelX(WALK_SPEED);
				stateManager->GetCurrentState()->SetFlipped(false);
			}
			else{
				b->SetVelX(RUN_SPEED);
				stateManager->GetCurrentState()->SetFlipped(false);
			}
		}
	}
	if(InputManager::GetInstance().KeyRelease(A_KEY)){
		b->SetVelX(0);
	}
	else{
		if(InputManager::GetInstance().KeyRelease(D_KEY)){
			b->SetVelX(0);
		}
	}
}

void Baon::NotifyTileCollision(Collision::CollisionAxis collisionAxis) {
	stateManager->GetCurrentState()->NotifyTileCollision();
}

BaonStateManager* Baon::GetState(){
	return stateManager;
}

// Cheats
void Baon::SetSuperJump(bool superJump) {
	this->superJump = superJump;
}

void Baon::SetSuperSpeed(bool superSpeed) {
	this->superSpeed = superSpeed;
}

bool Baon::GetSuperJump() {
	return superJump;
}

bool Baon::GetSuperSpeed() {
	return superSpeed;
}

bool Baon::GetBendMode() {
	return bendMode;
}

void Baon::SetBendMode(bool bendMode_) {
	this->bendMode = bendMode_;
}

void Baon::SetTouchingGround(bool isTouchingGround) {
	this->isTouchingGround = isTouchingGround;
}

bool Baon::GetTouchingGround() {
	return isTouchingGround;
}

BaonAttack Baon::GetLastGivenAttack() {
	return lastGivenAttack;
}

void Baon::SetLastGivenAttack(BaonAttack attack) {
	this->lastGivenAttack = attack;
}
