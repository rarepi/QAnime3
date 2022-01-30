#pragma once
#include <string>

class TVDBHandler
{
	public:
		TVDBHandler* getInstance();
		//void auth();
		//void getSeries(std::string name);
		//std::string getSeriesName(int id);
		//void cacheSeriesName(int id, std::string seriesName);
		//std::string getSeriesNameCached(int id);
		//void getSingleEpisode(int tvdb_id, int season, int episodeNumber);

	private:
		TVDBHandler();	// Singleton
		TVDBHandler* instance;
};

