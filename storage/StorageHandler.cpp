#include "StorageHandler.h"

SqlStatement::SqlStatement(SQL_COMMAND command, std::string table) {
    this->type = command;
    switch (command) {
    case SQL_COMMAND::CREATE_TABLE:
        this->statement << "PRAGMA foreign_keys = ON; CREATE TABLE IF NOT EXISTS " << table << "(\n";
        break;
    case SQL_COMMAND::INSERT:
        this->statement << "INSERT INTO " << table << "(\n";
        break;
    case SQL_COMMAND::UPDATE:
        this->statement << "UPDATE " << table << "\n";
        break;
    case SQL_COMMAND::DELETE:
        this->statement << "DELETE FROM " << table << "\n";
        break;
    case SQL_COMMAND::SELECT:
        this->statement << "SELECT * FROM " << table;
        break;
    default:
        break;
    }
}

// inserts items' values into SqlStatement items.
void SqlStatement::addItems(std::map<std::string, std::string> items) {
    for (auto const& [column, value] : items) {
        this->addItem(column, value);
    }
}

void SqlStatement::addItem(std::string name, std::string value) {
    this->items[name] = value;
}

void SqlStatement::addItem(std::string name, int value) {
    this->items[name] = std::to_string(value);
}

// inserts items' values into SqlStatement items.
void SqlStatement::addConditions(std::map<std::string, std::string> conditions) {
    for (auto const& [column, value] : conditions) {
        this->addCondition(column, value);
    }
}

void SqlStatement::addCondition(std::string column, std::string value) {
    this->conditions[column] = value;
}

void SqlStatement::addCondition(std::string column, int value) {
    this->conditions[column] = std::to_string(value);
}

std::string SqlStatement::buildSql() {
    switch (this->type)
    {
    case SQL_COMMAND::CREATE_TABLE:
        {
            std::string delimiter = "";
            for (auto const& [column, value] : this->items) {   // C++17
                this->statement << delimiter << column << " " << value;
                delimiter = ",\n";
            }
            this->statement << ");";
        }
        break;
    case SQL_COMMAND::INSERT:
        {
            std::string columns;
            std::string values = "VALUES(";
            std::string delimiter = "";
            for (auto const& [column, value] : this->items) {
                columns += delimiter + column;
                values += delimiter + value;
                delimiter = ",";
            }
            columns += ")";
            values += ")";
            this->statement << columns << "\n" << values << ";";
        }
        break;
    case SQL_COMMAND::UPDATE:
        {
            this->statement << "SET ";
            std::string delimiter = "";
            for (auto const& [column, value] : this->items) {
                this->statement << delimiter << column << " = " << value;
                delimiter = ",\n";
            }
            if (this->conditions.size() > 0) {
                this->statement << " WHERE ";
                delimiter = "";
                for (auto const& [column, value] : this->conditions) {
                    this->statement << delimiter << column << " = " << value;
                    delimiter = "\nAND ";
                }
            }
            this->statement << ";";
        }
    break;
    case SQL_COMMAND::DELETE:
        {
            if (this->conditions.size() > 0) {
                this->statement << " WHERE ";
                std::string delimiter = "";
                for (auto const& [column, value] : this->conditions) {
                    this->statement << delimiter << column << " = " << value;
                    delimiter = "\nAND ";
                }
            }
            this->statement << ";";
        }
        break;
    case SQL_COMMAND::SELECT:
    {
        if (this->conditions.size() > 0) {
            this->statement << " WHERE ";
            std::string delimiter = "";
            for (auto const& [column, value] : this->conditions) {
                this->statement << delimiter << column << " = " << value;
                delimiter = "\nAND ";
            }
        }
        this->statement << ";";
    }
    break;
    default:
        break;
    }

    return this->statement.str();
}

// invoked for each result row coming out of evaluated SQL statements
static int callback(void* results, int resultColumnCount, char** columnValues, char** columnNames) {
    std::map<std::string, std::string> rows;
    for (int i = 0; i < resultColumnCount; i++) {
        rows[columnNames[i]] = columnValues[i] ? columnValues[i] : "NULL";
    }
    static_cast<std::vector<std::map<std::string, std::string>>*>(results)->push_back(rows);

    // if this returns non-zero, the sqlite3_exec() routine returns SQLITE_ABORT without
    // invoking the callback again and without running any subsequent SQL statements.
    return 0;
}

