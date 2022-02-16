#include "Season.h"
#include "Series.h"

Season::Season(Series& series, const int id) {
	this->setId(id);
	this->setSeries(series);
}

void Season::setSeries(Series& series) {
	series.addSeason(this);
	this->series = &series;
}

const Series* Season::getSeries() const {
	return this->series;
}

void Season::addEpisode(Episode* episode) {
	this->episodes.push_back(episode);
}

const std::vector<Episode*>& Season::getEpisodes() const {
	return this->episodes;
}

Season::~Season() {
	for (Episode* episode : this->episodes) {
		delete episode;
	}
}