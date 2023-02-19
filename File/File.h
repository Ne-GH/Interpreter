#ifndef FILE_H
#define FILE_H

#include <string>
#include <string_view>
#include <filesystem>

class File {
	using Path = std::filesystem::path;
	std::string _content;
	Path _path;

public:
	File() = default;
	explicit File(Path path);
	
	void Open();
	void Save();
	void Close();

	std::string_view GetContent();
	Path GetPath();
	
};


#endif // !FILE_H
