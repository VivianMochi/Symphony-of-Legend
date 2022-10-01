#include "Transition.hpp"

#include "ResourceManager.hpp"

const int TRANS_SCREEN_WIDTH = 240;
const int TRANS_SPRITE_WIDTH = 480;
const int TRANS_COVER_OFFSET = 120;
const int TRANS_VERTICAL_REPEAT = 160;
const int TRANS_BUFFER_SPACE = 35;

Transition::Transition() {
	transitionSprite.setTexture(rm::loadTexture("Resource/Image/Transition.png"));
	reveal(true);
}

void Transition::update(sf::Time elapsed) {
	// Move if needed
	if (shouldBeCovered && !isCovered()) {
		transitionSprite.move((-TRANS_COVER_OFFSET - transitionSprite.getPosition().x) * 2.5 * elapsed.asSeconds(), 0);
	}
	else if (!shouldBeCovered && !isRevealed()) {
		transitionSprite.move((-TRANS_SPRITE_WIDTH - transitionSprite.getPosition().x) * 2.5 * elapsed.asSeconds(), 0);
	}

	transitionSprite.move(0, -50 * elapsed.asSeconds());

	// Wrap around
	if (transitionSprite.getPosition().x <= -TRANS_SPRITE_WIDTH + TRANS_BUFFER_SPACE) {
		transitionSprite.setPosition(TRANS_SCREEN_WIDTH, 0);
	}
	if (transitionSprite.getPosition().y <= -TRANS_VERTICAL_REPEAT) {
		transitionSprite.move(0, TRANS_VERTICAL_REPEAT);
	}
}

void Transition::cover(bool instant) {
	shouldBeCovered = true;
	if (instant) {
		transitionSprite.setPosition(-TRANS_COVER_OFFSET, 0);
	}
}

void Transition::reveal(bool instant) {
	shouldBeCovered = false;
	if (instant) {
		transitionSprite.setPosition(TRANS_SCREEN_WIDTH, 0);
	}
}

bool Transition::isCovered() {
	return shouldBeCovered && transitionSprite.getPosition().x <= -TRANS_COVER_OFFSET + TRANS_BUFFER_SPACE;
}

bool Transition::isRevealed() {
	return !shouldBeCovered && transitionSprite.getPosition().x >= TRANS_SCREEN_WIDTH;
}

void Transition::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	target.draw(transitionSprite, states);
}
