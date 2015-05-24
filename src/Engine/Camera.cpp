/*
 * Camera.cpp
 *
 *  Created on: 16/04/2015
 *      Author: Pedro
 */

#include "Camera.h"

#include <SDL_keycode.h>

#include "Game.h"
#include "InputManager.h"

#define LEFT_ARROW_KEY SDLK_LEFT
#define RIGHT_ARROW_KEY SDLK_RIGHT
#define UP_ARROW_KEY SDLK_UP
#define DOWN_ARROW_KEY SDLK_DOWN
#define ESCAPE_KEY SDLK_ESCAPE
#define LEFT_MOUSE_BUTTON SDL_BUTTON_LEFT

Point Camera::pos;
GameObject* Camera::focus = NULL;

void Camera::Follow(GameObject* newFocus) {
	focus = newFocus;
}

void Camera::Unfollow() {
	focus = NULL;
}

void Camera::Update(float dt) {
	if(focus != NULL){
			pos.setX(-focus->GetBox().GetX() + Game::SCREEN_WIDTH/2);
			pos.setY(-focus->GetBox().GetY() + Game::SCREEN_HEIGHT/2);
	}
	else{
		if(InputManager::GetInstance().IsKeyDown(LEFT_ARROW_KEY)){
			pos.setX(pos.getX() + 200*dt);
		}

		if(InputManager::GetInstance().IsKeyDown(RIGHT_ARROW_KEY)){
			pos.setX(pos.getX() - 200*dt);
		}

		if(InputManager::GetInstance().IsKeyDown(DOWN_ARROW_KEY)){
			pos.setY(pos.getY() - 200*dt);
		}

		if(InputManager::GetInstance().IsKeyDown(UP_ARROW_KEY)){
			pos.setY(pos.getY() + 200*dt);
		}
	}
}
