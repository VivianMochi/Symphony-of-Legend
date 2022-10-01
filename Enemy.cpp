#include "Enemy.hpp"

#include "PlayState.hpp"
#include "ResourceManager.hpp"

Enemy::Enemy(std::string type, float delay) {
	sprite.setTexture(rm::loadTexture("Resource/Image/Crab.png"));
	sprite.setOrigin(6, 12);

	maxDelay = delay;
	this->delay = maxDelay;
}

void Enemy::setState(PlayState *state) {
	this->state = state;
}

void Enemy::update(sf::Time elapsed) {
	// Update delay
	float lastDelay = delay;
	delay -= elapsed.asSeconds();

	// Update position
	if (delay <= maxDelay / 2) {
		if (delay >= 0) {
			sf::Vector2f desiredVelocity = ((LEGEND_POSITION - getPosition()) / (delay + BEAT_TIME)) * 0.9f;
			velocity += (desiredVelocity - velocity) * 0.1f * elapsed.asSeconds();
			velocity = velocity * 0.9f;
		}
		move(velocity);
	}

	// Update animation
	if (delay <= maxDelay / 2) {
		frameTimer -= elapsed.asSeconds();
		if (frameTimer <= 0) {
			frameTimer += BEAT_TIME / 2.0f;
			frame += 1;
			if (frame >= 4) {
				frame = 0;
			}
		}
	}

	// Update sprite
	int spriteXIndex = frame;
	int spriteYIndex = 0;
	if (delay <= 0.05 && delay >= -0.05) {
		spriteYIndex = 2;
	}
	else if (state && state->isNearBeat(0.1, true)) {
		spriteYIndex = 1;
	}
	sprite.setTextureRect(sf::IntRect(12 * spriteXIndex, 12 * spriteYIndex, 12, 12));
	sprite.setScale((facing == 3 ? -1 : 1), 1);
	sprite.setPosition(getPosition());
}

void Enemy::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	if (delay > -BEAT_TIME) {
		target.draw(sprite, states);
	}
}
