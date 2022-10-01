#pragma once

#include "State.hpp"

#include "Legend.hpp"
#include "Transition.hpp"

class PlayState : public State {
public:
	virtual void init() override;
	virtual void gotEvent(sf::Event event) override;
	virtual void update(sf::Time elapsed) override;
	virtual void render(sf::RenderWindow &window) override;

	bool isNearBeat(float window = 0.05, bool onlyAfter = false);

private:
	void onBeat();

	int level = 1;
	float levelTimer = 10;
	bool breakTime = false;
	float beatTimer = 0;
	float beatCounter = -1;

	std::string breakMessage = "";

	Legend legend;
	Transition trans;

	sf::Music beat;
};

