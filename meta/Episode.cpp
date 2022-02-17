#include "Episode.h"
#include "Season.h"

Episode::Episode() {
}

Episode::Episode(const Season& season, const int id) {
	this->setSeason(season);
	this->setId(id);
	this->setAbsolute(0);
	this->setRuntime(0);
}
Episode::Episode(const Season& season, const std::string& id)
	: Episode(season, std::stoi(id)) {}

void Episode::setSeason(const Season& season) {
	this->season = std::make_shared<Season>(season);
	this->getSeason()->addEpisode(*this);
}
void Episode::setAbsolute(const int absolute) {
	this->absolute = absolute;
}
void Episode::setAbsolute(const std::string& absolute) {
	this->setAbsolute(std::stoi(absolute));
}
void Episode::setRuntime(const unsigned int ms) {
	this->runtime = runtime;
}
void Episode::setRuntime(const std::string& ms) {
	this->setRuntime(std::stoi(ms));
}
void Episode::setFirstAiredDate(const std::string& firstAiredDate) {
	this->firstAiredDate = firstAiredDate;
}
void Episode::setFirstAiredBroadcaster(const std::string& firstAiredBroadcaster) {
	this->firstAiredBroadcaster = firstAiredBroadcaster;
}
void Episode::setTVDBUrl(const std::string& TVDBUrl) {
	this->TVDBUrl = TVDBUrl;
}

const std::shared_ptr<Season> Episode::getSeason() const {
	return this->season.lock();
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