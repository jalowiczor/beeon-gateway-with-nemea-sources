#pragma once

#include <cmath>
#include <string>

#include <Poco/JSON/JSON.h>
#include <Poco/JSON/Parser.h>

namespace BeeeOn {

class JsonUtil {
public:
	/*
	 * It parses message and return JSON Object.
	 * @param &data JSON message string
	 * @return JSON Object
	 */
	static Poco::JSON::Object::Ptr parse(const std::string &data);
};

}
