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
		void setUrlTVDB(const std::string urlTVDB);

		const Season* getSeason() const;
		const int getAbsolute() const;
		const int getRuntime() const;
		const std::string getFirstAiredDate() const;
		const std::string getFirstAiredBroadcaster() const;
		const std::string getUrlTVDB() const;

	private:
		Season* season;
		int absolute;
		int runtime;
		std::string firstAiredDate;
		std::string firstAiredBroadcaster;
		std::string urlTVDB;
};