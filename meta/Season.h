#pragma once
#include <string>
#include <vector>
#include "Metadata.h"
#include "Episode.h"

class Series;

class Season : public Metadata {
	public:
		Season();
		Season(const Series& series, int id);
		~Season();

		void setSeries(const Series& series);
		const std::shared_ptr<Series> getSeries() const;
		void addEpisode(const Episode& episode);
		const std::vector<std::shared_ptr<Episode>>& getEpisodes() const;

	private:
		std::weak_ptr<Series> series;
		std::vector<std::shared_ptr<Episode>> episodes;
};