// initialize pointer for first getInstance call
StorageHandler* StorageHandler::instance = 0;

StorageHandler::StorageHandler(const std::string& filename) {
    this->filename = filename;
    this->setupTables();
}

StorageHandler* StorageHandler::createInstance(const std::string& filename) {
    if (instance) delete instance;
    instance = new StorageHandler(filename);
    return instance;
}

StorageHandler* StorageHandler::getInstance() {
    if (!instance)
        throw "StorageHandler has not yet been initialized";
    return instance;
}

std::shared_ptr<Series> StorageHandler::cache(const Series& series) {
    int primaryKey = series.getId();
    this->seriesCache[primaryKey] = series;
    return std::make_shared<Series>(this->seriesCache[primaryKey]); // TODO: what happens to old pointers after replacing map values?
}

std::shared_ptr<Season> StorageHandler::cache(const Season& season) {
    std::tuple<int, int> primaryKey(season.getSeries()->getId(), season.getId());
    this->seasonCache[primaryKey] = season;
    return std::make_shared<Season>(this->seasonCache[primaryKey]);
}

std::shared_ptr<Episode> StorageHandler::cache(const Episode& episode) {
    std::tuple<int, int, int> primaryKey(
        episode.getSeason()->getSeries()->getId(),
        episode.getSeason()->getId(),
        episode.getId()
    );
    this->episodeCache[primaryKey] = episode;
    return std::make_shared<Episode>(this->episodeCache[primaryKey]);
}

// maps sql results to Series object. does *not* correctly reverse a object-to-map mapping
template <>
auto StorageHandler::map<Series>(const std::map<std::string, std::string>& from) {
    auto ptr = std::make_shared<Series>();
    ptr->setId(from.at("id"));
    ptr->setName(from.at("name"));
    ptr->setAirRhythm(from.at("air_rhythm"));
    ptr->setFirstAiredDate(from.at("first_aired_date"));
    ptr->setFirstAiredBroadcaster(from.at("first_aired_broadcaster"));
    return ptr;
}

// maps sql results to Season object. does *not* correctly reverse a object-to-map mapping
template <>
auto StorageHandler::map<Season>(const std::map<std::string, std::string>& from) {
    auto ptr = std::make_shared<Season>();
    ptr->setId(from.at("id"));
    //ptr->setSeries(from.at("series_id")); // todo: handle this
    ptr->setName(from.at("name"));
    return ptr;
}

// maps sql results to Episode object. does *not* correctly reverse a object-to-map mapping
template <>
auto StorageHandler::map<Episode>(const std::map<std::string, std::string>& from) {
    int series_id = std::stoi(from.at("series_id"));
    int season_id = std::stoi(from.at("season_id"));
    //auto series = this->getSeriesByPK(series_id);
    auto season = this->getSeasonByPK(series_id, season_id);
    Episode ep;
    ep.setId(from.at("id"));
    ep.setSeason(*season);
    //ep.getSeason().setSeries(from.at("series_id"); // todo: handle this
    ep.setName(from.at("name"));
    ep.setAbsolute(from.at("absolute"));
    ep.setRuntime(from.at("runtime"));
    ep.setFirstAiredDate(from.at("first_aired_date"));
    ep.setFirstAiredBroadcaster(from.at("first_aired_broadcaster"));
    ep.setTVDBUrl(from.at("tvdb_url"));
    return ep;
}

// maps Series object to map of sql-formatted strings.
std::map<std::string, std::string> StorageHandler::map(const Series& from) {
    std::map<std::string, std::string> map;
    map["id"] = from.getId();
    map["name"] = "'" + from.getName() + "'";
    map["air_rhythm"] = "'" + from.getAirRhythm() + "'";
    map["first_aired_date"] = "'" + from.getFirstAiredDate() + "'";
    map["first_aired_broadcaster"] = "'" + from.getFirstAiredBroadcaster() + "'";
    return map;
}

