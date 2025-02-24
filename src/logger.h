#ifndef _LOGGER_H_
#define _LOGGER_H_
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>

class Logger {
public:
    enum LogLevel {
        DEBUG,
        INFO,
        WARNING,
        ERROR
    };

    static void log(LogLevel level, const std::string& message) {
        std::string levelStr;
        switch (level) {
            case DEBUG:   levelStr = "DEBUG"; break;
            case INFO:    levelStr = "INFO"; break;
            case WARNING: levelStr = "WARNING"; break;
            case ERROR:   levelStr = "ERROR"; break;
        }

        std::string timestamp = getCurrentTimestamp();
        std::string logMessage = timestamp + " [" + levelStr + "] " + message + "\n";

        // Print to console
        std::cout << logMessage;

        // Write to file
        std::ofstream logFile("/opt/apps/com.openfde.manager/logs/application.log", std::ios::app);
        if (logFile.is_open()) {
            logFile << logMessage;
            logFile.close();
        }
    }

private:
    static std::string getCurrentTimestamp() {
        time_t now = time(nullptr);
        char buffer[80];
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localtime(&now));
        return std::string(buffer);
    }
};
#endif
