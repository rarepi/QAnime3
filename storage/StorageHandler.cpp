#include "StorageHandler.h"

SqlItem::SqlItem(std::string first, std::string second) {
    this->first = first;
    this->second = second;
}

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
    default:
        break;
    }
}

void SqlStatement::addItem(std::string name, std::string value) {
    switch (this->type)
    {
    case SQL_COMMAND::CREATE_TABLE:
        this->items.push_back(SqlItem(name, value));
        break;
    case SQL_COMMAND::INSERT:
    case SQL_COMMAND::UPDATE:
        this->items.push_back(SqlItem(name, "'" + value + "'"));
        break;
    case SQL_COMMAND::DELETE:
    default:
        break;
    }
}

void SqlStatement::addItem(std::string name, int value) {
    switch (this->type)
    {
    case SQL_COMMAND::CREATE_TABLE: // useless? CREATE_TABLE has no integer values, right?
        this->items.push_back(SqlItem(name, std::to_string(value)));
        break;
    case SQL_COMMAND::INSERT:
    case SQL_COMMAND::UPDATE:
        this->items.push_back(SqlItem(name, "'" + std::to_string(value) + "'"));
        break;
    case SQL_COMMAND::DELETE:
    default:
        break;
    }
}

void SqlStatement::addCondition(std::string column, std::string value) {
    switch (this->type)
    {
    case SQL_COMMAND::INSERT:
    case SQL_COMMAND::UPDATE:
    case SQL_COMMAND::DELETE:
        this->conditions.push_back(SqlItem(column, "'" + value + "'"));
        break;
    case SQL_COMMAND::CREATE_TABLE:
    default:
        break;
    }
}

void SqlStatement::addCondition(std::string column, int value) {
    switch (this->type)
    {
    case SQL_COMMAND::INSERT:
    case SQL_COMMAND::UPDATE:
    case SQL_COMMAND::DELETE:
        this->conditions.push_back(SqlItem(column, "'" + std::to_string(value) + "'"));
        break;
    case SQL_COMMAND::CREATE_TABLE:
    default:
        break;
    }
}

std::string SqlStatement::buildSql() {
    switch (this->type)
    {
    case SQL_COMMAND::CREATE_TABLE:
        {
            std::string delimiter = "";
            for (SqlItem item : this->items) {
                this->statement << delimiter << item.first << " " << item.second;
                delimiter = ",\n";
            }
            this->statement << ");";
        }
        break;
    case SQL_COMMAND::INSERT:
        {
            std::string names;
            std::string values = "VALUES(";
            std::string delimiter = "";
            for (SqlItem item : this->items) {
                names += delimiter + item.first;
                values += delimiter + item.second;
                delimiter = ",";
            }
            names += ")";
            values += ")";
            this->statement << names << "\n" << values << ";";
        }
        break;
    case SQL_COMMAND::UPDATE:
        {
            this->statement << "SET ";
            std::string delimiter = "";
            for (SqlItem item : this->items) {
                this->statement << delimiter << item.first << " = " << item.second;
                delimiter = ",\n";
            }
            this->statement << " WHERE ";
            delimiter = "";
            for (SqlItem condition : this->conditions) {
                this->statement << delimiter << condition.first << " = " << condition.second;
                delimiter = "\nAND ";
            }
            this->statement << ";";
        }
    break;
    case SQL_COMMAND::DELETE:
        {
            this->statement << " WHERE ";
            std::string delimiter = "";
            for (SqlItem condition : this->conditions) {
                this->statement << delimiter << condition.first << " = " << condition.second;
                delimiter = "\nAND ";
            }
        }
        break;
    default:
        break;
    }

    return this->statement.str();
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
    statementSeason.addItem("name", "TEXT");
    statementSeason.addItem("series_id", "INT NOT NULL");
    statementSeason.addItem("FOREIGN KEY (series_id) REFERENCES Series(id)", "ON DELETE CASCADE ON UPDATE CASCADE");
    statementSeason.addItem("PRIMARY KEY (id, series_id)", "");
    sql = statementSeason.buildSql();
    sqlite3_exec(this->connection, sql.c_str(), 0, 0, 0);

    SqlStatement statementEpisode(SQL_COMMAND::CREATE_TABLE, "Episode");
    statementEpisode.addItem("id", "INT NOT NULL");
    statementEpisode.addItem("name", "TEXT");
    statementEpisode.addItem("absolute", "INT");
    statementEpisode.addItem("runtime", "INT");
    statementEpisode.addItem("first_aired_date", "TEXT");
    statementEpisode.addItem("first_aired_broadcaster", "TEXT");
    statementEpisode.addItem("tvdb_url", "TEXT");
    statementEpisode.addItem("season_id", "INT NOT NULL");
    statementEpisode.addItem("FOREIGN KEY (season_id) REFERENCES Season (id)", "ON DELETE CASCADE ON UPDATE CASCADE");
    statementEpisode.addItem("PRIMARY KEY (id, season_id)", "");
    sql = statementEpisode.buildSql();
    sqlite3_exec(this->connection, sql.c_str(), 0, 0, 0);

    this->close();
    return;
}

