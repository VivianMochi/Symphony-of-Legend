#include "PlayState.hpp"

#include "BitmapText.hpp"
#include "ResourceManager.hpp"
#include "CompleteState.hpp"

#include <iostream>

Level::Level(std::string name, sf::Color color, std::string beatPattern, std::string instrument) {
	this->name = name;
	this->color = color;
	this->beatPattern = beatPattern;
	this->instrument = instrument;
}

void PlayState::init() {
	// Construct levels
	levels.emplace_back("Beach", sf::Color(0xD3C1A5FF), "C---", "Forest");
	levels.emplace_back("Fields", sf::Color(0x6FA870FF), "C000C202", "Forest");
	levels.emplace_back("Forest", sf::Color(0x67A552FF), "C121C121", "Forest");
	levels.emplace_back("Mountain", sf::Color(0x9294A0FF), "0--2--0-", "Mountain");
	levels.emplace_back("Cave", sf::Color(0x897A91FF), "0-------", "Mountain");
	levels.emplace_back("Desert", sf::Color(0xE2CF8EFF), "02-2-212", "Desert");
	levels.emplace_back("Dunes", sf::Color(0xFFBF8EFF), "-0-0-012", "Desert");
	levels.emplace_back("Ocean", sf::Color(0x9599DBFF), "02----2-", "Ocean");
	levels.emplace_back("Clouds", sf::Color(0xAED9F9FF), "01201212", "Ocean");
	levels.emplace_back("Space", sf::Color(0x4F55A8FF), "1-0-1-0-2-0-2-0-", "Space");
	generateSpawns();

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
			// Shield isn't fully implemented
			//attack(1);
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

		// Do aim-attack keys
		if (event.key.code == sf::Keyboard::W) {
			legend.turn(0);
			attack(0);
		}
		else if (event.key.code == sf::Keyboard::D) {
			legend.turn(1);
			attack(0);
		}
		else if (event.key.code == sf::Keyboard::S) {
			legend.turn(2);
			attack(0);
		}
		else if (event.key.code == sf::Keyboard::A) {
			legend.turn(3);
			attack(0);
		}

		// DEBUG
		/*
		if (event.key.code == sf::Keyboard::Q) {
			music.playRandomNote("Sword");
		}
		else if (event.key.code == sf::Keyboard::Num1) {
			breakTime = true;
			beatCounter = 29;
			trans.cover(true);
		}
		*/
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
			allAttacks += totalAttacks;
			allPerfect += perfect;
			allMisses += misses;
			level += 1;
			if (level >= levels.size()) {
				level = levels.size() - 1;
				getGame()->changeState(new CompleteState(allPerfect, allAttacks, allMisses));
			}
		}
		totalAttacks = 0;
		perfect = 0;
		misses = 0;
		legend.alive = true;
		generateSpawns();
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
	backdrop.setFillColor(levels[level].color);
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

	// Render hud
	BitmapText text(rm::loadTexture("Resource/Image/Font.png"), "Level " + std::to_string(level + 1) + ": " + levels[level].name);
	text.setColor(sf::Color::White);
	text.setPosition(2, 2);

	sf::RectangleShape hudBox(sf::Vector2f(text.getWidth() + 4, 16));
	hudBox.setFillColor(sf::Color(0x40404040));
	hudBox.setPosition(sf::Vector2f(0, -4));
	sf::Sprite hudEdge(rm::loadTexture("Resource/Image/Hud.png"), sf::IntRect(0, 0, 8, 16));
	hudEdge.setColor(sf::Color(0x40404040));
	hudEdge.setPosition(hudBox.getPosition() + sf::Vector2f(hudBox.getSize().x, 0));

	window.draw(hudBox);
	window.draw(hudEdge);
	window.draw(text);

	// Render transition
	window.draw(trans);

	// Render break text
	if (!gameComplete) {
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
		}
		if (beatCounter >= 29) {
			text.setText(breakMessages[3]);
			text.setPosition(120 - text.getWidth() / 2, 88);
			window.draw(text);
		}
	}

	// DEBUG
	/*
	text.setText(std::to_string(beatCounter));
	text.setPosition(2, 115);
	//window.draw(text);
	text.setText("Key: " + music.getKey());
	text.setPosition(2, 125);
	//window.draw(text);
	*/
}

