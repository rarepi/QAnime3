#include "TVDBHandler.h"

// initialize pointer for first getInstance call
TVDBHandler* TVDBHandler::instance = 0;

TVDBHandler::TVDBHandler(const std::string& TVDB_URL, const std::string& tvdb_cache) {
    this->tvdb_url = TVDB_URL;
    this->tvdb_cache = tvdb_cache;
}

TVDBHandler* TVDBHandler::createInstance(const std::string& tvdb_url, const std::string& tvdb_cache) {
    if (instance) delete instance;
    instance = new TVDBHandler(tvdb_url, tvdb_cache);
    return instance;
}

TVDBHandler* TVDBHandler::getInstance() {
    if (!instance)
        throw "TVDBHandler has not yet been initialized";
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

Series* TVDBHandler::getSeriesData(const std::string& tvdbName) {
    std::string url = this->tvdb_url + "/" + tvdbName;
    std::string html = this->getHtml(url);
    Series* seriesData;
    if (html.size() > 0) {
        seriesData = this->parseSeries(html.c_str(), tvdbName);
    } // else return nullptr
    return seriesData;
}

Series* TVDBHandler::parseSeries(const char* html, const std::string& tvdbName) {
    if (!html) return nullptr;
    GumboOutput* htmlParsed = gumbo_parse(html);
    GumboNode* root = htmlParsed->root;

    // find basic series info div
    GumboNode* seriesInfoDiv = findBasicSeriesInfo(root);
    if (!seriesInfoDiv || seriesInfoDiv->v.element.children.length == 0) return nullptr;
    
    // get first <ul>
    GumboNode* ul;
    for (int i = 0; i < seriesInfoDiv->v.element.children.length; i++) {
        ul = (GumboNode*)seriesInfoDiv->v.element.children.data[i];
        if (ul->v.element.tag == GUMBO_TAG_UL)
            break;
    }
    // check if node element <ul>, else the html is in an unexpected format
    if (ul->type != GUMBO_NODE_ELEMENT || ul->v.element.tag != GUMBO_TAG_UL)
        throw "parseSeries: Unexpected format. Couldn't find <ul> element.";

    Series* series = new Series(tvdbName);
    std::vector<std::pair<std::string, std::string>> dataCollection;

    // iterate through all <li>
    for (size_t i = 0; i < ul->v.element.children.length; i++) {
        GumboNode* li = (GumboNode*) ul->v.element.children.data[i];
        // check if node element <li>, else skip
        if (!li || li->type != GUMBO_NODE_ELEMENT || li->v.element.tag != GUMBO_TAG_LI)
            continue;
        
        std::pair<std::string, std::string> data;
        // iterate through all children of <li>
        for (size_t j = 0; j < li->v.element.children.length; j++) {
            GumboNode* li_child = (GumboNode*) li->v.element.children.data[j];

            // check if node element, else skip
            if (!li_child || li_child->type != GUMBO_NODE_ELEMENT)
                continue;

            // save node tag for next step and check if it's either a <strong> or <span>, else skip
            GumboTag tag = li_child->v.element.tag;
            if (tag != GUMBO_TAG_STRONG && tag != GUMBO_TAG_SPAN)
                continue;

            // iterate through the <strong>'s and <span>'s content nodes
            for (size_t k = 0; k < li_child->v.element.children.length; k++) {
                GumboNode* contentNode = (GumboNode*) li_child->v.element.children.data[k];
                // dig one deeper if it's a hyperlink
                if (contentNode->type == GUMBO_NODE_ELEMENT && contentNode->v.element.tag == GUMBO_TAG_A)
                    contentNode = (GumboNode*)contentNode->v.element.children.data[0];

                // write any qualified data into string pairs
                if (contentNode->type == GUMBO_NODE_TEXT) {
                    switch (tag) {
                    case GUMBO_TAG_STRONG:  // description
                        data.first = contentNode->v.text.text;
                        break;
                    case GUMBO_TAG_SPAN:    // value
                        if (data.second.empty())
                            data.second = contentNode->v.text.text;
                        else // case of multiple spans, we append these to the first span.
                            data.second.append(", " + std::string(contentNode->v.text.text));
                        break;
                    }
                }
            }           
        }
        dataCollection.push_back(data);
    }

    // write collected series data into Series object and return it
    for (auto data : dataCollection) {
        // trim whitespace
        trim(data.first);
        trim(data.second);
        
        // remove duplicate whitespace within the string (wtf tvdb?)
        std::string::iterator new_end = std::unique(data.second.begin(),data.second.end(),
                [&](char lhs, char rhs) -> bool {
                    return (std::isspace(lhs) && std::isspace(rhs));
                });
        data.second.erase(new_end, data.second.end()); 

        // pair descriptions of wanted information with Series class (ugly hardcoded way)
        // an alternative would be to use the order of elements for identification (which can break just as easily)
        if (data.first == "TheTVDB.com Series ID")
            series->setId(std::stoi(data.second));
        else if (data.first == "First Aired")
            series->setFirstAiredDate(data.second);
        else if (data.first == "Airs")
            series->setAirRhythm(data.second);
        else if (data.first == "Networks")
            series->setFirstAiredBroadcaster(data.second);
    }

    return series;
}

GumboNode* TVDBHandler::findBasicSeriesInfo(GumboNode* node) {
    if (node->type != GUMBO_NODE_ELEMENT)
        return nullptr;

    // find any div
    if (node->v.element.tag == GUMBO_TAG_DIV) {
        // grab id attribute
        GumboAttribute* idAttribute = gumbo_get_attribute(&node->v.element.attributes, "id");
        if (idAttribute) {
            // check if id fits
            std::string idAttributeValue = idAttribute->value;
            if (idAttributeValue.find("series_basic_info") != -1) {
                return node;    // return node if id is correct
            }
        }
    }

    // if current node is not correct we keep searching recursively
    GumboVector* children = &node->v.element.children;
    for (unsigned int i = 0; i < children->length; i++) {
        GumboNode* result = this->findBasicSeriesInfo(static_cast<GumboNode*>(children->data[i]));
        if (result) 
            return result;
    }
    return nullptr;    // none found!
}

Season* TVDBHandler::getSeasonData(const std::string& tvdbName, const int& season) {
    std::string url = this->tvdb_url + "/" + tvdbName + "/seasons/official/" + std::to_string(season);
    std::string html = this->getHtml(url);
    Season* seasonData;
    if (html.size() > 0) {
        seasonData = this->parseSeason(html.c_str(), season);
    } // else return nullptr
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
                                textNode = static_cast<GumboNode*>(contentNode->v.element.children.data[0]);
                                if (ep->getFirstAiredDate().empty()) {
                                    ep->setFirstAiredDate(textNode->v.text.text);
                                } else if (ep->getFirstAiredBroadcaster().empty()) {
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