// maps Season object to map of sql-formatted strings.
std::map<std::string, std::string> StorageHandler::map(const Season& from) {
    std::map<std::string, std::string> map;
    map["id"] = from.getId();
    map["series_id"] = from.getSeries()->getId();
    map["name"] = "'" + from.getName() + "'";
    return map;
}

// maps Episode object to map of sql-formatted strings.
std::map<std::string, std::string> StorageHandler::map(const Episode& from) {
    std::map<std::string, std::string> map;
    map["id"] = from.getId();
    map["season_id"] = from.getSeason()->getId();
    map["series_id"] = from.getSeason()->getSeries()->getId();
    map["name"] = "'" + from.getName() + "'";
    map["absolute"] = from.getAbsolute();
    map["runtime"] = from.getRuntime();
    map["first_aired_date"] = "'" + from.getFirstAiredDate() + "'";
    map["first_aired_broadcaster"] = "'" + from.getFirstAiredBroadcaster() + "'";
    map["tvdb_url"] = "'" + from.getTVDBUrl() + "'";
    return map;
}

int StorageHandler::open() {
    return sqlite3_open(this->filename.c_str(), &this->connection);
}

int StorageHandler::close() {
    return sqlite3_close(this->connection);
}

void StorageHandler::setupTables() {
    std::string sql;
    this->open();

    SqlStatement statementSeries(SQL_COMMAND::CREATE_TABLE, "Series");
    statementSeries.addItem("id", "INT PRIMARY KEY NOT NULL");
    statementSeries.addItem("name", "TEXT NOT NULL");
    statementSeries.addItem("tvdb_name", "TEXT NOT NULL UNIQUE");
    statementSeries.addItem("air_rhythm", "TEXT");
    statementSeries.addItem("first_aired_date", "TEXT");
    statementSeries.addItem("first_aired_broadcaster", "TEXT");
    sql = statementSeries.buildSql();
    sqlite3_exec(this->connection, sql.c_str(), 0, 0, 0);

    SqlStatement statementSeason(SQL_COMMAND::CREATE_TABLE, "Season");
    statementSeason.addItem("id", "INT NOT NULL");
    statementSeason.addItem("series_id", "INT NOT NULL");
    statementSeason.addItem("name", "TEXT");
    statementSeason.addItem("FOREIGN KEY (series_id) REFERENCES Series(id)", "ON DELETE CASCADE ON UPDATE CASCADE");
    statementSeason.addItem("PRIMARY KEY (id, series_id)", "");
    sql = statementSeason.buildSql();
    sqlite3_exec(this->connection, sql.c_str(), 0, 0, 0);

    SqlStatement statementEpisode(SQL_COMMAND::CREATE_TABLE, "Episode");
    statementEpisode.addItem("id", "INT NOT NULL");
    statementEpisode.addItem("season_id", "INT NOT NULL");
    statementEpisode.addItem("series_id", "INT NOT NULL");
    statementEpisode.addItem("name", "TEXT");
    statementEpisode.addItem("absolute", "INT");
    statementEpisode.addItem("runtime", "INT");
    statementEpisode.addItem("first_aired_date", "TEXT");
    statementEpisode.addItem("first_aired_broadcaster", "TEXT");
    statementEpisode.addItem("tvdb_url", "TEXT");
    statementEpisode.addItem("FOREIGN KEY (season_id) REFERENCES Season (id)", "ON DELETE CASCADE ON UPDATE CASCADE");
    statementEpisode.addItem("FOREIGN KEY (series_id) REFERENCES Series (id)", "ON DELETE CASCADE ON UPDATE CASCADE");
    statementEpisode.addItem("PRIMARY KEY (id, season_id, series_id)", "");
    sql = statementEpisode.buildSql();
    sqlite3_exec(this->connection, sql.c_str(), 0, 0, 0);

    this->close();
    return;
}