void StorageHandler::addSeries(Series& series) {
    this->open();

    char* err;

    SqlStatement statement(SQL_COMMAND::INSERT, "Series");
    statement.addItem("id", series.getId());
    statement.addItem("name", series.getName());
    statement.addItem("tvdb_name", series.getTVDBName());
    statement.addItem("air_rhythm", series.getAirRhythm());
    statement.addItem("first_aired_date", series.getFirstAiredDate());
    statement.addItem("first_aired_broadcaster", series.getFirstAiredBroadcaster());
    std::string sql = statement.buildSql();
    int rc = sqlite3_exec(this->connection, sql.c_str(), 0, 0, &err);

    this->close();
}

void StorageHandler::updateSeries(Series& series) {
    this->open();

    char* err;

    SqlStatement statement(SQL_COMMAND::UPDATE, "Series");
    statement.addItem("id", series.getId());
    statement.addItem("name", series.getName());
    statement.addItem("tvdb_name", series.getTVDBName());
    statement.addItem("air_rhythm", series.getAirRhythm());
    statement.addItem("first_aired_date", series.getFirstAiredDate());
    statement.addItem("first_aired_broadcaster", series.getFirstAiredBroadcaster());
    statement.addCondition("id", series.getId());
    std::string sql = statement.buildSql();
    int rc = sqlite3_exec(this->connection, sql.c_str(), 0, 0, &err);

    this->close();
}

void StorageHandler::deleteSeries(Series& series) {
    this->open();

    char* err;

    SqlStatement statement(SQL_COMMAND::DELETE, "Series");
    statement.addCondition("id", series.getId());
    std::string sql = statement.buildSql();
    int rc = sqlite3_exec(this->connection, sql.c_str(), 0, 0, &err);

    this->close();
}

void StorageHandler::addSeason(Season& season) {
    this->open();

    char* err;

    SqlStatement statement(SQL_COMMAND::INSERT, "Season");
    statement.addItem("id", season.getId());
    statement.addItem("name", season.getName());
    statement.addItem("series_id", season.getSeries()->getId());
    std::string sql = statement.buildSql();
    int rc = sqlite3_exec(this->connection, sql.c_str(), 0, 0, &err);

    this->close();
}

void StorageHandler::updateSeason(Season& season)
{
    this->open();

    char* err;

    SqlStatement statement(SQL_COMMAND::UPDATE, "Season");
    statement.addItem("id", season.getId());
    statement.addItem("name", season.getName());
    statement.addItem("series_id", season.getSeries()->getId());
    statement.addCondition("id", season.getId());
    statement.addCondition("series_id", season.getSeries()->getId());
    std::string sql = statement.buildSql();
    int rc = sqlite3_exec(this->connection, sql.c_str(), 0, 0, &err);

    this->close();
}

void StorageHandler::deleteSeason(Season& season)
{
    this->open();

    char* err;

    SqlStatement statement(SQL_COMMAND::DELETE, "Season");
    statement.addCondition("id", season.getId());
    statement.addCondition("series_id", season.getSeries()->getId());
    std::string sql = statement.buildSql();
    int rc = sqlite3_exec(this->connection, sql.c_str(), 0, 0, &err);

    this->close();
}

void StorageHandler::addEpisode(Episode& episode) {
    this->open();

    char* err;

    SqlStatement statement(SQL_COMMAND::INSERT, "Episode");
    statement.addItem("id", episode.getId());
    statement.addItem("name", episode.getName());
    statement.addItem("absolute", episode.getAbsolute());
    statement.addItem("runtime", episode.getRuntime());
    statement.addItem("first_aired_date", episode.getFirstAiredDate());
    statement.addItem("first_aired_broadcaster", episode.getFirstAiredBroadcaster());
    statement.addItem("tvdb_url", episode.getTVDBUrl());
    statement.addItem("season_id", episode.getSeason()->getId());
    std::string sql = statement.buildSql();
    sqlite3_exec(this->connection, sql.c_str(), 0, 0, &err);

    this->close();
}

void StorageHandler::updateEpisode(Episode& episode)
{
    this->open();

    char* err;

    SqlStatement statement(SQL_COMMAND::UPDATE, "Episode");
    statement.addItem("id", episode.getId());
    statement.addItem("name", episode.getName());
    statement.addItem("absolute", episode.getAbsolute());
    statement.addItem("runtime", episode.getRuntime());
    statement.addItem("first_aired_date", episode.getFirstAiredDate());
    statement.addItem("first_aired_broadcaster", episode.getFirstAiredBroadcaster());
    statement.addItem("tvdb_url", episode.getTVDBUrl());
    statement.addItem("season_id", episode.getSeason()->getId());
    statement.addCondition("id", episode.getId());
    statement.addCondition("season_id", episode.getSeason()->getId());
    std::string sql = statement.buildSql();
    sqlite3_exec(this->connection, sql.c_str(), 0, 0, &err);

    this->close();
}

void StorageHandler::deleteEpisode(Episode& episode)
{
    this->open();

    char* err;

    SqlStatement statement(SQL_COMMAND::DELETE, "Episode");
    statement.addCondition("id", episode.getId());
    statement.addCondition("season_id", episode.getSeason()->getId());
    std::string sql = statement.buildSql();
    sqlite3_exec(this->connection, sql.c_str(), 0, 0, &err);

    this->close();
}