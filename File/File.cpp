#include "File.h"

File::File(std::filesystem::path path) : _path(path) {  }

auto File::GetPath() -> Path{
	return _path;
}
