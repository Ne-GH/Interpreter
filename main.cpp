#include "mainwindow.h"
#include <QApplication>
#include <iostream>
#include <filesystem>
#include "Interpreter/Interpreter.h"
#include "File/File.h"

int main(int argc, char *argv[]) {

    if (argc >= 2) {
        if (std::filesystem::exists(argv[1])) {
            std::fstream file(argv[1]);
            std::string file_str,tmp;

            while (std::getline(file,tmp))
                file_str += tmp + '\n';
            
            Interpreter interpreter(true);
            interpreter.SetMod(Interpreter::RUN);
            interpreter.Run(file_str);
        }
        else {
            std::cout << "文件不存在" << std::endl;
        }
        return 0;
    }
    else {
        QApplication a(argc, argv);
        MainWindow w;

        w.show();

        return a.exec();
    }


}
