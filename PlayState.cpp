#include "PlayState.hpp"

#include "BitmapText.hpp"
#include "ResourceManager.hpp"

#include <iostream>

const int BPM = 144;
const float BEAT_TIME = 60.0f / BPM;

void PlayState::init() {
	legend.setState(this);
	legend.setPosition(120, 74);

	trans.cover(true);
	trans.reveal();

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

	legend.update(elapsed);

	// Update level info
	if (breakTime && beatCounter >= 29 && trans.isCovered()) {
		level += 1;
		trans.reveal();
	}

	trans.update(elapsed);
}

void PlayState::render(sf::RenderWindow &window) {
	// Render backdrop
	sf::RectangleShape backdrop(sf::Vector2f(240, 135));
	backdrop.setFillColor(sf::Color(130, 190, 80));
	window.draw(backdrop);

	window.draw(legend);

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

bool PlayState::isNearBeat(float window, bool onlyAfter) {
	if (onlyAfter) {
		return beatTimer > BEAT_TIME - window;
	}
	else {
		return beatTimer < window || beatTimer > BEAT_TIME - window;
	}
}

void PlayState::onBeat() {
	std::cout << (breakTime ? "Break - " : "Play - ") << beatCounter << " - " << levelTimer << "\n";
}
