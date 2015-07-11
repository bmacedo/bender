/*
 * Stage.cpp
 *
 *  Created on: 24/05/2015
 *      Author: Pedro2
 */

#include "Stage.h"

#include <stddef.h>
#include <SDL_pixels.h>

#include "BendHUD.h"
#include "SwordEnemy.h"
#include "Engine/Camera.h"
#include "Engine/Collision.h"
#include "Engine/Geometry/Point.h"
#include "Engine/InputManager.h"
#include "Engine/Sprite.h"
#include "PedraBasico.h"

Stage::Stage(int posX) : AbstractStage(2, 1, posX) {
	int scale = 2;
	int level = 1;

	music = new Music("audio/floresta.ogg");
	music->Play(Music::ALWAYS);

	tileMap = new TileMap("floresta.tmx", 5, scale);

	baon = new Baon(scale, tileMap->GetMapMax(), initialPositionX);
	baon->SetGroundTouchResolver(tileMap);
	
	Enemy* e = new Enemy(scale, 900);
	SwordEnemy* swordEnemy = new SwordEnemy(scale, 50);
	enemyAI = new EnemyAIManager(baon, e);
	swordEnemyAI = new SwordEnemyAIManager(baon, swordEnemy);

	AddObject(new Monumento(102, 8, scale, level));
	AddObject(new Monumento(262, 8, scale, level));

	AddObject(baon);

	AddObject(e);
	AddObject(swordEnemy);
	AddObject(new Enemy(scale, 4000));
	AddObject(new Enemy(scale, 8000));
	AddObject(new Enemy(scale, 8300));
	AddObject(new Enemy(scale, 8600));

	AddObject(new Scroll(scale, level));

	Camera::pos.setX(0);
	Camera::pos.setY(0);
	Camera::Follow(baon);
}

Stage::~Stage() {
}

bool Stage::OnLevelWon(float dt) {
	levelUpTimer->Update(dt);
	if (levelUpTimer->Get() > 1) {
		if (!levelWon) {
			music->Stop();
			levelWonSound->Play(0);
			levelWon = true;
		}
		SDL_Color color;
		color.r = color.g = color.b = 225;
		color.a = 255;
		levelUpText = new Text("font/Call me maybe.ttf", 40, Text::SOLID,
				"YOU WIN! Press space", color, Game::SCREEN_WIDTH/2 - 150, Game::SCREEN_HEIGHT/2 - 40);
		if (InputManager::GetInstance().KeyPress(SPACE_KEY)) {
			Game::GetInstance()->Push(new StageTwo());
			popRequested = true;
		}
		return true;
	}
	return false;
}
