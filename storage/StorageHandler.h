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

		std::shared_ptr<Series> addSeries(const Series& series);
		std::shared_ptr<Series> updateSeries(const Series& series);
		void deleteSeries(const Series& series);
		std::shared_ptr<Series> getSeriesById(int id);

		std::shared_ptr<Season> addSeason(const Season& season);
		std::shared_ptr<Season> updateSeason(const Season& season);
		void deleteSeason(const Season& season);

		std::shared_ptr<Episode> addEpisode(const Episode& episode);
		std::shared_ptr<Episode> updateEpisode(const Episode& episode);
		void deleteEpisode(const Episode& episode);

		void cache(std::shared_ptr<Series> seriesPtr);
		void cache(std::shared_ptr<Season> seasonPtr);
		void cache(std::shared_ptr<Episode> episodePtr);

		template <class T>
		auto map(const std::map<std::string, std::string>& from);
		std::map<std::string, std::string> map(const Series& from);
		std::map<std::string, std::string> map(const Season& from);
		std::map<std::string, std::string> map(const Episode& from);


	private:
		StorageHandler(const std::string& filename);	// private constructor for singleton
		static StorageHandler* instance;

		int open();
		int close();
		void setupTables();

		std::string filename;
		sqlite3* connection;

		std::map<int, std::shared_ptr<Series>> seriesCache;
		std::map<int, std::shared_ptr<Season>> seasonCache;
		std::map<int, std::shared_ptr<Episode>> episodeCache;
};