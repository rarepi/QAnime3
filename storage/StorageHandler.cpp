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

// maps sql results to Series object. does *not* correctly reverse a object-to-map mapping
void StorageHandler::map(const std::map<std::string, std::string>& from, Series& to) {
    to.setId(from.at("id"));
    to.setName(from.at("name"));
    to.setAirRhythm(from.at("air_rhythm"));
    to.setFirstAiredDate(from.at("first_aired_date"));
    to.setFirstAiredBroadcaster(from.at("first_aired_broadcaster"));
}

// maps Series object to map of sql-formatted strings.
void StorageHandler::map(const Series& from, std::map<std::string, std::string>& to) {
    to["id"] = from.getId();
    to["name"] = "'" + from.getName() + "'";
    to["air_rhythm"] = "'" + from.getAirRhythm() + "'";
    to["first_aired_date"] = "'" + from.getFirstAiredDate() + "'";
    to["first_aired_broadcaster"] = "'" + from.getFirstAiredBroadcaster() + "'";
}

// maps sql results to Season object. does *not* correctly reverse a object-to-map mapping
void StorageHandler::map(const std::map<std::string, std::string>& from, Season& to) {
    to.setId(from.at("id"));
    //to.setSeries(from.at("series_id")); // todo: handle this
    to.setName(from.at("name"));
}

// maps Season object to map of sql-formatted strings.
void StorageHandler::map(const Season& from, std::map<std::string, std::string>& to) {
    to["id"] = from.getId();
    to["series_id"] = from.getSeries()->getId();
    to["name"] = "'" + from.getName() + "'";
}

// maps sql results to Episode object. does *not* correctly reverse a object-to-map mapping
void StorageHandler::map(const std::map<std::string, std::string>& from, Episode& to) {
    to.setId(from.at("id"));
    //to.setSeason(from.at("season_id"); // todo: handle this
    //to.getSeason().setSeries(from.at("series_id"); // todo: handle this
    to.setName(from.at("name"));
    to.setAbsolute(from.at("absolute"));
    to.setRuntime(from.at("runtime"));
    to.setFirstAiredDate(from.at("first_aired_date"));
    to.setFirstAiredBroadcaster(from.at("first_aired_broadcaster"));
    to.setTVDBUrl(from.at("tvdb_url"));
}

// maps Episode object to map of sql-formatted strings.
void StorageHandler::map(const Episode& from, std::map<std::string, std::string>& to) {
    to["id"] = from.getId();
    to["season_id"] = from.getSeason()->getId();
    to["series_id"] = from.getSeason()->getSeries()->getId();
    to["name"] = "'" + from.getName() + "'";
    to["absolute"] = from.getAbsolute();
    to["runtime"] = from.getRuntime();
    to["first_aired_date"] = "'" + from.getFirstAiredDate() + "'";
    to["first_aired_broadcaster"] = "'" + from.getFirstAiredBroadcaster() + "'";
    to["tvdb_url"] = "'" + from.getTVDBUrl() + "'";
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
    statementSeason.addItem("series_id", "INT NOT NULL");
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

void StorageHandler::addSeries(Series& series) {
    // map object attributes
    std::map<std::string, std::string> items;
    this->map(series, items);

    // create sql statement
    SqlStatement statement(SQL_COMMAND::INSERT, "Series");
    statement.addItems(items);
    std::string sql = statement.buildSql();

    // execute sql statement
    char* err;
    this->open();
    int rc = sqlite3_exec(this->connection, sql.c_str(), 0, 0, &err);
    this->close();
}

void StorageHandler::updateSeries(Series& series) {
    // map object attributes
    std::map<std::string, std::string> items;
    this->map(series, items);

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
}

void StorageHandler::deleteSeries(Series& series) {
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

void StorageHandler::selectSeriesById(int id) {
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
    Series series;
    this->map(result, series);
    
    // ... TODO

}

void StorageHandler::addSeason(Season& season) {
    // map object attributes
    std::map<std::string, std::string> items;
    this->map(season, items);

    // create sql statement
    SqlStatement statement(SQL_COMMAND::INSERT, "Season");
    statement.addItems(items);
    std::string sql = statement.buildSql();

    // execute sql statement
    this->open();
    char* err;
    int rc = sqlite3_exec(this->connection, sql.c_str(), 0, 0, &err);
    this->close();
}

void StorageHandler::updateSeason(Season& season) {    
    // map object attributes
    std::map<std::string, std::string> items;
    this->map(season, items);

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
}

void StorageHandler::deleteSeason(Season& season) {
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

void StorageHandler::addEpisode(Episode& episode) {
    // map object attributes
    std::map<std::string, std::string> items;
    this->map(episode, items);

    // create sql statement
    SqlStatement statement(SQL_COMMAND::INSERT, "Episode");
    statement.addItems(items);
    std::string sql = statement.buildSql();

    // execute sql statement
    this->open();
    char* err;
    sqlite3_exec(this->connection, sql.c_str(), 0, 0, &err);
    this->close();
}

void StorageHandler::updateEpisode(Episode& episode) {
    // map object attributes
    std::map<std::string, std::string> items;
    this->map(episode, items);

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
}

void StorageHandler::deleteEpisode(Episode& episode) {
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