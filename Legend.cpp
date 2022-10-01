#include "Legend.hpp"

#include "PlayState.hpp"
#include "ResourceManager.hpp"

Legend::Legend() {
	sprite.setTexture(rm::loadTexture("Resource/Image/Legend.png"));
	sprite.setOrigin(6, 12);
}

void Legend::setState(PlayState *state) {
	this->state = state;
}

void Legend::update(sf::Time elapsed) {
	// Update sprite
	int spriteXIndex = 0;
	int spriteYIndex = 0;
	if (facing == 2) {
		spriteXIndex = 1;
	}
	else if (facing == 0) {
		spriteXIndex = 2;
	}
	if (state && state->isNearBeat(0.1, true)) {
		spriteYIndex = 1;
	}
	sprite.setTextureRect(sf::IntRect(12 * spriteXIndex, 12 * spriteYIndex, 12, 12));
	sprite.setScale((facing == 3 ? -1 : 1), 1);
	sprite.setPosition(getPosition());
}

void Legend::turn(int direction) {
	facing = direction;
}

void Legend::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	target.draw(sprite);
}
