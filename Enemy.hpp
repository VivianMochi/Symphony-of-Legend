#pragma once

#include <SFML/Graphics.hpp>

class PlayState;

class Enemy : public sf::Drawable, public sf::Transformable {
public:
	Enemy(std::string type = "Crab", float delay = 20);

	void setState(PlayState *state);
	void update(sf::Time elapsed);

	float getDelay();

	bool alive = true;
	int side = 1;
	int facing = 3;

private:
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

	PlayState *state = nullptr;

	float maxDelay = 20;
	float delay = 20;
	sf::Vector2f velocity;

	int frame = 0;
	float frameTimer = 0;

	sf::Sprite sprite;
};

