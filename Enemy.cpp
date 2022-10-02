#include "Enemy.hpp"

#include "PlayState.hpp"
#include "ResourceManager.hpp"

Enemy::Enemy(std::string type, float delay) {
	sprite.setTexture(rm::loadTexture("Resource/Image/" + type + ".png"));
	sprite.setOrigin(6, 12);

	maxDelay = delay;
	this->delay = maxDelay;
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
			state->loseLevel();
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

float Enemy::getDelay() {
	return delay;
}

void Enemy::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	if (alive) {
		target.draw(sprite, states);
	}
}