std::shared_ptr<Series> StorageHandler::addSeries(const Series& series) {
    // map object attributes
    std::map<std::string, std::string> items = this->map(series);

    // create sql statement
    SqlStatement statement(SQL_COMMAND::INSERT, "Series");
    statement.addItems(items);
    std::string sql = statement.buildSql();

    // execute sql statement
    char* err;
    this->open();
    int rc = sqlite3_exec(this->connection, sql.c_str(), 0, 0, &err);
    this->close();

    // put data into cache and return pointer to cached data
    auto seriesPtr = std::make_shared<Series>(series);
    seriesPtr = cache(*seriesPtr);
    return seriesPtr;
}

std::shared_ptr<Series> StorageHandler::updateSeries(const Series& series) {
    // map object attributes
    std::map<std::string, std::string> items = this->map(series);

    // create sql statement
    SqlStatement statement(SQL_COMMAND::UPDATE, "Series");
    statement.addItems(items);
    statement.addCondition("id", series.getId());
    std::string sql = statement.buildSql();

    // execute sql statement
    char* err;
    this->open();
    int rc = sqlite3_exec(this->connection, sql.c_str(), 0, 0, &err);
    this->close();

    // put data into cache and return pointer to cached data
    auto seriesPtr = std::make_shared<Series>(series);
    seriesPtr = cache(*seriesPtr);
    return seriesPtr;
}

void StorageHandler::deleteSeries(const Series& series) {
    // create sql statement
    SqlStatement statement(SQL_COMMAND::DELETE, "Series");
    statement.addCondition("id", series.getId());
    std::string sql = statement.buildSql();

    // execute sql statement
    this->open();
    char* err;
    int rc = sqlite3_exec(this->connection, sql.c_str(), 0, 0, &err);
    this->close();
}

std::map<int, Series> StorageHandler::getSeriesAll() {
    // create sql statement
    SqlStatement statement(SQL_COMMAND::SELECT, "Series");
    std::string sql = statement.buildSql();

    // execute sql statement
    std::vector<std::map<std::string, std::string>> results;
    this->open();
    char* err;
    int rc = sqlite3_exec(this->connection, sql.c_str(), callback, &results, &err);
    this->close();

    // exactly one result expected
    if (results.size() == 0)
        throw "No Series found!";

    // map results to object
    for(std::map<std::string, std::string>& result : results) {
        auto seriesPtr = this->map<Series>(result);
        cache(*seriesPtr);
    }

    return this->seriesCache;
}

std::shared_ptr<Series> StorageHandler::getSeriesByPK(int id) {
    if(this->seriesCache.count(id))
        return std::make_shared<Series>(this->seriesCache[id]);

    // create sql statement
    SqlStatement statement(SQL_COMMAND::SELECT, "Series");
    statement.addCondition("id", id);
    std::string sql = statement.buildSql();

    // execute sql statement
    std::vector<std::map<std::string, std::string>> results;
    this->open();
    char* err;
    int rc = sqlite3_exec(this->connection, sql.c_str(), callback, &results, &err);
    this->close();

    // exactly one result expected
    if (results.size() == 0)
        throw "Series not found for this id!";
    else if (results.size() > 1)
        throw "Multiple Series found for this id!";

    // map results to object
    std::map<std::string, std::string> result = results.at(0);

    // put data into cache and return pointer to cached data
    auto seriesPtr = this->map<Series>(result);
    seriesPtr = cache(*seriesPtr);
    return seriesPtr;
}

std::shared_ptr<Season> StorageHandler::addSeason(const Season& season) {
    // map object attributes
    std::map<std::string, std::string> items = this->map(season);

    // create sql statement
    SqlStatement statement(SQL_COMMAND::INSERT, "Season");
    statement.addItems(items);
    std::string sql = statement.buildSql();

    // execute sql statement
    this->open();
    char* err;
    int rc = sqlite3_exec(this->connection, sql.c_str(), 0, 0, &err);
    this->close();

    // put data into cache and return pointer to cached data
    auto seasonPtr = std::make_shared<Season>(season);
    seasonPtr = cache(*seasonPtr);
    return seasonPtr;
}

