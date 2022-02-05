#include "Season.h"
#include "Series.h"

Season::Season(int id) {
	this->setId(id);
}

Season::Season(Series* series, int id) {
	series->addSeason(this);
	this->series = series;
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