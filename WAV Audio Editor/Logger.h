#pragma once
#include <fstream>
#include <string>
#include <mutex>

class Logger {
public:
    static void Init(const std::string& filename); // ������������� � ������ �����
    static void Log(const std::string& message);   // ����������� ���������
    static void Close();                           // �������� ����� ����

private:
    static std::ofstream logFile; // �������� ����� ��� ������ �����
    static std::mutex logMutex;   // ������� ��� ������������� ������� � �����
};

