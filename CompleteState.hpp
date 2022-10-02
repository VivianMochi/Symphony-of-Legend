#pragma once

#include "State.hpp"

class CompleteState : public State {
public:
	CompleteState(int perfect = 0, int total = 0, int missed = 0);

	virtual void init() override;
	virtual void gotEvent(sf::Event event) override;
	virtual void update(sf::Time elapsed) override;
	virtual void render(sf::RenderWindow &window) override;

private:
	int perfect;
	int total;
	int missed;
};

