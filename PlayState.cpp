#include "PlayState.hpp"

#include "BitmapText.hpp"
#include "ResourceManager.hpp"

#include <iostream>

void PlayState::init() {
	legend.setState(this);
	legend.setPosition(LEGEND_POSITION);

	attackSprite.setTexture(rm::loadTexture("Resource/Image/Attacks.png"));
	attackSprite.setOrigin(6, 6);

	trans.cover(true);
	trans.reveal();

	metronome.setBuffer(rm::loadSoundBuffer("Resource/Sound/Metronome.wav"));
	crabSpawnSound.setBuffer(rm::loadSoundBuffer("Resource/Sound/CrabSpawn.wav"));

	music.setKey("C5");
}

void PlayState::gotEvent(sf::Event event) {
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::Z) {
			attack(0);
		}
		else if (event.key.code == sf::Keyboard::X) {
			attack(1);
		}
		else if (event.key.code == sf::Keyboard::Up) {
			legend.turn(0);
			doBufferInput();
		}
		else if (event.key.code == sf::Keyboard::Right) {
			legend.turn(1);
			doBufferInput();
		}
		else if (event.key.code == sf::Keyboard::Down) {
			legend.turn(2);
			doBufferInput();
		}
		else if (event.key.code == sf::Keyboard::Left) {
			legend.turn(3);
			doBufferInput();
		}

		// DEBUG
		if (event.key.code == sf::Keyboard::Q) {
			music.playRandomNote("Sword");
		}
	}
}

void PlayState::update(sf::Time elapsed) {
	// Update input buffer
	// Todo: if buffer expires, add to missed attack count
	attackBuffer -= elapsed.asSeconds();

	// Update beat timer
	beatTimer -= elapsed.asSeconds();
	if (beatTimer <= 0) {
		beatTimer += BEAT_TIME;
		beatCounter += 1;
		if (beatCounter == 32) {
			// Start level
			breakTime = false;
			beatTimer = BEAT_TIME;
			beatCounter = 0;
		}
		onBeat();
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

	// Update attack sprite
	attackFrameTimer -= elapsed.asSeconds();
	if (attackFrameTimer <= 0) {
		attackFrameTimer += BEAT_TIME / 8.0f;
		attackFrame += 1;
		if (attackFrame >= 9) {
			attackFrame = 8;
		}
	}
	attackSprite.setTextureRect(sf::IntRect(attackFrame * 12, attackType * 12, 12, 12));
	attackSprite.setPosition(LEGEND_POSITION + sf::Vector2f(0, -6) + getDirectionVector(attackDirection) * 12.0f);
	attackSprite.setRotation(90 * (attackDirection - 1));

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
	if (attackFrame < 4 || (attackFrame < 8 && std::rand() % 2)) {
		window.draw(attackSprite);
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
		beatCounter = 22;
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

	// Go to break
	if (beatCounter == 24) {
		breakTime = true;
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

void PlayState::attack(int type) {
	if (legend.alive) {
		// Todo: Add to miss count if buffer isn't expired
		attackBuffer = INPUT_BUFFER_TIME;
		attackType = type;
		attackDirection = legend.facing;
		attackFrame = -1;
		attackFrameTimer = 0;

		doBufferInput();
	}
}

void PlayState::doBufferInput() {
	if (legend.alive && attackBuffer > 0) {
		attackDirection = legend.facing;

		for (Enemy &enemy : enemies) {
			if (enemy.alive && enemy.side == attackDirection && enemy.getDelay() <= OK_WINDOW) {
				attackBuffer = 0;
				enemy.hit(attackType);
				music.playRandomNote(attackType == 0 ? "Sword" : "Shield");
			}
		}
	}
}
