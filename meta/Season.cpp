#include "Season.h"
#include "Series.h"

Season::Season() {
}

Season::Season(const Series& series, const int id) {
	this->setId(id);
	this->setSeries(series);
}

void Season::setSeries(const Series& series) {
	this->series = std::make_shared<Series>(series);
	this->getSeries()->addSeason(*this);
}

const std::shared_ptr<Series> Season::getSeries() const {
	return this->series.lock();
}

void Season::addEpisode(const Episode& episode) {
	this->episodes.push_back(std::make_shared<Episode>(episode));
}

const std::vector<std::shared_ptr<Episode>>& Season::getEpisodes() const {
	return this->episodes;
}

Season::~Season() {
}