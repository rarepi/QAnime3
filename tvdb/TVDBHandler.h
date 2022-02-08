#pragma once
#pragma warning( push )
#pragma warning( disable : 26812 )

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
		Season* getSeasonData(const Series& series, const int& season);

	private:
		TVDBHandler(const std::string& tvdb_url, const std::string& tvdb_cache);	// private constructor for singleton
		static TVDBHandler* instance;
		std::string tvdb_url;
		std::string tvdb_cache;

		std::string getHtml(const std::string& url);

		GumboNode* TVDBHandler::findNode(GumboNode* node, 
			GumboNodeType type, GumboTag tag, 
			const char* attribute, const char* attributeValue);

		std::map<int, int> collectSeriesSeasons(const char* html, const std::string order);

		char* findSeriesName(GumboNode* node);
		Series* parseSeriesHtml(const char* html, const std::string& tvdbName);
		Season* parseSeasonHtml(const char* html, int season);
};

#pragma warning( pop )