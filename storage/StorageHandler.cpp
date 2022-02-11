#include "StorageHandler.h"

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
    const char* sql;
    this->open();

    sql =
        "CREATE TABLE IF NOT EXISTS Series ("
        "id                       INT PRIMARY KEY NOT NULL,"
        "name                     TEXT NOT NULL,"
        "tvdb_name                TEXT NOT NULL UNIQUE,"
        "air_rhythm               TEXT,"
        "first_aired_date         TEXT,"
        "first_aired_broadcaster  TEXT"
        ");";
    sqlite3_exec(this->connection, sql, 0, 0, 0);

    sql = "PRAGMA foreign_keys = ON;"
        "CREATE TABLE IF NOT EXISTS Season ("
        "id             INT NOT NULL,"
        "name           TEXT,"
        "series_id      INT NOT NULL,"
        "FOREIGN KEY (series_id) REFERENCES Series(id) ON DELETE CASCADE ON UPDATE CASCADE,"
        "PRIMARY KEY (id, series_id)"
        ");";
    sqlite3_exec(this->connection, sql, 0, 0, 0);

    sql = "PRAGMA foreign_keys = ON;"
        "CREATE TABLE IF NOT EXISTS Episode ("
        "id                         INT NOT NULL,"
        "name                       TEXT,"
        "absolute                   INT,"
        "runtime                    INT,"
        "first_aired_date           TEXT,"
        "first_aired_broadcaster    TEXT,"
        "tvdb_url                   TEXT,"
        "season_id                  INT NOT NULL,"
        "FOREIGN KEY (season_id) REFERENCES Season (id) ON DELETE CASCADE ON UPDATE CASCADE," 
        "PRIMARY KEY (id, season_id)"
        ");";
    sqlite3_exec(this->connection, sql, 0, 0, 0);

    this->close();
    return;
}

void StorageHandler::addSeries(Series& data) {
    this->open();

    char* err;

    std::string sql;
    sql += "INSERT INTO Series(id, name, tvdb_name, air_rhythm, first_aired_date, first_aired_broadcaster)";
    sql += "VALUES(";
    sql += std::to_string(data.getId()) + ",";
    sql += "'" + data.getName() + "',";
    sql += "'" + data.getTVDBName() + "',";
    sql += "'" + data.getAirRhythm() + "',";
    sql += "'" + data.getFirstAiredDate() + "',";
    sql += "'" + data.getFirstAiredBroadcaster() + "'";
    sql += ");";
    int rc = sqlite3_exec(this->connection, sql.c_str(), 0, 0, &err);

    this->close();
}

void StorageHandler::addSeason(Season& data) {
    this->open();

    std::string sql;
    sql += "INSERT INTO Season(id, name, series_id)";
    sql += "VALUES(";
    sql += std::to_string(data.getId()) + ",";
    sql += "'" + data.getName() + "',";
    sql += std::to_string(data.getSeries()->getId());
    sql += ");";
    sqlite3_exec(this->connection, sql.c_str(), 0, 0, 0);

    this->close();
}

void StorageHandler::addEpisode(Episode& data) {
    this->open();

    std::string sql;
    sql += "INSERT INTO Episode(id, name, absolute, runtime, first_aired_date, first_aired_broadcaster, tvdb_url, season_id)";
    sql += "VALUES(";
    sql += std::to_string(data.getId()) + ",";
    sql += "'" + data.getName() + "',";
    sql += std::to_string(data.getAbsolute()) + "',";
    sql += std::to_string(data.getRuntime()) + "',";
    sql += "'" + data.getFirstAiredDate() + "',";
    sql += "'" + data.getFirstAiredBroadcaster() + "',";
    sql += "'" + data.getUrlTVDB() + "',";
    sql += std::to_string(data.getSeason()->getId());
    sql += ");";
    sqlite3_exec(this->connection, sql.c_str(), 0, 0, 0);

    this->close();
}