std::shared_ptr<Season> StorageHandler::updateSeason(const Season& season) {    
    // map object attributes
    std::map<std::string, std::string> items = this->map(season);

    // create sql statement
    SqlStatement statement(SQL_COMMAND::UPDATE, "Season");
    statement.addItems(items);
    statement.addCondition("id", season.getId());
    statement.addCondition("series_id", season.getSeries()->getId());
    std::string sql = statement.buildSql();

    // execute sql statement
    this->open();
    char* err;
    int rc = sqlite3_exec(this->connection, sql.c_str(), 0, 0, &err);
    this->close();

    // put data into cache and return pointer to cached data
    auto seasonPtr = std::make_shared<Season>(season);
    seasonPtr = cache(*seasonPtr);
    return seasonPtr;
}

void StorageHandler::deleteSeason(const Season& season) {
    // create sql statement
    SqlStatement statement(SQL_COMMAND::DELETE, "Season");
    statement.addCondition("id", season.getId());
    statement.addCondition("series_id", season.getSeries()->getId());
    std::string sql = statement.buildSql();

    // execute sql statement
    this->open();
    char* err;
    int rc = sqlite3_exec(this->connection, sql.c_str(), 0, 0, &err);
    this->close();
}

std::shared_ptr<Season> StorageHandler::getSeasonByPK(int series_id, int season_id) {
    std::tuple<int, int> pimaryKey(series_id, season_id);
    if(this->seasonCache.count(pimaryKey))
        return std::make_shared<Season>(this->seasonCache[pimaryKey]);

    // create sql statement
    SqlStatement statement(SQL_COMMAND::SELECT, "Season");
    statement.addCondition("id", season_id);
    statement.addCondition("series_id", series_id);
    std::string sql = statement.buildSql();

    // execute sql statement
    std::vector<std::map<std::string, std::string>> results;
    this->open();
    char* err;
    int rc = sqlite3_exec(this->connection, sql.c_str(), callback, &results, &err);
    this->close();

    // exactly one result expected
    if (results.size() == 0)
        throw "Season not found for this id!";
    else if (results.size() > 1)
        throw "Multiple Seasons found for this id!";

    // map results to object
    std::map<std::string, std::string> result = results.at(0);

    // put data into cache and return pointer to cached data
    auto seasonPtr = this->map<Season>(result);
    seasonPtr = cache(*seasonPtr);
    return seasonPtr;
}

std::shared_ptr<Episode> StorageHandler::addEpisode(const Episode& episode) {
    // map object attributes
    std::map<std::string, std::string> items = this->map(episode);

    // create sql statement
    SqlStatement statement(SQL_COMMAND::INSERT, "Episode");
    statement.addItems(items);
    std::string sql = statement.buildSql();

    // execute sql statement
    this->open();
    char* err;
    sqlite3_exec(this->connection, sql.c_str(), 0, 0, &err);
    this->close();

    // put data into cache and return pointer to cached data
    auto episodePtr = std::make_shared<Episode>(episode);
    episodePtr = cache(*episodePtr);
    return episodePtr;
}

std::shared_ptr<Episode> StorageHandler::updateEpisode(const Episode& episode) {
    // map object attributes
    std::map<std::string, std::string> items = this->map(episode);

    // create sql statement
    SqlStatement statement(SQL_COMMAND::UPDATE, "Episode");
    statement.addItems(items);
    statement.addCondition("id", episode.getId());
    statement.addCondition("season_id", episode.getSeason()->getId());
    statement.addCondition("series_id", episode.getSeason()->getSeries()->getId());
    std::string sql = statement.buildSql();

    // execute sql statement
    this->open();
    char* err;
    sqlite3_exec(this->connection, sql.c_str(), 0, 0, &err);
    this->close();

    // put data into cache and return pointer to cached data
    auto episodePtr = std::make_shared<Episode>(episode);
    episodePtr = cache(*episodePtr);
    return episodePtr;
}

void StorageHandler::deleteEpisode(const Episode& episode) {
    // create sql statement
    SqlStatement statement(SQL_COMMAND::DELETE, "Episode");
    statement.addCondition("id", episode.getId());
    statement.addCondition("season_id", episode.getSeason()->getId());
    statement.addCondition("series_id", episode.getSeason()->getSeries()->getId());
    std::string sql = statement.buildSql();

    // execute sql statement
    this->open();
    char* err;
    sqlite3_exec(this->connection, sql.c_str(), 0, 0, &err);
    this->close();
}