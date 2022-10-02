#include "PlayState.hpp"

#include "BitmapText.hpp"
#include "ResourceManager.hpp"

#include <iostream>

void PlayState::init() {
	legend.setState(this);
	legend.setPosition(LEGEND_POSITION);

	trans.cover(true);
	trans.reveal();

	metronome.setBuffer(rm::loadSoundBuffer("Resource/Sound/Metronome.wav"));
	crabSpawnSound.setBuffer(rm::loadSoundBuffer("Resource/Sound/CrabSpawn.wav"));

	music.setKey("C5");
}

void PlayState::gotEvent(sf::Event event) {
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::Z) {
			// Todo: check if any buffers are not expired, add those to missed count
			swordBuffer = INPUT_BUFFER_TIME;
			shieldBuffer = 0;
			doInputFromBuffers();
		}
		else if (event.key.code == sf::Keyboard::X) {
			swordBuffer = 0;
			shieldBuffer = INPUT_BUFFER_TIME;
			doInputFromBuffers();
		}
		else if (event.key.code == sf::Keyboard::Up) {
			legend.turn(0);
			doInputFromBuffers();
		}
		else if (event.key.code == sf::Keyboard::Right) {
			legend.turn(1);
			doInputFromBuffers();
		}
		else if (event.key.code == sf::Keyboard::Down) {
			legend.turn(2);
			doInputFromBuffers();
		}
		else if (event.key.code == sf::Keyboard::Left) {
			legend.turn(3);
			doInputFromBuffers();
		}

		// DEBUG
		if (event.key.code == sf::Keyboard::Q) {
			music.playRandomNote("Sword");
		}
	}
}

void PlayState::update(sf::Time elapsed) {
	// Update input buffers
	// Todo: if buffer expires, add to missed attack count
	swordBuffer -= elapsed.asSeconds();
	shieldBuffer -= elapsed.asSeconds();

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
			levelTimer += 3.33;
			trans.cover();

			if (legend.alive) {
				breakMessage1 = "Level " + std::to_string(level) + " Complete!";
				breakMessage2 = "Moving on!";
			}
			else {
				breakMessage1 = "You died!";
				breakMessage2 = "Let's go again!";
			}
		}
	}

	// Update beat timer
	beatTimer -= elapsed.asSeconds();
	if (beatTimer <= 0) {
		beatTimer += BEAT_TIME;
		beatCounter += 1;
		if (beatCounter <= 31) {
			onBeat();
		}
	}

	// Set up next level
	if (breakTime && beatCounter >= 29 && trans.isCovered()) {
		enemies.clear();
		poofs.clear();
		if (legend.alive) {
			level += 1;
		}
		legend.alive = true;
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
		text.setText(breakMessage1);
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
		text.setText(breakMessage2);
		text.setPosition(120 - text.getWidth() / 2, 88);
		window.draw(text);
	}

	// DEBUG
	text.setText(std::to_string(levelTimer));
	text.setPosition(2, 125);
	//window.draw(text);
	text.setText(std::to_string(beatCounter));
	text.setPosition(2, 115);
	//window.draw(text);
	text.setText("Key: " + music.getKey());
	text.setPosition(2, 125);
	//window.draw(text);
}

void PlayState::createEnemy(std::string type, int direction, float delayBeats) {
	if (legend.alive) {
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
}

void PlayState::createPoof(sf::Vector2f position, float diameter) {
	poofs.emplace_back(diameter);
	poofs.back().setPosition(position);
}

void PlayState::loseLevel() {
	if (legend.alive && !breakTime) {
		legend.alive = false;
		createPoof(LEGEND_POSITION + sf::Vector2f(0, -6), 24);

		// Todo: don't clear enemies here
		enemies.clear();
	}
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
	std::string enemyType = "Crab";
	if (level % 2 == 0) {
		enemyType = "Bird";
	}
	if (!breakTime) {
		if (beatCounter == 4) {
			createEnemy(enemyType, 1, 8);
		}
		else if (beatCounter == 16) {
			createEnemy(enemyType, 1, 4);
		}
		else if (beatCounter == 17) {
			createEnemy(enemyType, 3, 4);
		}
	}

	// Adjust chord progression
	if (beatCounter == 0) {
		music.setChordByCode("1");
	}
	else if (beatCounter == 8) {
		music.setChordByCode("4");
	}
	else if (beatCounter == 16) {
		music.setChordByCode("2m");
	}
	else if (beatCounter == 24) {
		music.setChord({0, 4, 7, 10}, 7);
	}

	// Play beat
	//metronome.play();
	if (beatCounter % 2 == 0) {
		music.playChordBase("Bass");
	}
	if (beatCounter % 4 == 0) {
		//music.playChord("Complete");
	}

	if (beatCounter >= 26 && beatCounter <= 29) {
		music.playRandomNote("Complete");
	}
}

void PlayState::doInputFromBuffers() {
	if (legend.alive) {
		int direction = legend.facing;
		for (Enemy &enemy : enemies) {
			if (enemy.alive && enemy.side == direction && enemy.getDelay() <= OK_WINDOW) {
				if (swordBuffer > 0) {
					swordBuffer = 0;
					enemy.alive = false;
					createPoof(enemy.getPosition() + sf::Vector2f(0, -6));
					music.playRandomNote("Sword");
				}
			}
		}
	}
}
