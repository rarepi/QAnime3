#pragma once
#include <string>
#include "Metadata.h"

class Season;

class Episode : public Metadata {
	public:
		Episode(Season& season, int id);
		Episode(Season& season, std::string id);

		void setSeason(Season& season);
		void setAbsolute(const int absolute);
		void setRuntime(const unsigned int ms);
		void setFirstAiredDate(const std::string firstAiredDate);
		void setFirstAiredBroadcaster(const std::string firstAiredBroadcaster);
		void setTVDBUrl(const std::string TVDBUrl);

		const Season* getSeason() const;
		const int getAbsolute() const;
		const int getRuntime() const;
		const std::string getFirstAiredDate() const;
		const std::string getFirstAiredBroadcaster() const;
		const std::string getTVDBUrl() const;

	private:
		Season* season;
		int absolute = -1;
		int runtime = -1;
		std::string firstAiredDate;
		std::string firstAiredBroadcaster;
		std::string TVDBUrl;
};