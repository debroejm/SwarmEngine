#include "../../Core.h"

#include <sstream>

using namespace Swarm::Logging;

namespace Swarm {
    namespace Config {

        RawConfigData::RawConfigData(const char * filepath) {
            this->filepath = filepath;
            ifstream dataStream(filepath, ios::in);
            if(dataStream.is_open()) {
                string line = "";
                while(getline(dataStream, line)) {
                    size_t splitIndex = line.find_first_of(":=");
                    if(splitIndex == string::npos) continue;
                    string key = line.substr(0, splitIndex);
                    string value = line.substr(splitIndex+1, line.size()-splitIndex-1);
                    int testInt;
                    if(stringstream(value)>>testInt) {
                        dataMap_int[key] = testInt;
                    } else {
                        dataMap_str[key] = value;
                    }
                }
                dataStream.close();
            } else {
                Log::log_config(WARNING) << "Failed to open '" << filepath << "'";
                return;
            }
        }

        RawConfigData::~RawConfigData() {

        }

        void RawConfigData::writeConfigData(const char * filepath) {
            ofstream outputStream;
            outputStream.open(filepath, ios::out | ios::trunc);

            unordered_map<string, int>::iterator iInt = dataMap_int.begin();
            while(iInt != dataMap_int.end()) {
                outputStream << iInt->first << "=" << iInt->second << "\n";
                iInt++;
            }

            unordered_map<string, string>::iterator iStr = dataMap_str.begin();
            while(iStr != dataMap_str.end()) {
                outputStream << iStr->first << "=" << iStr->second << "\n";
                iStr++;
            }

            outputStream.close();
        }

        int RawConfigData::getInteger(string key, int defaultValue) {
            unordered_map<string, int>::iterator i = dataMap_int.find(key);
            if(i == dataMap_int.end()) {
                setInteger(key, defaultValue);
                return defaultValue;
            }
            else return i->second;
        }
        void RawConfigData::setInteger(string key, int value) { dataMap_int[key] = value; }

        string RawConfigData::getString(string key, string defaultValue) {
            unordered_map<string, string>::iterator i = dataMap_str.find(key);
            if(i == dataMap_str.end()) {
                setString(key, defaultValue);
                return defaultValue;
            }
            else return i->second;
        }
        void RawConfigData::setString(string key, string value) { dataMap_str[key] = value; }

    }
}