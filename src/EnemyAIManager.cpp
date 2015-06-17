#include "EnemyAIManager.h"
#include "Engine/Collision.h"
#include <iostream>

EnemyAIManager::EnemyAIManager(Baon* baon_, Enemy* enemy_){
	this->enemy = enemy_;
	this->baon = baon_;
}

EnemyAIManager::~EnemyAIManager(){
	
}

void EnemyAIManager::update(const float dt){
	if(!baon->IsDead()){
		if(abs(enemy->GetBody()->GetX() - baon->GetBody()->GetX()) < 50){
			if(!enemy->IsState(Enemy::enemyStates::PUNCH)){
				enemy->changeState(Enemy::enemyStates::PUNCH);
			}
		}
	}
	if(enemy->IsState(Enemy::enemyStates::PUNCH)){
		if(enemy->StateEnd()){
			enemy->changeState(Enemy::enemyStates::PATROLLING);
		}
	}
	if(enemy->IsState(Enemy::enemyStates::FOLLOW)){
		int baonX = baon->GetBody()->GetX();
		int enemyX = enemy->GetBody()->GetX();

		// Esse 30 tem que tirar depois, e so pra nao ficar trocando entre os ifs.
		if(baonX - 30 > enemyX){
			enemy->Run(false);
		}
		else if(baonX < enemyX){
			enemy->Run(true);	
		}
	}

	if(!enemy->IsDead() && enemy->IsTakingDamage()){
		enemy->TakeDamage(true);
	}
	if(!baon->IsDead()){

		Rect baonRect, enemyRect;

		baonRect.SetX(baon->GetBody()->GetX());
		baonRect.SetY(baon->GetBody()->GetY());
		baonRect.SetW(30*baon->GetScale());
		baonRect.SetH(50*baon->GetScale());

		enemyRect.SetX(enemy->GetBody()->GetX());
		enemyRect.SetY(enemy->GetBody()->GetY());
		enemyRect.SetW(30*enemy->GetScale());
		enemyRect.SetH(50*enemy->GetScale());

		if(Collision::IsColliding(baonRect, enemyRect, 0, 0)){
			bool right;
			if(enemyRect.GetX() > baonRect.GetX()){
				right = true;
			}else{
				right = false;
			}

			if(!enemy->IsDead() && !baon->IsDead()){
				if(baon->isDamage){
					enemy->TakeDamage(true);
				}
				if(!baon->isTakingDamage() && enemy->isDamage){
					baon->TakeDamage(true, right);
				}
			}
		}
	}
}
