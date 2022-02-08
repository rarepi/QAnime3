#include <fstream>
#include <curl/curl.h>
#include <json/json.h>

#include <QtWidgets/QApplication>
#include "QAnime3.h"

#include "tvdb/TVDBHandler.h"
#include "qbt/QBTHandler.h"

static Json::Value readSettings() {
    // read settings file into string
    std::ifstream settingsFile{"settings.json"};
    if (!settingsFile.is_open())
        throw "Unable to open settings file";
    std::stringstream buffer;
    buffer << settingsFile.rdbuf();
    std::string settingsStr = buffer.str();

    // parse settings json
    Json::CharReaderBuilder builder;
    Json::CharReader* reader = builder.newCharReader();
    Json::Value settings;
    bool parsingSuccessful = reader->parse(settingsStr.c_str(), settingsStr.c_str() + settingsStr.size(), &settings, nullptr);
    delete reader;
    return settings;
}

int main(int argc, char *argv[]) {
    curl_global_init(CURL_GLOBAL_ALL);

    Json::Value settings = readSettings();

    TVDBHandler* tvdb = TVDBHandler::createInstance(
        settings["tvdb_url"].asString(), settings["tvdb_cache"].asString()
    );
    QBTHandler* qbt = QBTHandler::createInstance(
        settings["qbt_url"].asString(), settings["qbt_username"].asString(), settings["qbt_password"].asString()
    );

    Series* series = tvdb->getSeriesData("one-piece");
    Season* season = tvdb->getSeasonData(*series, 21);
    std::string test = qbt->getAppVersion();
    Json::Value test2 = qbt->getTorrentList();
    //qbt->renameFile("db0ffe8174317b0b0ee4beb7b54f558bb9089746", "test.txt", "test_new.txt");
    QApplication a(argc, argv);
    QAnime3 w;
    w.show();
    return a.exec();
}