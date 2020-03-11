#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <string>

using namespace std;

class ConfManager {
public:
	const int DEFAULT_HUB_PORT = 7610;
	const int DEFAULT_POWER_SOCKET_PORT = 1029;

	ConfManager() {
		predefined_setting_map["hub_port"] = SettingType::TYPE_INT;
		predefined_setting_map["power_socket_port"] = SettingType::TYPE_INT;
	}

	int ReadConfig() {
		fstream config_file("hub.conf", ios::in);
		if (!config_file.is_open()) {
			cout << "Unable to open config file. Creating default config file.\n";

			fstream new_config_file("hub.conf", ios::out);
			if (!new_config_file.is_open()) {
				cout << "Unable to create default config file.\n";
				return 1;
			}

			new_config_file << "hub_port " << DEFAULT_HUB_PORT << "\n";
			new_config_file << "power_socket_port " << DEFAULT_POWER_SOCKET_PORT << "\n";

			int_setting_map["hub_port"] = DEFAULT_HUB_PORT;
			int_setting_map["power_socket_port"] = DEFAULT_POWER_SOCKET_PORT;
		}
		else {
			string key, value;
			while (config_file >> key) {
				if (config_file >> value) {
					SettingType type = predefined_setting_map[key];

					switch (type) {
					case SettingType::TYPE_STRING: 
						str_setting_map.try_emplace(key, value);
						break;
					case SettingType::TYPE_INT:
						int_setting_map.try_emplace(key, stoi(value));
						break;
					case SettingType::TYPE_FLOAT: 
						float_setting_map.try_emplace(key, stof(value));
						break;
					}

				}
				else {
					cout << "Found invalid pair in config file {key;value}.\n";
				}
			}
		}

		return 0;
	}

	template<class T>
	struct item_return { 
		typedef T type; 
	};

	template<class T>
	typename item_return<T>::type GetSetting(const string& key_);

	template<>
	item_return<string>::type GetSetting<string>(const string& key_) {
		return str_setting_map[key_];
	}

	template<>
	item_return<int>::type GetSetting<int>(const string& key_) {
		return int_setting_map[key_];
	}

	template<>
	item_return<float>::type GetSetting<float>(const string& key_) {
		return float_setting_map[key_];
	}

private:
	map<string, string> str_setting_map;
	map<string, int> int_setting_map;
	map<string, float> float_setting_map;

	enum class SettingType {
		TYPE_INT, 
		TYPE_STRING,
		TYPE_FLOAT
	};

	map<string, SettingType> predefined_setting_map;
};