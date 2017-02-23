#include "api/Core.h"

#include "api/Logging.h"

#include <fstream>
#include <iostream>
#include <sstream>

using namespace Swarm::Logging;

namespace Swarm {
    namespace Config {

        RawConfigData::RawConfigData(const char * filepath) {
            Log::log_core(INFO) << "Reading configuration file '" << filepath << "'";
            this->filepath = filepath;
            std::ifstream dataStream(filepath, std::ios::in);
            if(dataStream.is_open()) {
                std::string line = "";
                while(getline(dataStream, line)) {
                    size_t splitIndex = line.find_first_of(":=");
                    if(splitIndex == std::string::npos) continue;
                    std::string key = line.substr(0, splitIndex);
                    std::string value = line.substr(splitIndex+1, line.size()-splitIndex-1);
                    int testInt;
                    if(std::stringstream(value)>>testInt) {
                        dataMap_int[key] = testInt;
                    } else {
                        dataMap_str[key] = value;
                    }
                }
                dataStream.close();
            } else {
                Log::log_core(WARNING) << "Failed to open '" << filepath << "'";
                return;
            }
        }

        RawConfigData::~RawConfigData() {

        }

        void RawConfigData::writeConfigData(const char * filepath) {
            std::ofstream outputStream;
            outputStream.open(filepath, std::ios::out | std::ios::trunc);

            std::unordered_map<std::string, int>::iterator iInt = dataMap_int.begin();
            while(iInt != dataMap_int.end()) {
                outputStream << iInt->first << "=" << iInt->second << "\n";
                iInt++;
            }

            std::unordered_map<std::string, std::string>::iterator iStr = dataMap_str.begin();
            while(iStr != dataMap_str.end()) {
                outputStream << iStr->first << "=" << iStr->second << "\n";
                iStr++;
            }

            outputStream.close();
        }

        int RawConfigData::getInteger(std::string key, int defaultValue) {
            std::unordered_map<std::string, int>::iterator i = dataMap_int.find(key);
            if(i == dataMap_int.end()) {
                setInteger(key, defaultValue);
                return defaultValue;
            }
            else return i->second;
        }
        void RawConfigData::setInteger(std::string key, int value) { dataMap_int[key] = value; }

        std::string RawConfigData::getString(std::string key, std::string defaultValue) {
            std::unordered_map<std::string, std::string>::iterator i = dataMap_str.find(key);
            if(i == dataMap_str.end()) {
                setString(key, defaultValue);
                return defaultValue;
            }
            else return i->second;
        }
        void RawConfigData::setString(std::string key, std::string value) { dataMap_str[key] = value; }

    }
}