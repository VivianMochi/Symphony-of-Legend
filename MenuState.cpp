#include "MenuState.hpp"

#include "PlayState.hpp"
#include "ResourceManager.hpp"
#include "BitmapText.hpp"

void MenuState::init() {
	trans.cover(true);
	trans.reveal();
}

void MenuState::gotEvent(sf::Event event) {
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::Z) {
			if (trans.isRevealed()) {
				transitioning = true;
				trans.cover();
			}
		}
	}
}

void MenuState::update(sf::Time elapsed) {
	flashTimer -= elapsed.asSeconds();
	if (flashTimer <= 0) {
		flashTimer = BEAT_TIME * 4;
	}

	trans.update(elapsed);
	if (transitioning && trans.isCovered()) {
		getGame()->changeState(new PlayState());
	}
}

void MenuState::render(sf::RenderWindow &window) {
	sf::RectangleShape backdrop(sf::Vector2f(240, 135));
	backdrop.setFillColor(sf::Color(60, 180, 230));
	window.draw(backdrop);

	BitmapText text(rm::loadTexture("Resource/Image/Font.png"), "Symphony of Legend");
	text.setPosition(120 - text.getWidth() / 2, 38);
	text.setColor(sf::Color::White);
	window.draw(text);

	if (trans.isRevealed() && flashTimer <= BEAT_TIME * 2) {
		text.setText("Press Z to Start");
		text.setPosition(120 - text.getWidth() / 2, 88);
		window.draw(text);
	}

	window.draw(trans);
}
