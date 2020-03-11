#include "SPS-Hub.h"

using namespace std;
using namespace httplib;
using json = nlohmann::json;

int main()
{
	map<string, PowerSocket> power_sockets;

	string HUB_ADDRESS;
	{
		fstream ip_conf("ipaddr", ios::in);
		if (!ip_conf.is_open()) {
			cout << "Unable read local IP address.\n";
			return 0;
		}

		int n;
		ip_conf >> n;

		vector<string> ip_addresses(n);
		for (int i = 0; i < n; i++) {
			ip_conf >> ip_addresses[i];
		}

		if (n >= 2) {
			HUB_ADDRESS = ip_addresses[1];
		}
		else {
			cout << "Not connected to local network.\n";
			return 0;
		}
	}

	ConfManager conf_manager;
	conf_manager.ReadConfig();

	int hub_port = conf_manager.GetSetting<int>("hub_port");
	int power_socket_port = conf_manager.GetSetting<int>("power_socket_port");
	
	{ // Read power sockets' {key;address} pairs from config file
		fstream input_file("addr_list.conf", ios::in);
		if (!input_file.is_open()) {
			cout << "Unable to read power sockets' addresses.\n";
			return 1;
		}

		string key;
		string address;

		while (input_file >> key) {
			if (input_file >> address) {
				power_sockets.try_emplace(key, PowerSocket(address));
			}
			else {
				cout << "Found invalid pair {key;address}.\n";
			}
		}

		if (power_sockets.empty()) {
			cout << "Power socket address list is empty, unable to continue.\n";
			return 0;
		}
	}

	{ // Send hub address to all power sockets
		int success_rate = 0;
		for (auto& power_socket : power_sockets) {
			Client clnt(power_socket.first, power_socket_port);

			json body = {
				{"Hub Address", HUB_ADDRESS},
				{"Key", power_socket.first}
			};

			auto result = clnt.Post("/hub", body.dump(), "application/json");
			if (result && result->status == 200) {
				success_rate++;
				power_socket.second.notified = true;
				cout << "Successfully sent hub address to {" << power_socket.first << ";" << power_socket.second.address << "}.\n";
			}
			else {
				cout << "Connection to {" << power_socket.first << ";" << power_socket.second.address << "} was unsuccessful.\n";
			}
		}

		if (success_rate == 0) {
			cout << "Failed to notify any power sockets about Hub existance, unable to continue.\n";
			return 0;
		}
	}

	Server srv;

	srv.Get("/data_point", [](const Request& req, Response& res) { // Front-end server reqesting info about power

		});

	srv.Post("/data", [&](const Request& req, Response& res) { // Power socket sending info about power
		json body = json::parse(req.body);
		
		string key = body["Key"].get<string>();
		float data = body["Data"].get<float>();

		power_sockets[key].latest_data = data;

		// Update statistics here
		});

	srv.Get("/socket_state", [](const Request& req, Response& res) { // Front-end server requesting info about socket states

		});

	srv.Post("/socket_state", [](const Request& req, Response& res) { // Front-end server sending info about socket states

		});

	srv.listen("localhost", hub_port);

	return 0;
}
