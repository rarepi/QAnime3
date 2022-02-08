#pragma once
#include <string>
#include <algorithm>
#include <vector>
#include <map>

#include <curl/curl.h>
#include <gumbo.h>

#include "../meta/Series.h"

class TVDBHandler {
	public:
		// creates and returns the TVDBHandler instance using the given parameters
		static TVDBHandler* createInstance(const std::string& tvdb_url, const std::string& tvdb_cache);
		// returns the current TVDBHandler instance
		static TVDBHandler* getInstance();
		Series* TVDBHandler::getSeriesData(const std::string& tvdbName);
		Season* getSeasonData(const std::string& tvdbName, const int& season);

	private:
		TVDBHandler(const std::string& tvdb_url, const std::string& tvdb_cache);	// private constructor for singleton
		static TVDBHandler* instance;
		std::string tvdb_url;
		std::string tvdb_cache;

		std::string getHtml(const std::string& url);

		Series* parseSeries(const char* html, const std::string& tvdbName);
		GumboNode* findBasicSeriesInfo(GumboNode* node);

		Season* parseSeason(const char* html, int season);
		GumboNode* findTbodyGumbo(GumboNode* node);
};