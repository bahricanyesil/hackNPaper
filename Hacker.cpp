#include "Hacker.h"

using namespace std;

int Hacker::ID = 1;

Hacker::Hacker(float _arrivalTime) {
	this->arrivalTime = _arrivalTime;
}

Hacker::Hacker(const Hacker& hacker) {
	this->currentID = ID++;
	this->arrivalTime = hacker.arrivalTime;
	this->numberOfGifts = hacker.numberOfGifts;
	this->enteredStickerAt = hacker.enteredStickerAt;
	this->enteredHoodieAt = hacker.enteredHoodieAt;
	this->totalWaitingSticker = hacker.totalWaitingSticker;
	this->totalWaitingHoodie = hacker.totalWaitingHoodie;
	this->totalLineChanges = hacker.totalLineChanges;
	this->totalCommits = hacker.totalCommits;
	this->validCommits = hacker.validCommits;
}

Hacker::~Hacker() {

}