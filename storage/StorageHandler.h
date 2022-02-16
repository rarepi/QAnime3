#include <string>
#include <sstream>
#include <map>
#include <sqlite3.h>
#include <variant>
#include <any>
#include <functional>
#include <typeindex>
#include <typeinfo>

#include "../meta/Series.h"

// thanks microsoft
#pragma push_macro("DELETE")
#undef DELETE
enum class SQL_COMMAND {
	CREATE_TABLE, 
	INSERT,
	UPDATE,
	DELETE,
	SELECT
};
#pragma pop_macro("DELETE")

class SqlStatement {
	public:
		SqlStatement(SQL_COMMAND command, std::string table);
		void addItems(std::map<std::string, std::string> items);
		void addItem(std::string name, std::string value);
		void addItem(std::string name, int value);
		void addConditions(std::map<std::string, std::string> conditions);
		void addCondition(std::string column, std::string value);
		void addCondition(std::string column, int value);
		std::string buildSql();
	private:
		SQL_COMMAND type;
		std::stringstream statement;
		std::map<std::string, std::string> items;
		std::map<std::string, std::string> conditions;
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
		void selectSeriesById(int id);

		void addSeason(Season& season);
		void updateSeason(Season& season);
		void deleteSeason(Season& season);
		void addEpisode(Episode& episode);
		void updateEpisode(Episode& episode);
		void deleteEpisode(Episode& episode);

		void map(const std::map<std::string, std::string>& from, Series& to);
		void map(const Series& from, std::map<std::string, std::string>& to);
		void map(const std::map<std::string, std::string>& from, Season& to);
		void map(const Season& from, std::map<std::string, std::string>& to);
		void map(const std::map<std::string, std::string>& from, Episode& to);
		void map(const Episode& from, std::map<std::string, std::string>& to);

	private:
		StorageHandler(const std::string& filename);	// private constructor for singleton
		static StorageHandler* instance;

		int open();
		int close();
		void setupTables();

		std::string filename;
		sqlite3* connection;
};