void PlayState::createEnemy(std::string type, int direction) {
	if (legend.alive) {
		enemies.emplace_back(type);
		enemies.back().setState(this);
		sf::Vector2f offsetPosition = getDirectionVector(direction) * (getDirectionVector(direction).x == 0 ? 40.0f : 60.0f);
		offsetPosition += sf::Vector2f(std::rand() % 20 - 10, std::rand() % 20 - 10);
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

std::vector<float> PlayState::getEnemyDelays(std::string enemyType) {
	if (enemyType == "Crab") {
		return { 4 };
	}
	else if (enemyType == "Hedgehog") {
		return { 4, 8 };
	}
	else if (enemyType == "Bird") {
		return { 6 };
	}
	else {
		return { 4 };
	}
}

void PlayState::generateSpawns() {
	spawns.clear();

	// Get valid enemy types
	// Todo: Add spawn weights so that we don't need to add duplicates
	// We are weighting hedgehogs at 1/2 because they take double the hits to kill
	std::vector<std::string> enemyTypes = { "Crab", "Crab" };
	if (level >= 3) {
		enemyTypes.push_back("Hedgehog");
	}
	if (level >= 6) {
		enemyTypes.push_back("Bird");
		enemyTypes.push_back("Bird");
	}

	// Spawn settings
	SpawnSettings settings;
	if (level == 0) {
		settings.spawnStart = 8;
	}
	if (level >= 8) {
		settings.allowDoubles = true;
	}
	if (level >= 9) {
		settings.allowSpawnAnytime = true;
	}
	
	// Calculate spawn times
	for (int i = settings.attackEnd; i >= settings.attackStart; i--) {
		// Spawn chance
		if (std::rand() % 20 <= level + 8) {
			// Trim out enemy types that won't properly fit into this beat
			std::vector<std::string> validEnemyTypes = enemyTypes;
			auto iter = validEnemyTypes.begin();
			while (iter != validEnemyTypes.end()) {
				if (!testEnemyFit(*iter, i, settings)) {
					iter = validEnemyTypes.erase(iter);
				}
				else {
					iter++;
				}
			}

			if (!validEnemyTypes.empty()) {
				// Construct spawn
				EnemySpawn spawn;
				spawn.type = validEnemyTypes[std::rand() % validEnemyTypes.size()];
				spawn.beat = i - getEnemyDelays(spawn.type).back();
				spawn.side = std::rand() % 4;
				spawns.push_back(spawn);

				// Double hit chance
				if (settings.allowDoubles && spawn.type != "Bird" && std::rand() % 10 == 0) {
					spawn.type = "Bird";
					spawn.beat = i - getEnemyDelays(spawn.type).back();
					spawns.push_back(spawn);
				}
			}
		}
	}
}

bool PlayState::testEnemyFit(std::string enemyType, float attackBeat, SpawnSettings settings) {
	// Check spawn restrictions
	float spawnBeat = attackBeat - getEnemyDelays(enemyType).back();
	if (spawnBeat < settings.spawnStart) {
		return false;
	}
	if (std::fmod(spawnBeat, 8) >= 4 && !settings.allowSpawnAnytime) {
		return false;
	}

	// Reserve used beats
	std::set<float> reserved;
	for (EnemySpawn &spawn : spawns) {
		for (float time : getEnemyDelays(spawn.type)) {
			reserved.emplace(spawn.beat + time);
		}
	}

	// Determine if overlap exists
	for (float time : getEnemyDelays(enemyType)) {
		if (reserved.count(spawnBeat + time) != 0) {
			return false;
		}
	}
	return true;
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
	std::string beatPattern = levels[level].beatPattern;
	if (beatPattern != "" && (legend.alive || beatCounter >= 24)) {
		int index = beatCounter % beatPattern.size();
		if (beatPattern[index] == 'C') {
			music.playChord("Complete");
			music.playChordNote(levels[level].instrument);
		}
		else if (beatPattern[index] != '-') {
			music.playChordNote(levels[level].instrument, beatPattern[index] - '0');
		}
	}

	// Play completion sounds
	if (!gameComplete) {
		if (beatCounter == 24) {
			music.playChordNote("Complete", 0);
		}
		else if (beatCounter >= 26 && beatCounter <= 29) {
			music.playRandomNote("Complete");
		}
		else if (beatCounter == 30) {
			music.playChordNote("Complete", 2);
		}
	}

	// Go to break
	if (beatCounter == 24) {
		breakTime = true;
		trans.cover();
		if (legend.alive) {
			if (level == levels.size() - 1) {
				gameComplete = true;
			}
			breakMessages[0] = levels[level].name + " Complete!";
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
	if (!breakTime) {
		for (EnemySpawn &spawn : spawns) {
			if (beatCounter == spawn.beat) {
				createEnemy(spawn.type, spawn.side);
			}
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
