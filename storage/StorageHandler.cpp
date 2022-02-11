#include "StorageHandler.h"

SqlItem::SqlItem(std::string name, std::string value) {
    this->name = name;
    this->value = value;
}

SqlStatement::SqlStatement(SQL_COMMAND command, std::string argument) {
    this->type = command;
    switch (command) {
    case SQL_COMMAND::CREATE_TABLE:
        this->statement << "PRAGMA foreign_keys = ON; CREATE TABLE IF NOT EXISTS ";
        this->statement << argument << "(";
        break;
    case SQL_COMMAND::INSERT:
        this->statement << "INSERT INTO ";
        this->statement << argument;
        break;
    case SQL_COMMAND::DELETE:
        // TODO
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
        this->items.push_back(SqlItem("'" + name + "'", value));
        break;
    case SQL_COMMAND::DELETE:
        // TODO
        break;
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
        this->items.push_back(SqlItem("'" + name + "'", std::to_string(value)));
        break;
    case SQL_COMMAND::DELETE:
        // TODO
        break;
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
            for (SqlItem item : items) {
                this->statement << delimiter << item.name << " " << item.value;
                delimiter = ",";
            }
            this->statement << ");";
        }
        break;
    case SQL_COMMAND::INSERT:
        {
        std::string names;
        std::string values = "VALUES(";
        std::string delimiter = "";
        for (SqlItem item : items) {
            names += delimiter + item.name;
            values += delimiter + item.value;
            delimiter = ",";
        }
        names += ")";
        values += ")";
        this->statement << names + values + ";";
        }
        break;
    case SQL_COMMAND::DELETE:
        // TODO
        break;
    default:
        break;
    }
    //std::string result = this->statement.str();
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

void StorageHandler::addSeries(Series& data) {
    this->open();

    char* err;

    SqlStatement statement(SQL_COMMAND::INSERT, "Series");
    statement.addItem("id", data.getId());
    statement.addItem("name", data.getName());
    statement.addItem("tvdb_name", data.getTVDBName());
    statement.addItem("air_rhythm", data.getAirRhythm());
    statement.addItem("first_aired_date", data.getFirstAiredDate());
    statement.addItem("first_aired_broadcaster", data.getFirstAiredBroadcaster());
    std::string sql = statement.buildSql();
    int rc = sqlite3_exec(this->connection, sql.c_str(), 0, 0, &err);

    this->close();
}

void StorageHandler::addSeason(Season& data) {
    this->open();

    char* err;

    SqlStatement statement(SQL_COMMAND::INSERT, "Season");
    statement.addItem("id", data.getId());
    statement.addItem("name", data.getName());
    statement.addItem("series_id", data.getSeries()->getId());
    std::string sql = statement.buildSql();
    int rc = sqlite3_exec(this->connection, sql.c_str(), 0, 0, &err);

    this->close();
}

void StorageHandler::addEpisode(Episode& data) {
    this->open();

    char* err;

    SqlStatement statement(SQL_COMMAND::INSERT, "Episode");
    statement.addItem("id", data.getId());
    statement.addItem("name", data.getName());
    statement.addItem("absolute", data.getAbsolute());
    statement.addItem("runtime", data.getRuntime());
    statement.addItem("first_aired_date", data.getFirstAiredDate());
    statement.addItem("first_aired_broadcaster", data.getFirstAiredBroadcaster());
    statement.addItem("tvdb_url", data.getTVDBUrl());
    statement.addItem("season_id", data.getSeason()->getId());
    std::string sql = statement.buildSql();
    sqlite3_exec(this->connection, sql.c_str(), 0, 0, &err);

    this->close();
}