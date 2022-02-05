#include "Series.h"

Series::Series(std::string name) {
	this->setName(name);
}

void Series::addSeason(Season* season) {
	this->seasons.push_back(season);
}

Series::~Series() {
	for (Season* season : this->seasons) {
		delete season;
	}
}

void Series::setTVDBName(const std::string& name) {
	// TODO check for spaces
	this->TVDBName = name;
}

const std::string& Series::getTVDBName() const {
	return this->TVDBName;
}