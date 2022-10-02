#pragma once

#include "State.hpp"

#include "Legend.hpp"
#include "Transition.hpp"
#include "Enemy.hpp"
#include "Poof.hpp"
#include "MusicManager.hpp"

const int BPM = 144;
const float BEAT_TIME = 60.0f / BPM;
const float OK_WINDOW = 0.15;
const float PERFECT_WINDOW = 0.05;
const sf::Vector2f LEGEND_POSITION(120, 74);
const float INPUT_BUFFER_TIME = 0.15;

class PlayState : public State {
public:
	virtual void init() override;
	virtual void gotEvent(sf::Event event) override;
	virtual void update(sf::Time elapsed) override;
	virtual void render(sf::RenderWindow &window) override;

	void createEnemy(std::string type, int direction, float delayBeats = 4);
	void createPoof(sf::Vector2f position, float diameter = 12);

	void loseLevel();

	// Helpers
	bool isNearBeat(float window = 0.05, bool onlyAfter = false);
	sf::Vector2f getDirectionVector(int direction);

private:
	void onBeat();

	// 0 = sword
	// 1 = shield
	void attack(int type = 0);
	void doBufferInput();

	int level = 1;
	std::string areaName = "Mountain";
	bool breakTime = false;
	float beatTimer = 0;
	int beatCounter = -1;

	float attackBuffer = 0;
	int attackType = 0;
	int attackDirection = 0;
	int attackFrame = 8;
	float attackFrameTimer = 0;

	std::string breakMessage1;
	std::string breakMessage2;

	Legend legend;
	sf::Sprite attackSprite;
	std::vector<Enemy> enemies;
	std::vector<Poof> poofs;
	Transition trans;

	sf::Sound metronome;
	sf::Sound poofSound;

	MusicManager music;
};

