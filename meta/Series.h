#pragma once
#include <string>
#include <vector>
#include "Metadata.h"
#include "Season.h"

class Series : public Metadata {
	public:
		Series(std::string name);
		Series::~Series();
		void addSeason(Season* season);
		void setTVDBName(const std::string& name);
		const std::string& getTVDBName() const;

	private:
		std::vector<Season*> seasons;
		std::string TVDBName;
		std::string airRhythm;
		std::string firstAiredDate;
		std::string firstAiredBroadcaster;
};