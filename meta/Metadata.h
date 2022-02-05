#pragma once
#include <string>

class Metadata {
	public:
		void setId(const int &id);
		const int& getId() const;

		void setName(std::string& name);
		const std::string& getName() const;

	protected:
		int id;
		std::string name;
};