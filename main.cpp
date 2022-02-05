#include "QAnime3.h"
#include <QtWidgets/QApplication>
#include <curl/curl.h>
#include "tvdb/TVDBHandler.h"

int main(int argc, char *argv[])
{
    curl_global_init(CURL_GLOBAL_ALL);
    TVDBHandler* tvdb = TVDBHandler::getInstance();

    Season* season = tvdb->getSeasonData("one-piece", 21);
    QApplication a(argc, argv);
    QAnime3 w;
    w.show();
    return a.exec();
}