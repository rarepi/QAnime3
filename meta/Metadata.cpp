#include "Metadata.h"

void Metadata::setId(int id) {
	this->id = id;
}

void Metadata::setId(const std::string& id) {
	this->setId(std::stoi(id));
}

const int Metadata::getId() const {
	return this->id;
}

void Metadata::setName(const std::string& name) {
	this->name = name;
}

const std::string& Metadata::getName() const {
	return this->name;
}
