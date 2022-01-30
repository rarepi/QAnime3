#include "TVDBHandler.h"
#include <curl/curl.h>

TVDBHandler::TVDBHandler() {
	instance = new TVDBHandler();
}

TVDBHandler* TVDBHandler::getInstance() {
	if (!instance) {
		instance = new TVDBHandler();
	}
	return instance;
}