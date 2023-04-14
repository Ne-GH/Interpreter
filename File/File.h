#ifndef FILE_H
#define FILE_H

#include <string>
#include <string_view>
#include <filesystem>
#include <QWidget>
#include <QTextEdit>
class File {
	using Path = std::filesystem::path;

	std::string _content;
	Path _path;
	QWidget* _window;
public:
	File() = default;
    File(QWidget* window);

	
	Path Create();
	void Open();
	void Read();
	void Read(Path file_path);
	void Save(std::string_view updata_text);
	void Close();

	std::string& GetContent();

	Path GetPath();
	
};


#endif // !FILE_H
