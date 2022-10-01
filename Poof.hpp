#pragma once

#include <SFML/Graphics.hpp>

class Poof : public sf::Drawable, public sf::Transformable {
public:
	Poof(float diameter = 12);

	void update(sf::Time elapsed);

private:
	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const override;

	float finalDiameter = 0;

	sf::CircleShape sprite;
};

