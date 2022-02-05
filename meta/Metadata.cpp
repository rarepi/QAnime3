#include "Metadata.h"

void Metadata::setId(const int& id) {
	this->id = id;
}

const int& Metadata::getId() const {
	return this->id;
}

void Metadata::setName(std::string& name) {
	this->name = name;
}

const std::string& Metadata::getName() const {
	return this->name;
}
