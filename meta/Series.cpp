#include "Series.h"

Series::Series() {
}

Series::Series(const std::string& TVDBName) {
	this->setTVDBName(TVDBName);
}

void Series::addSeason(const Season& season) {
	this->seasons.push_back(std::make_shared<Season>(season));
}

Series::~Series() {
}

void Series::setTVDBName(const std::string& name) {
	// TODO check for spaces etc
	this->TVDBName = name;
}

const std::string& Series::getTVDBName() const {
	return this->TVDBName;
}

void Series::setAirRhythm(const std::string& airRhythm) {
	this->airRhythm = airRhythm;
}

const std::string& Series::getAirRhythm() const {
	return this->airRhythm;
}

void Series::setFirstAiredDate(const std::string& firstAiredDate) {
	this->firstAiredDate = firstAiredDate;
}

const std::string& Series::getFirstAiredDate() const {
	return this->firstAiredDate;
}

void Series::setFirstAiredBroadcaster(const std::string& firstAiredBroadcaster) {
	this->firstAiredBroadcaster = firstAiredBroadcaster;
}

const std::string& Series::getFirstAiredBroadcaster() const {
	return this->firstAiredBroadcaster;
}
