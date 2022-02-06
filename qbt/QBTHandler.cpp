#include "QBTHandler.h"

// Source: https://stackoverflow.com/a/5525631/5920409
static size_t curl_to_string(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realsize = size * nmemb;
    static_cast<std::string*>(userp)->append(static_cast<char*>(contents), realsize);
    return realsize;
}

static Json::Value parseStringToJson(const std::string str) {
    Json::CharReaderBuilder builder;
    Json::CharReader* reader = builder.newCharReader();

    Json::Value json;
    std::string errors;

    bool parsingSuccessful = reader->parse(
        str.c_str(),
        str.c_str() + str.size(),
        &json,
        &errors
    );
    delete reader;

    return json;
}

// initialize pointer for first getInstance call
QBTHandler* QBTHandler::instance = 0;

QBTHandler::QBTHandler(
    const std::string& qbt_url,
    const std::string& username,
    const std::string& password
) {
    this->qbt_url = qbt_url;
    this->username = username;
    this->password = password;
    this->authenticate();
}

QBTHandler* QBTHandler::createInstance(
    const std::string& qbt_url,
    const std::string& username,
    const std::string& password
) {
    if (instance) delete instance;
    instance = new QBTHandler(qbt_url, username, password);
    return instance;
}

QBTHandler* QBTHandler::getInstance() {
    if (!instance)
        throw "QBTHandler has not yet been initialized";
    return instance;
}

void QBTHandler::authenticate() {
    std::string method = "/auth/login";
    std::string params = "username=" + this->username + "&password=" + this->password;
    std::string data;
    std::string header;

    std::string url = this->qbt_url + method;

    CURL* curl_handle;
    CURLcode res;

    curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, params.c_str());
    curl_easy_setopt(curl_handle, CURLOPT_HEADERFUNCTION, curl_to_string);
    curl_easy_setopt(curl_handle, CURLOPT_HEADERDATA, &header);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, curl_to_string);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &data);
    res = curl_easy_perform(curl_handle);

    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }

    curl_easy_cleanup(curl_handle);

    size_t cookiePos = header.find("SID=");
    this->key = header.substr(cookiePos+4, 32);

    return;
}

std::string QBTHandler::getAppVersion() {
    std::string method = "/app/version";
    std::string cookie = "SID=" + this->key + ";";
    std::string data;
    std::string header;
    std::string url = this->qbt_url + method;

    CURL* curl_handle;
    CURLcode res;

    curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl_handle, CURLOPT_COOKIE, cookie.c_str());
    curl_easy_setopt(curl_handle, CURLOPT_HEADERFUNCTION, curl_to_string);
    curl_easy_setopt(curl_handle, CURLOPT_HEADERDATA, &header);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, curl_to_string);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &data);
    res = curl_easy_perform(curl_handle);

    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }

    curl_easy_cleanup(curl_handle);

    return data;
}

Json::Value QBTHandler::getTorrentList() {
    std::string method = "/torrents/info";
    std::string cookie = "SID=" + this->key + ";";
    std::string params = "sort=name";
    std::string url = this->qbt_url + method;
    std::string data;
    std::string header;

    CURL* curl_handle;
    CURLcode res;

    curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl_handle, CURLOPT_COOKIE, cookie.c_str());
    curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, params.c_str());
    curl_easy_setopt(curl_handle, CURLOPT_HEADERFUNCTION, curl_to_string);
    curl_easy_setopt(curl_handle, CURLOPT_HEADERDATA, &header);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, curl_to_string);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &data);
    res = curl_easy_perform(curl_handle);

    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }

    curl_easy_cleanup(curl_handle);

    Json::Value json = parseStringToJson(data);

    return json;
}

void QBTHandler::renameFile(const std::string &hash, const std::string &oldPath, const std::string &newPath) {
    std::string method = "/torrents/renameFile";
    std::string cookie = "SID=" + this->key + ";";
    std::string params = "hash=" + hash + "&oldPath=" + oldPath + "&newPath=" + newPath;
    std::string url = this->qbt_url + method;
    std::string data;
    std::string header;

    CURL* curl_handle;
    CURLcode res;

    curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl_handle, CURLOPT_COOKIE, cookie.c_str());
    curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, params.c_str());
    curl_easy_setopt(curl_handle, CURLOPT_HEADERFUNCTION, curl_to_string);
    curl_easy_setopt(curl_handle, CURLOPT_HEADERDATA, &header);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, curl_to_string);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &data);
    res = curl_easy_perform(curl_handle);

    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }

    curl_easy_cleanup(curl_handle);
    return;
}