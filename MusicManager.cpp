#include "MusicManager.hpp"

#include "ResourceManager.hpp"

MusicManager::MusicManager() {
	sounds.resize(10);
}

void MusicManager::setKey(std::string key) {
	this->key = key;
}

void MusicManager::setChordByCode(std::string code) {
	chord.clear();
	int offset = 0;
	std::string suffix = code.substr(1);
	std::string chordNum = code.substr(0, 1);
	if (chordNum == "2") {
		offset = 2;
	}
	else if (chordNum == "3") {
		offset = 4;
	}
	else if (chordNum == "4") {
		offset = 5;
	}
	else if (chordNum == "5") {
		offset = 7;
	}
	else if (chordNum == "6") {
		offset = 9;
	}
	else if (chordNum == "7") {
		offset = 11;
	}
	chord.push_back(offset);
	chord.push_back(offset + (suffix == "m" ? 3 : 4));
	chord.push_back(offset + 7);
}

void MusicManager::setChord(std::vector<int> chord, int offset) {
	for (int &pitch : chord) {
		pitch += offset;
	}
	this->chord = chord;
}

void MusicManager::playNote(std::string instrument, int offset) {
	int pitch = getPitchFromNoteCode(key) + offset;
	sounds[currentSound] = std::make_shared<sf::Sound>();
	sounds[currentSound]->setBuffer(rm::loadSoundBuffer("Resource/Sound/Instrument/" + instrument + ".wav"));
	sounds[currentSound]->setPitch(std::pow(1.0594630943, pitch - 36));
	sounds[currentSound]->play();

	currentSound += 1;
	if (currentSound >= sounds.size()) {
		currentSound -= sounds.size();
	}
}

void MusicManager::playChordBase(std::string instrument) {
	if (!chord.empty()) {
		playNote(instrument, chord[0]);
	}
}

void MusicManager::playChord(std::string instrument) {
	for (int offset : chord) {
		playNote(instrument, offset);
	}
}

void MusicManager::playRandomNote(std::string instrument) {
	if (!chord.empty()) {
		playNote(instrument, chord[std::rand() % chord.size()]);
	}
}

std::string MusicManager::getKey() {
	return key;
}

int MusicManager::getPitchFromNoteCode(std::string code) {
	// This is really ugly :(
	int pitch = -24;
	std::string note = code.substr(0, code.size() - 1);
	char octave = code.back();
	if (note == "Db") {
		pitch += 1;
	}
	else if (note == "D") {
		pitch += 2;
	}
	else if (note == "Eb") {
		pitch += 3;
	}
	else if (note == "E") {
		pitch += 4;
	}
	else if (note == "F") {
		pitch += 5;
	}
	else if (note == "Gb") {
		pitch += 6;
	}
	else if (note == "G") {
		pitch += 7;
	}
	else if (note == "Ab") {
		pitch += 8;
	}
	else if (note == "A") {
		pitch += 9;
	}
	else if (note == "Bb") {
		pitch += 10;
	}
	else if (note == "B") {
		pitch += 11;
	}
	pitch += (octave - '0') * 12;
	return pitch;
}

