#pragma once
#include <string>
#include <vector>
#include "Metadata.h"
#include "Episode.h"

class Series;

class Season : public Metadata {
	public:
		Season(int id); // TODO remove
		Season(Series* series, int id);
		~Season();

		void addEpisode(Episode* episode);
		const std::vector<Episode*>& getEpisodes() const;

	private:
		Series* series;
		std::vector<Episode*> episodes;
};