#include "PlayState.hpp"

#include "BitmapText.hpp"
#include "ResourceManager.hpp"

void PlayState::init() {
	trans.cover(true);
	trans.reveal();
}

void PlayState::gotEvent(sf::Event event) {

}

void PlayState::update(sf::Time elapsed) {
	trans.update(elapsed);
}

void PlayState::render(sf::RenderWindow &window) {
	sf::RectangleShape backdrop(sf::Vector2f(240, 135));
	backdrop.setFillColor(sf::Color(130, 190, 80));
	window.draw(backdrop);

	BitmapText text(rm::loadTexture("Resource/Image/Font.png"), "Level 1");
	text.setPosition(2, 2);
	text.setColor(sf::Color::White);
	window.draw(text);

	window.draw(trans);
}
