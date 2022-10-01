#include "MenuState.hpp"

#include "PlayState.hpp"
#include "ResourceManager.hpp"
#include "BitmapText.hpp"

void MenuState::init() {

}

void MenuState::gotEvent(sf::Event event) {
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::Space) {
			trans.cover();
		}
	}
}

void MenuState::update(sf::Time elapsed) {
	trans.update(elapsed);
	if (trans.isCovered()) {
		getGame()->changeState(new PlayState());
	}
}

void MenuState::render(sf::RenderWindow &window) {
	sf::RectangleShape backdrop(sf::Vector2f(240, 135));
	backdrop.setFillColor(sf::Color(60, 180, 230));
	window.draw(backdrop);

	BitmapText text(rm::loadTexture("Resource/Image/Font.png"), "Main Menu");
	text.setPosition(2, 2);
	text.setColor(sf::Color::White);
	window.draw(text);

	window.draw(trans);
}
