#pragma once
#include <string>
#include <memory>

class Metadata {
	public:
		void setId(int id);
		void setId(const std::string& id);
		const int getId() const;

		void setName(const std::string& name);
		const std::string& getName() const;

	protected:
		int id = -1;
		std::string name;
};