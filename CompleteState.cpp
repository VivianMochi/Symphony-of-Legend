#include "CompleteState.hpp"

#include "BitmapText.hpp"
#include "ResourceManager.hpp"

CompleteState::CompleteState(int perfect, int total, int missed) {
	this->perfect = perfect;
	this->total = total;
	this->missed = missed;
}

void CompleteState::init() {
	getGame()->playGlobalSound("Resource/Sound/Start.wav");
}

void CompleteState::gotEvent(sf::Event event) {

}

void CompleteState::update(sf::Time elapsed) {

}

void CompleteState::render(sf::RenderWindow &window) {
	BitmapText text(rm::loadTexture("Resource/Image/Font.png"));
	text.setColor(sf::Color::White);
	
	text.setText("You are the legend!");
	text.setPosition(120 - text.getWidth() / 2, 38);
	window.draw(text);
	
	text.setText("Total perfect: " + std::to_string(perfect) + " of " + std::to_string(total));
	text.setPosition(120 - text.getWidth() / 2, 58);
	window.draw(text);
	
	text.setText("Total misses: " + std::to_string(missed));
	text.setPosition(120 - text.getWidth() / 2, 68);
	window.draw(text);
	
	text.setText("Nice work!");
	text.setPosition(120 - text.getWidth() / 2, 88);
	window.draw(text);
}
