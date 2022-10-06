#include "MenuState.hpp"

#include "PlayState.hpp"
#include "ResourceManager.hpp"
#include "BitmapText.hpp"

void MenuState::init() {
	trans.cover(true);
	trans.reveal();
}

void MenuState::gotEvent(sf::Event event) {
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::Z) {
			if (trans.isRevealed()) {
				transitioning = true;
				trans.cover();
				getGame()->playGlobalSound("Resource/Sound/Start.wav");
				flashTimer = 13;
			}
		}
	}
}

void MenuState::update(sf::Time elapsed) {
	flashTimer -= elapsed.asSeconds();
	if (flashTimer <= 0) {
		flashTimer = BEAT_TIME * 4;
	}

	trans.update(elapsed);
	if (transitioning && trans.isCovered() && flashTimer <= 10) {
		preload();
		getGame()->changeState(new PlayState());
	}
}

void MenuState::render(sf::RenderWindow &window) {
	sf::RectangleShape backdrop(sf::Vector2f(240, 135));
	backdrop.setFillColor(sf::Color(130, 144, 183));
	window.draw(backdrop);

	BitmapText text(rm::loadTexture("Resource/Image/Font.png"), "Symphony of Legend");
	text.setPosition(120 - text.getWidth() / 2, 38);
	text.setColor(sf::Color::White);
	window.draw(text);

	if (trans.isRevealed() && flashTimer <= BEAT_TIME * 2) {
		text.setText("Press Z to Start");
		text.setPosition(120 - text.getWidth() / 2, 88);
		window.draw(text);
	}

	window.draw(trans);
}

void MenuState::preload() {
	rm::loadTexture("Resource/Image/Attacks.png");
	rm::loadTexture("Resource/Image/Bird.png");
	rm::loadTexture("Resource/Image/Crab.png");
	rm::loadTexture("Resource/Image/Hedgehog.png");
	rm::loadTexture("Resource/Image/Legend.png");

	rm::loadSoundBuffer("Resource/Sound/Death.wav");
	rm::loadSoundBuffer("Resource/Sound/Poof.wav");
	rm::loadSoundBuffer("Resource/Sound/Start.wav");
	rm::loadSoundBuffer("Resource/Sound/Instrument/Bird.wav");
	rm::loadSoundBuffer("Resource/Sound/Instrument/Complete.wav");
	rm::loadSoundBuffer("Resource/Sound/Instrument/Crab.wav");
	rm::loadSoundBuffer("Resource/Sound/Instrument/Desert.wav");
	rm::loadSoundBuffer("Resource/Sound/Instrument/Forest.wav");
	rm::loadSoundBuffer("Resource/Sound/Instrument/Hedgehog.wav");
	rm::loadSoundBuffer("Resource/Sound/Instrument/Mountain.wav");
	rm::loadSoundBuffer("Resource/Sound/Instrument/Ocean.wav");
	rm::loadSoundBuffer("Resource/Sound/Instrument/Shield.wav");
	rm::loadSoundBuffer("Resource/Sound/Instrument/Space.wav");
	rm::loadSoundBuffer("Resource/Sound/Instrument/Sword.wav");
}
