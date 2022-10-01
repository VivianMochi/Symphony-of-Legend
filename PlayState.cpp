#include "PlayState.hpp"

#include "BitmapText.hpp"
#include "ResourceManager.hpp"

#include <iostream>

void PlayState::init() {
	legend.setState(this);
	legend.setPosition(LEGEND_POSITION);

	trans.cover(true);
	trans.reveal();

	crabSpawnSound.setBuffer(rm::loadSoundBuffer("Resource/Sound/CrabSpawn.wav"));

	beat.openFromFile("Resource/Music/Beat.ogg");
	beat.setLoop(true);
	beat.play();
}

void PlayState::gotEvent(sf::Event event) {
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::Up) {
			legend.turn(0);
		}
		else if (event.key.code == sf::Keyboard::Right) {
			legend.turn(1);
		}
		else if (event.key.code == sf::Keyboard::Down) {
			legend.turn(2);
		}
		else if (event.key.code == sf::Keyboard::Left) {
			legend.turn(3);
		}
	}
}

void PlayState::update(sf::Time elapsed) {
	// Update level timer
	levelTimer -= elapsed.asSeconds();
	if (levelTimer <= 0) {
		if (breakTime) {
			// Start level
			breakTime = false;
			levelTimer += 10;
			beatTimer = 0;
			beatCounter = -1;
		}
		else {
			// Go to break
			breakTime = true;
			levelTimer = 13.33 - beat.getPlayingOffset().asSeconds();
			trans.cover();

			breakMessage = "Level " + std::to_string(level) + " Complete!";
		}
	}

	// Update beat timer
	beatTimer -= elapsed.asSeconds();
	if (beatTimer <= 0) {
		beatTimer += BEAT_TIME;
		beatCounter += 1;
		onBeat();
	}

	// Set up next level
	if (breakTime && beatCounter >= 29 && trans.isCovered()) {
		enemies.clear();
		poofs.clear();
		level += 1;
		trans.reveal();
	}

	// Update entities
	legend.update(elapsed);
	for (Enemy &enemy : enemies) {
		enemy.update(elapsed);
	}
	for (Poof &poof : poofs) {
		poof.update(elapsed);
	}

	trans.update(elapsed);
}

void PlayState::render(sf::RenderWindow &window) {
	// Render backdrop
	sf::RectangleShape backdrop(sf::Vector2f(240, 135));
	backdrop.setFillColor(sf::Color(130, 190, 80));
	window.draw(backdrop);

	// Render entities
	window.draw(legend);
	for (Enemy &enemy : enemies) {
		window.draw(enemy);
	}
	for (Poof &poof : poofs) {
		window.draw(poof);
	}

	// Render level name
	BitmapText text(rm::loadTexture("Resource/Image/Font.png"), "Level " + std::to_string(level));
	text.setColor(sf::Color::White);
	text.setPosition(2, 2);
	window.draw(text);

	// Render transition
	window.draw(trans);

	// Render break text
	if (beatCounter >= 26) {
		text.setText(breakMessage);
		text.setPosition(120 - text.getWidth() / 2, 38);
		window.draw(text);
	}
	if (beatCounter >= 27) {
		text.setText("Perfect: ???");
		text.setPosition(120 - text.getWidth() / 2, 58);
		window.draw(text);
	}
	if (beatCounter >= 28) {
		text.setText("Attempts: ???");
		text.setPosition(120 - text.getWidth() / 2, 68);
		window.draw(text);
		// Todo: also show missed attacks?
	}
	if (beatCounter >= 29) {
		text.setText("Moving on!");
		text.setPosition(120 - text.getWidth() / 2, 88);
		window.draw(text);
	}

	// DEBUG
	text.setText(std::to_string(levelTimer));
	text.setPosition(2, 125);
	window.draw(text);
	text.setText(std::to_string((int)beatCounter));
	text.setPosition(2, 115);
	window.draw(text);
}

void PlayState::createEnemy(std::string type, int direction, float delayBeats) {
	enemies.emplace_back(type, delayBeats * BEAT_TIME);
	enemies.back().setState(this);
	enemies.back().setPosition(LEGEND_POSITION + getDirectionVector(direction) * 60.0f);
	enemies.back().side = direction;
	enemies.back().facing = direction + 2;
	if (enemies.back().facing >= 4) {
		enemies.back().facing -= 4;
	}

	createPoof(enemies.back().getPosition() + sf::Vector2f(0, -6));

	crabSpawnSound.setPitch(0.8 + std::rand() % 40 / 100.0f);
	crabSpawnSound.play();
}

void PlayState::createPoof(sf::Vector2f position, float diameter) {
	poofs.emplace_back(diameter);
	poofs.back().setPosition(position);
}

bool PlayState::isNearBeat(float window, bool onlyAfter) {
	if (onlyAfter) {
		return beatTimer > BEAT_TIME - window;
	}
	else {
		return beatTimer < window || beatTimer > BEAT_TIME - window;
	}
}

sf::Vector2f PlayState::getDirectionVector(int direction) {
	if (direction == 0) {
		return sf::Vector2f(0, -1);
	}
	else if (direction == 1) {
		return sf::Vector2f(1, 0);
	}
	else if (direction == 2) {
		return sf::Vector2f(0, 1);
	}
	else if (direction == 3) {
		return sf::Vector2f(-1, 0);
	}
	return sf::Vector2f(0, 0);
}

void PlayState::onBeat() {
	if (!breakTime) {
		if (beatCounter == 4) {
			createEnemy("Crab", 1, 8);
		}
		else if (beatCounter == 16) {
			createEnemy("Crab", 1, 4);
		}
		else if (beatCounter == 17) {
			createEnemy("Crab", 3, 4);
		}
	}
}
