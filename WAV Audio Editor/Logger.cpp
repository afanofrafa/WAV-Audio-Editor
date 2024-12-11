#include "Logger.h"

std::ofstream Logger::logFile;
std::mutex Logger::logMutex;

void Logger::Init(const std::string& filename) {
    std::lock_guard<std::mutex> lock(logMutex);
    // Открываем файл с флагом std::ios::trunc, чтобы перезаписать его содержимое при запуске
    logFile.open(filename, std::ios::out | std::ios::trunc);
    if (!logFile.is_open()) {
        
    }
}

void Logger::Log(const std::string& message) {
    std::lock_guard<std::mutex> lock(logMutex); // Синхронизируем доступ к файлу

    if (logFile.is_open()) {
        logFile << message << std::endl;
    }
    else {
    }
}

void Logger::Close() {
    std::lock_guard<std::mutex> lock(logMutex);
    if (logFile.is_open()) {
        logFile.close();
    }
}