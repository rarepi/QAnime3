#pragma once
#include <string>
#include <vector>
#include "Metadata.h"
#include "Season.h"

class Series : public Metadata {
	public:
		Series();
		Series(const std::string& TVDBName);
		Series::~Series();
		void addSeason(Season* season);

		void setTVDBName(const std::string& name);
		const std::string& getTVDBName() const;
		void setAirRhythm(const std::string& airRhythm);
		const std::string& getAirRhythm() const;
		void setFirstAiredDate(const std::string& firstAiredDate);
		const std::string& getFirstAiredDate() const;
		void setFirstAiredBroadcaster(const std::string& firstAiredBroadcaster);
		const std::string& getFirstAiredBroadcaster() const;

	private:
		std::vector<Season*> seasons;
		std::string TVDBName;
		std::string airRhythm;
		std::string firstAiredDate;
		std::string firstAiredBroadcaster;
};