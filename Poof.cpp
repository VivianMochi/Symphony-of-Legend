#include "Poof.hpp"

Poof::Poof(float diameter) {
	finalDiameter = diameter * 2;
	sprite.setRadius(diameter / 2.0f);
	sprite.setFillColor(sf::Color::Transparent);
	sprite.setOutlineColor(sf::Color::White);
	sprite.setOutlineThickness(-diameter / 2.0f);
	sprite.setOrigin(sprite.getRadius(), sprite.getRadius());
}

void Poof::update(sf::Time elapsed) {
	sprite.setRadius(sprite.getRadius() + (finalDiameter / 2.0f - sprite.getRadius()) * 5.0f * elapsed.asSeconds());
	sprite.setOutlineThickness(sprite.getOutlineThickness() + (0 - sprite.getOutlineThickness()) * 5.0f *elapsed.asSeconds());
	sprite.setOrigin(sprite.getRadius(), sprite.getRadius());
}

void Poof::draw(sf::RenderTarget &target, sf::RenderStates states) const {
	states.transform.translate(getPosition());

	if (std::abs(sprite.getOutlineThickness()) >= 0.25) {
		target.draw(sprite, states);
	}
}
