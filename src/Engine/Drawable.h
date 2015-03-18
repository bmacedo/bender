/*
 * Drawable.h
 *
 *  Created on: 14/03/2015
 *      Author: Pedro
 */

#ifndef ENGINE_DRAWABLE_H_
#define ENGINE_DRAWABLE_H_
#include <iostream>
#include "SDL.h"

class Drawable {
private:
	float x,y;
	float rotation;
	std::string id;
	bool phisicalBody;
	SDL_Texture *texture;
protected:
	void SetPhysicalBody(bool isBody);
	int red, green, blue, alpha;
public:
	Drawable(std::string id, float x, float y);
	virtual ~Drawable();

	void Render();
	virtual void OnRender() = 0;

	float getRotation() const;
	void setRotation(float rotation);
	float getX() const;
	void setX(float x);
	float getY() const;
	void setY(float y);
	void setID(std::string id);
	std::string getID() const;
	bool IsPhysicalBody();
	void SetColor(int r, int g, int b, int a);
	void SetTexture(SDL_Texture* texture);
	SDL_Texture* GetTexture();

	const std::string& getId() const {
		return id;
	}

	void setId(const std::string& id) {
		this->id = id;
	}
};

#endif /* ENGINE_DRAWABLE_H_ */
