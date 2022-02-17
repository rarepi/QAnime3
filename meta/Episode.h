#pragma once
#include <string>
#include "Metadata.h"

class Season;

class Episode : public Metadata {
	public:
		Episode();
		Episode(const Season& season, const int id);
		Episode(const Season& season, const std::string& id);

		void setSeason(const Season& season);
		void setAbsolute(const int absolute);
		void setAbsolute(const std::string& absolute);
		void setRuntime(const unsigned int ms);
		void setRuntime(const std::string& ms);
		void setFirstAiredDate(const std::string& firstAiredDate);
		void setFirstAiredBroadcaster(const std::string& firstAiredBroadcaster);
		void setTVDBUrl(const std::string& TVDBUrl);

		const std::shared_ptr<Season> getSeason() const;
		const int getAbsolute() const;
		const int getRuntime() const;
		const std::string getFirstAiredDate() const;
		const std::string getFirstAiredBroadcaster() const;
		const std::string getTVDBUrl() const;

	private:
		std::weak_ptr<Season> season;
		int absolute = -1;
		int runtime = -1;
		std::string firstAiredDate;
		std::string firstAiredBroadcaster;
		std::string TVDBUrl;
};