#include "Episode.h"
#include "Season.h"

Episode::Episode(Season& season, int id) {
	this->setSeason(season);
	this->setId(id);
	this->setAbsolute(0);
	this->setRuntime(0);
}
Episode::Episode(Season& season, std::string id)
	: Episode(season, std::stoi(id)) {}

void Episode::setSeason(Season& season) {
	season.addEpisode(this);
	this->season = &season;
}
void Episode::setAbsolute(int absolute) {
	this->absolute = absolute;
}
void Episode::setRuntime(unsigned int ms) {
	this->runtime = runtime;
}
void Episode::setFirstAiredDate(std::string firstAiredDate) {
	this->firstAiredDate = firstAiredDate;
}
void Episode::setFirstAiredBroadcaster(std::string firstAiredBroadcaster) {
	this->firstAiredBroadcaster = firstAiredBroadcaster;
}
void Episode::setTVDBUrl(std::string TVDBUrl) {
	this->TVDBUrl = TVDBUrl;
}

const Season* Episode::getSeason() const {
	return this->season;
}
const int Episode::getAbsolute() const {
	return this->absolute;
}
const int Episode::getRuntime() const {
	return this->runtime;
}
const std::string Episode::getFirstAiredDate() const {
	return this->firstAiredDate;
}
const std::string Episode::getFirstAiredBroadcaster() const {
	return this->firstAiredBroadcaster;
}
const std::string Episode::getTVDBUrl() const {
	return this->TVDBUrl;
}