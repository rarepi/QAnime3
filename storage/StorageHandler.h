#include <string>
#include <sstream>
#include <sqlite3.h>

#include "../meta/Series.h"

// basically just a pair<string, string>
class SqlItem {
	public:
		SqlItem(std::string first, std::string second);
		std::string first;
		std::string second;
};

// thanks microsoft
#pragma push_macro("DELETE")
#undef DELETE
enum class SQL_COMMAND {
	CREATE_TABLE, 
	INSERT,
	UPDATE,
	DELETE
};
#pragma pop_macro("DELETE")

class SqlStatement {
	public:
		SqlStatement(SQL_COMMAND command, std::string table);
		void addItem(std::string name, std::string value);
		void addItem(std::string name, int value);
		void addCondition(std::string column, std::string value);
		void addCondition(std::string column, int value);
		std::string buildSql();
	private:
		SQL_COMMAND type;
		std::stringstream statement;
		std::vector<SqlItem> items;
		std::vector<SqlItem> conditions;
};

class StorageHandler {
	public:
		// creates and returns the TVDBHandler instance using the given parameters
		static StorageHandler* createInstance(const std::string& filename);
		// returns the current TVDBHandler instance
		static StorageHandler* getInstance();

		void addSeries(Series& series);
		void updateSeries(Series& series);
		void deleteSeries(Series& series);
		void addSeason(Season& season);
		void updateSeason(Season& season);
		void deleteSeason(Season& season);
		void addEpisode(Episode& episode);
		void updateEpisode(Episode& episode);
		void deleteEpisode(Episode& episode);

	private:
		StorageHandler(const std::string& filename);	// private constructor for singleton
		static StorageHandler* instance;

		int open();
		int close();
		void setupTables();

		std::string filename;
		sqlite3* connection;
};