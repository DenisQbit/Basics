#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm> // ��� ������������� std::min
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <filesystem>
//#include <unistd.h>

namespace fs = std::filesystem;

int main() {
    const std::string desktopPath = "."; // ���� � ������� ����������
    const std::string fileName = desktopPath + "file.cpp";
    const size_t fileSize = 7ull * 1024 * 1024 * 1024;

    std::ofstream file(fileName, std::ios::binary | std::ios::out);
    if (file.is_open()) {
        char buffer[1024] = { 0 }; // �����, ����������� ������
        size_t totalWritten = 0;

        while (totalWritten < fileSize) {
            size_t toWrite = sizeof(buffer);
            if (fileSize - totalWritten < toWrite) {
                toWrite = fileSize - totalWritten;
            }
            file.write(buffer, toWrite);
            totalWritten += toWrite;
        }

        file.close();

        std::cout << "File created with size " << fileSize << " bytes." << std::endl;
    }
    else {
        std::cerr << "Failed to create file." << std::endl;
    }

    // ���� � ����������, � ������� ����� ������ �����
    fs::path search_path = "."; // ������� ����������


    // ���������� ������, ������� �� ����
    std::vector<std::string> extensions = { ".txt", ".cpp", ".h", ".jpg", ".png" };

    // �������� �� ���� ������ � ����������
    for (const auto& entry : fs::directory_iterator(search_path)) {
        if (entry.is_regular_file()) { // ���������, ��� ��� ������� ����
            // �������� ���������� �����
            std::string extension = entry.path().extension().string();
            // ���������, ���� �� ���������� � ������ �������
            if (std::find(extensions.begin(), extensions.end(), extension) != extensions.end()) {
                // ������� �������� �����
                std::cout << entry.path().filename() << std::endl;
                // ���� ��� ���� .cpp, �� ������� ��� ����������
                if (extension == ".cpp") {
                    std::ifstream file(entry.path());
                    if (file.is_open()) {
                        std::string line;
                        while (std::getline(file, line)) {
                            std::cout << line << std::endl;
                        }
                        file.close();
                    }
                    else {
                        std::cout << "�� ������� ������� ���� " << entry.path() << std::endl;
                    }
                }
            }
        }
    }
    /*
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != nullptr) {
        std::cout << "Current working directory: " << cwd << std::endl;
    }
    else {
        std::cerr << "Error getting current working directory." << std::endl;
    }
    */
    return 0;
}