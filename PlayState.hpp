#pragma once

#include "State.hpp"

#include "Legend.hpp"
#include "Transition.hpp"
#include "Enemy.hpp"

const int BPM = 144;
const float BEAT_TIME = 60.0f / BPM;
const sf::Vector2f LEGEND_POSITION(120, 74);

class PlayState : public State {
public:
	virtual void init() override;
	virtual void gotEvent(sf::Event event) override;
	virtual void update(sf::Time elapsed) override;
	virtual void render(sf::RenderWindow &window) override;

	void createEnemy(std::string type, int direction, float delayBeats = 4);

	// Helpers
	bool isNearBeat(float window = 0.05, bool onlyAfter = false);
	sf::Vector2f getDirectionVector(int direction);

private:
	void onBeat();

	int level = 1;
	float levelTimer = 10;
	bool breakTime = false;
	float beatTimer = 0;
	float beatCounter = -1;

	std::string breakMessage = "";

	Legend legend;
	std::vector<Enemy> enemies;
	Transition trans;

	sf::Music beat;
};

