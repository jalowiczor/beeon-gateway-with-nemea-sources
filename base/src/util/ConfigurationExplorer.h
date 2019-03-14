#pragma once

namespace BeeeOn {

class FileLoader;

class ConfigurationExplorer {
public:
	virtual ~ConfigurationExplorer();

	virtual void explore(FileLoader &loader) = 0;
};

}
