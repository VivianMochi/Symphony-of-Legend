#include "Enemy.hpp"

#include "PlayState.hpp"
#include "ResourceManager.hpp"

Enemy::Enemy(std::string type) {
	this->type = type;
	sprite.setTexture(rm::loadTexture("Resource/Image/" + type + ".png"));
	sprite.setOrigin(6, 12);

	maxDelay = 4 * BEAT_TIME;
	if (type == "Bird") {
		maxDelay = 6 * BEAT_TIME;
	}
	this->delay = maxDelay;

	if (type == "Hedgehog") {
		shield = 1;
	}
}

void Enemy::setState(PlayState *state) {
	this->state = state;
}

void Enemy::update(sf::Time elapsed) {
	if (alive) {
		// Update delay
		float lastDelay = delay;
		delay -= elapsed.asSeconds();

		// Update death
		if (delay <= -BEAT_TIME) {
			alive = false;
			if (state) {
				state->loseLevel();
			}
		}

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
		if (delay <= maxDelay / 2 || type == "Bird") {
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
		if (delay <= PERFECT_WINDOW && delay >= -PERFECT_WINDOW) {
			spriteYIndex = 2;
		}
		else if (state && state->isNearBeat(0.1, true)) {
			spriteYIndex = 1;
		}
		sprite.setTextureRect(sf::IntRect(12 * spriteXIndex, 12 * spriteYIndex, 12, 12));
		sprite.setScale((facing == 3 ? -1 : 1), 1);
		sprite.setPosition(getPosition());
	}
}

void Enemy::hit(int damageType) {
	if (state) {
		if (damageType == 0) {
			if (shield > 0) {
				shield -= 1;
				move(state->getDirectionVector(side) * 10.0f);
				velocity = sf::Vector2f();
				delay += BEAT_TIME * 4;
				maxDelay = BEAT_TIME * 4;
			}
			else {
				alive = false;
				state->createPoof(getPosition() + sf::Vector2f(0, -6));
			}
		}
		else if (damageType == 1) {
			state->createPoof(getPosition() + sf::Vector2f(0, -6), 8);
			move(state->getDirectionVector(side) * 10.0f);
			velocity = state->getDirectionVector(side) * 2.0f;
			delay += BEAT_TIME * 4;
			maxDelay = BEAT_TIME * 16;
		}
	}
}

float Enemy::getDelay() {
	return delay;
}

void Enemy::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	if (alive) {
		target.draw(sprite, states);
	}
}
