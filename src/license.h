#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <stdexcept>

class License {
public:
	explicit License(const std::string& path) {
		loadFromFile(path);
	}

	bool isValid() const {
		return valid;
	}

	std::string getOwner() const {
		return owner;
	}

	std::time_t getExpiry() const {
		return expiry;
	}

	void loadFromFile(const std::string& path) {

		std::ifstream fin(path);   // C++11 ¼æÈÝ
		if (!fin) {
			throw std::runtime_error("License file not found: " + path);
		}

		std::string key;
		owner.clear();
		expiry = 0;
		valid = false;

		while (fin >> key) {
			if (key == "USER") {
				fin >> owner;
			}
			else if (key == "EXPIRY") {
				long long timestamp;
				fin >> timestamp;
				expiry = static_cast<std::time_t>(timestamp);
			}
		}

		std::time_t now = std::time(NULL);
		if (!owner.empty() && expiry > now) {
			valid = true;
		}
	}

private:
	std::string owner;
	std::time_t expiry;
	bool valid;

	
};