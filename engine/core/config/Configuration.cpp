/*
 * Copyright 2017 James De Broeck
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "../CoreInternal.h"

#include <iostream>
#include <sstream>

using namespace Swarm::Logging;

namespace Swarm {
    namespace Config {

        RawConfigData::RawConfigData(const char * filepath) {
            Core::log_core(INFO) << "Reading configuration file '" << filepath << "'" << Flush();
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
                Core::log_core(WARNING) << "Failed to open '" << filepath << "'" << Flush();
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