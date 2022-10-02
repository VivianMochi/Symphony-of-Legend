#include "PlayState.hpp"

#include "BitmapText.hpp"
#include "ResourceManager.hpp"

#include <iostream>

void PlayState::init() {
	breakMessages.resize(4);

	legend.setState(this);
	legend.setPosition(LEGEND_POSITION);

	attackSprite.setTexture(rm::loadTexture("Resource/Image/Attacks.png"));
	attackSprite.setOrigin(6, 6);

	trans.cover(true);
	trans.reveal();

	deathSound.setBuffer(rm::loadSoundBuffer("Resource/Sound/Death.wav"));
	poofSound.setBuffer(rm::loadSoundBuffer("Resource/Sound/Poof.wav"));

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
	float lastBuffer = attackBuffer;
	attackBuffer -= elapsed.asSeconds();
	if (lastBuffer > 0 && attackBuffer <= 0) {
		// Last buffer was not consumed, add a miss
		misses += 1;
	}

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
		totalAttacks = 0;
		perfect = 0;
		misses = 0;
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
		text.setText(breakMessages[0]);
		text.setPosition(120 - text.getWidth() / 2, 38);
		window.draw(text);
	}
	if (beatCounter >= 27) {
		text.setText(breakMessages[1]);
		text.setPosition(120 - text.getWidth() / 2, 58);
		window.draw(text);
	}
	if (beatCounter >= 28) {
		text.setText(breakMessages[2]);
		text.setPosition(120 - text.getWidth() / 2, 68);
		window.draw(text);
		// Todo: also show missed attacks?
	}
	if (beatCounter >= 29) {
		text.setText(breakMessages[3]);
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

void PlayState::createEnemy(std::string type, int direction) {
	if (legend.alive) {
		enemies.emplace_back(type);
		enemies.back().setState(this);
		sf::Vector2f offsetPosition = getDirectionVector(direction) * (getDirectionVector(direction).x == 0 ? 40.0f : 60.0f);
		enemies.back().setPosition(LEGEND_POSITION + offsetPosition);
		enemies.back().side = direction;
		enemies.back().facing = direction + 2;
		if (enemies.back().facing >= 4) {
			enemies.back().facing -= 4;
		}

		createPoof(enemies.back().getPosition() + sf::Vector2f(0, -6));

		poofSound.setPitch(0.8 + std::rand() % 40 / 100.0f);
		poofSound.play();

		music.playRandomNote(type);
	}
}

void PlayState::createPoof(sf::Vector2f position, float diameter) {
	poofs.emplace_back(diameter);
	poofs.back().setPosition(position);
}

void PlayState::loseLevel() {
	if (legend.alive && !breakTime) {
		legend.alive = false;
		beatCounter = 20;
		createPoof(LEGEND_POSITION + sf::Vector2f(0, -6), 24);
		createPoof(LEGEND_POSITION + sf::Vector2f(0, -6), 40);
		deathSound.play();

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
	// Adjust chord progression
	if (beatCounter == 0) {
		music.setChordByCode("1");
		music.setChord({ 2, 5, 9 });
	}
	else if (beatCounter == 8) {
		music.setChordByCode("4");
		music.setChord({ 2, 4, 7 });
	}
	else if (beatCounter == 16) {
		music.setChordByCode("2m");
		music.setChord({ -2, 0, 5 });
	}
	else if (beatCounter == 24) {
		music.setChord({0, 4, 7, 10}, 7);
		music.setChord({ -3, 0, 5 });
	}

	// Play beat
	std::string beatPattern = "0--2--0-";
	beatPattern = "0-1-2-0-";
	beatPattern = "C000C202";
	if (beatPattern != "" && (legend.alive || beatCounter >= 24)) {
		int index = beatCounter % beatPattern.size();
		if (beatPattern[index] == 'C') {
			music.playChord("Complete");
			music.playChordNote(areaName);
		}
		else if (beatPattern[index] != '-') {
			music.playChordNote(areaName, beatPattern[index] - '0');
		}
	}

	// Play completion sounds
	if (beatCounter == 24) {
		music.playChordNote("Complete", 0);
	}
	else if (beatCounter >= 26 && beatCounter <= 29) {
		music.playRandomNote("Complete");
	}
	else if (beatCounter == 30) {
		music.playChordNote("Complete", 2);
	}

	// Go to break
	if (beatCounter == 24) {
		breakTime = true;
		trans.cover();
		if (legend.alive) {
			breakMessages[0] = "Level " + std::to_string(level) + " Complete!";
			breakMessages[1] = "Perfect: " + std::to_string(perfect) + " of " + std::to_string(totalAttacks);
			breakMessages[2] = "Misses: " + std::to_string(misses);
			breakMessages[3] = "Moving on!";
		}
		else {
			breakMessages[0] = "You died!";
			breakMessages[1] = "";
			breakMessages[2] = "";
			breakMessages[3] = "Try again!";
		}
	}

	// Spawn enemies
	std::vector<std::string> enemyTypes = { "Crab", "Bird" };
	if (!breakTime) {
		if (beatCounter % 8 <= 3 && std::rand() % 4 && beatCounter < 24) {
			createEnemy(enemyTypes[std::rand() % enemyTypes.size()], std::rand() % 4);
		}
	}
}

void PlayState::attack(int type) {
	if (legend.alive) {
		totalAttacks += 1;
		if (attackBuffer > 0) {
			misses += 1;
		}
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
				if (enemy.getDelay() >= -PERFECT_WINDOW && enemy.getDelay() <= PERFECT_WINDOW) {
					perfect += 1;
				}
				attackBuffer = 0;
				enemy.hit(attackType);
				music.playRandomNote(attackType == 0 ? "Sword" : "Shield");
			}
		}
	}
}
