#pragma once
#include <string>

#include <curl/curl.h>
#include <json/json.h>

class QBTHandler {
	public:
		static QBTHandler* createInstance(
			const std::string& qbt_url, 
			const std::string& username, 
			const std::string& password
		);
		static QBTHandler* getInstance();
		void authenticate();
		std::string getAppVersion();
		Json::Value getTorrentList();
		void renameFile(const std::string& hash, const std::string& oldPath, const std::string& newPath);

	private:
		// private constructor for singleton
		QBTHandler(
			const std::string& qbt_url,
			const std::string& username,
			const std::string& password
		);
		static QBTHandler* instance;
		std::string qbt_url;
		std::string username;
		std::string password;
		std::string key;
};