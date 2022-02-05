#pragma once
#include <string>
#include <vector>

#include <curl/curl.h>
#include <gumbo.h>

#include "../meta/Series.h"

class TVDBHandler {
	public:
		static TVDBHandler* getInstance();
		Season* getSeasonData(const std::string tvdbName, const int season);

	private:
		TVDBHandler();	// private constructor for singleton
		static TVDBHandler* instance;

		std::string getHtml(const std::string& url);
		Season* parseSeason(const char* html, int season);
		GumboNode* findTbodyGumbo(GumboNode* node);
};