#pragma once

#include <SFML/Audio.hpp>
#include <string>
#include <vector>
#include <memory>

class MusicManager {
public:
	MusicManager();

	void setKey(std::string key);
	void setChordByCode(std::string code);
	void setChord(std::vector<int>, int offset = 0);

	void playNote(std::string instrument, int offset = 0);
	void playChordNote(std::string instrument, int index = 0);
	void playChord(std::string instrument);
	void playRandomNote(std::string instrument);

	std::string getKey();

private:
	// Helper
	int getPitchFromNoteCode(std::string code);

	std::string key = "C5";
	std::vector<int> chord;
	std::vector<std::shared_ptr<sf::Sound>> sounds;
	int currentSound = 0;
};
