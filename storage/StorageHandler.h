#include <string>
#include <sstream>
#include <sqlite3.h>

#include "../meta/Series.h"

class SqlItem {
	public:
		SqlItem(std::string name, std::string value);
		std::string name;
		std::string value;
	private:
};

// thanks microsoft
#pragma push_macro("DELETE")
#undef DELETE
enum class SQL_COMMAND {
	CREATE_TABLE, 
	INSERT,
	DELETE
};
#pragma pop_macro("DELETE")

class SqlStatement {
	public:
		SqlStatement(SQL_COMMAND command, std::string argument);
		void addItem(std::string name, std::string value);
		void addItem(std::string name, int value);
		std::string buildSql();
	private:
		SQL_COMMAND type;
		std::stringstream statement;
		std::vector<SqlItem> items;
};

class StorageHandler {
	public:
		// creates and returns the TVDBHandler instance using the given parameters
		static StorageHandler* createInstance(const std::string& filename);
		// returns the current TVDBHandler instance
		static StorageHandler* getInstance();

		void addSeries(Series& series);
		void addSeason(Season& season);
		void addEpisode(Episode& episode);

	private:
		StorageHandler(const std::string& filename);	// private constructor for singleton
		static StorageHandler* instance;

		int open();
		int close();
		void setupTables();

		std::string filename;
		sqlite3* connection;
};