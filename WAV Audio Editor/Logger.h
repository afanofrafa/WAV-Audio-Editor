#pragma once
#include <fstream>
#include <string>
#include <mutex>

class Logger {
public:
    static void Init(const std::string& filename); // Инициализация с именем файла
    static void Log(const std::string& message);   // Логирование сообщения
    static void Close();                           // Закрытие файла лога

private:
    static std::ofstream logFile; // Файловый поток для записи логов
    static std::mutex logMutex;   // Мьютекс для синхронизации доступа к файлу
};

