#include "State.hpp"

State::State() {

}

State::~State() {

}

void State::setGame(Game *game) {
	this->game = game;
}

Game *State::getGame() {
	return game;
}
