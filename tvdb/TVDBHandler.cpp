#include "TVDBHandler.h"

#define TVDB_BASE_URL "https://thetvdb.com/series"

// initialize pointer for first getInstance call
TVDBHandler* TVDBHandler::instance = 0;

TVDBHandler::TVDBHandler() {}

TVDBHandler* TVDBHandler::getInstance() {
	if (!instance)
		instance = new TVDBHandler();
	return instance;
}

// trim functions source: https://stackoverflow.com/a/217605/5920409
static inline void ltrim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
        }));
}

static inline void rtrim(std::string& s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

static inline void trim(std::string& s) {
    ltrim(s);
    rtrim(s);
}

Season* TVDBHandler::getSeasonData(const std::string tvdbName, const int season) {
    std::string url = std::string(TVDB_BASE_URL) + "/" + tvdbName + "/seasons/official/" + std::to_string(season);
    std::string html = getHtml(url);
    Season* seasonData;
    if (html.size() > 0) {
        seasonData = parseSeason(html.c_str(), season);
    } // else return NULL
    return seasonData;
}

Season* TVDBHandler::parseSeason(const char* html, int seasonNumber) {
    if (!html) return NULL;
    GumboOutput* htmlParsed = gumbo_parse(html);
    GumboNode* root = htmlParsed->root;

    // find table content (tbody tag)
    GumboNode* tbody = findTbodyGumbo(root);
    if (!tbody) return NULL;
    GumboVector tableChildren = tbody->v.element.children;

    Season* season = new Season(seasonNumber);

    // iterate through all <tr>
    for (size_t i = 0; i < tableChildren.length; i++) {
        GumboNode* tr = (GumboNode*) tableChildren.data[i];
        if (tr->v.element.tag == GUMBO_TAG_TR) {
            GumboVector* trChildren = &tr->v.element.children;
            Episode* ep;

            // iterate through all <td>
            for (size_t j = 0; j < trChildren->length; j++) {
                GumboNode* td = (GumboNode*)trChildren->data[j];
                if (td->v.element.tag == GUMBO_TAG_TD) {
                    GumboVector tdChildren = td->v.element.children;

                    // iterate through all items in <td>
                    for (size_t k = 0; k < tdChildren.length; k++) {
                        GumboNode* contentNode = static_cast<GumboNode*>(tdChildren.data[k]);
                        GumboNode* textNode;
                        std::string data;

                        // determine item type by element tag
                        switch (contentNode->v.element.tag) {
                            // assume episode number
                            case GUMBO_TAG_STYLE:
                                data = contentNode->v.text.text;  // extract string "SxxExx"
                                data.erase(0, data.find("E")+1);  // remove everything except for the ep number
                                ep = new Episode(*season, data);
                                break;
                            // assume url & episode name
                            case GUMBO_TAG_A:
                                textNode = static_cast<GumboNode*>(contentNode->v.element.children.data[0]);
                                data = textNode->v.text.text;
                                trim(data);
                                ep->setName(data);
                                break;
                            // assume first aired date OR broadcaster
                            case GUMBO_TAG_DIV:
                                // set date if unset, else set broadcaster
                                if (ep->getFirstAiredDate().empty()) {
                                    textNode = static_cast<GumboNode*>(contentNode->v.element.children.data[0]);
                                    ep->setFirstAiredDate(textNode->v.text.text);
                                } else if (ep->getFirstAiredBroadcaster().empty()) {
                                    textNode = static_cast<GumboNode*>(contentNode->v.element.children.data[0]);
                                    ep->setFirstAiredBroadcaster(textNode->v.text.text);
                                } //else throw "Third div encountered!"; // TODO
                                break;
                            // irrelevant, do nothing
                            case GUMBO_TAG_I:
                                break;
                            // unknown tags
                            case GUMBO_TAG_OL:
                            case GUMBO_TAG_P:
                            case GUMBO_TAG_RT:
                            case GUMBO_TAG_MGLYPH:
                            case GUMBO_TAG_KBD:
                            case GUMBO_TAG_OUTPUT:
                                break;
                            default:
                                //throw "Unknown tag encountered!"; // TODO stop after leaving tbody
                                break;
                        }
                    }
                }
            }
        }
    }
    return season;
}

// recursively find the first tbody in given gumbo tree
GumboNode* TVDBHandler::findTbodyGumbo(GumboNode* node) {
    if (node->type != GUMBO_NODE_ELEMENT)
        return NULL;

    if (node->v.element.tag == GUMBO_TAG_TBODY) {
        return node;
    }

    GumboVector* children = &node->v.element.children;
    for (unsigned int i = 0; i < children->length; i++) {
        GumboNode* tbody = findTbodyGumbo(static_cast<GumboNode*>(children->data[i]));
        if (tbody) return tbody;
    }
    return NULL;    // none found!
}

// Source: https://stackoverflow.com/a/5525631/5920409
static size_t curl_to_string(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realsize = size * nmemb;
    static_cast<std::string*>(userp)->append(static_cast<char*>(contents), realsize);
    return realsize;
}

std::string TVDBHandler::getHtml(const std::string &url) {
    CURL* curl_handle;
    CURLcode res;

    std::string data;

    curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, curl_to_string);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &data);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    res = curl_easy_perform(curl_handle);

    if (res != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }

    curl_easy_cleanup(curl_handle);
    return data;
}