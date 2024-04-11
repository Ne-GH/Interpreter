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
	return path_;
}

std::string& File::GetContent() {
	return content_;
}
File::File(QWidget* window) :window_(window) {  }

auto File::Create() -> Path {
	path_ = QFileDialog::getSaveFileName(window_,
		"创建文件",
		".", 
		"C语言文件(*.c);;All Files (*)").toStdString();

#if defined(_WIN32) || defined(_WIN64)
	StringPathToWStringPath(path_);
#endif
	std::fstream file_system(path_,std::ios::in | std::ios::out | std::ios::trunc);
	return path_;
}

void File::Open() {
	path_ = QFileDialog::getOpenFileName(
		nullptr,
		"打开文件",
		"/",
		"C文件(*.c);;所有文件(*.*)").toStdString();

#if defined(_WIN32) || defined(_WIN64)
	StringPathToWStringPath(path_);
#endif


	Read();
}

void File::Read() {
	content_.clear();
	if (path_.empty()) {
		return;
	}
	std::fstream file_stream(path_);
	std::string line;
	while (std::getline(file_stream, line)) {
		content_ += line + "\n";
	}
}
void File::Read(Path file_path) {
	path_ = file_path;
	Read();
}

void File::Save(std::string_view updata_text) {
	if (path_.empty()) {
		path_ = Create();
	}
	std::fstream file(path_);
	file << updata_text;

    content_ = updata_text;

}

void File::Close() {
	path_.clear();
	content_.clear();
}

