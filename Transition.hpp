#pragma once

#include <SFML/Graphics.hpp>

class Transition : public sf::Drawable {
public:
	Transition();

	void update(sf::Time elapsed);

	void cover(bool instant = false);
	void reveal(bool instant = false);
	bool isCovered();
	bool isRevealed();

private:
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

	bool shouldBeCovered = false;

	sf::Sprite transitionSprite;
};

