#include "Desk.h"

int Desk::ID = 0;

Desk::Desk(float _serviceTime, string _type) {
	this->serviceTime = _serviceTime;
	this->type = _type;
}

Desk::Desk(const Desk& desk) {
	this->currentID = ID++;
	this->serviceTime = desk.serviceTime;
	this->type = desk.type;
	this->isAvailable = desk.isAvailable;
}

Desk::~Desk() {

}