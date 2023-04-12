#include "File.h"
#include <QFileDialog>
#include <fstream>
#include "../Logs/Log.h"
#include <codecvt>

void StringPathToWStringPath(std::filesystem::path& path) {
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	std::wstring wpath = conv.from_bytes(path.string());
	path = std::filesystem::path(wpath);
	return;
}

auto File::GetPath() -> Path{
	return _path;
}

std::string& File::GetContent() {
	return _content;
}
File::File(QWidget* window) :_window(window) {  }

auto File::Create() -> Path {
	_path = QFileDialog::getSaveFileName(_window,
		"创建文件",
		".", 
		"C语言文件(*.c);;All Files (*)").toStdString();

#if defined(_WIN32) || defined(_WIN64)
	StringPathToWStringPath(_path);
#endif
	std::fstream file_system(_path,std::ios::in | std::ios::out | std::ios::trunc);
	return _path;
}

void File::Open() {
	_path = QFileDialog::getOpenFileName(
		nullptr,
		"打开文件",
		"/",
		"C文件(*.c);;所有文件(*.*)").toStdString();

#if defined(_WIN32) || defined(_WIN64)
	StringPathToWStringPath(_path);
#endif


	Read();
}

void File::Read() {
	_content.clear();
	if (_path.empty()) {
		return;
	}
	std::fstream file_stream(_path);
	std::string line;
	while (std::getline(file_stream, line)) {
		_content += line + "\n";
	}
}
void File::Read(Path file_path) {
	_path = file_path;
	Read();
}

void File::Save(std::string_view updata_text) {
	if (_path.empty()) {
		_path = Create();
	}
	std::fstream file_system(_path);

    _content = updata_text;

}

void File::Close() {
	_path.clear();
	_content.clear();
}

