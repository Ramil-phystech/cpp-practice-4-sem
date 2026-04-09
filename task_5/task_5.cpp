#include <iostream>
#include <string>
#include <deque>
#include <ctime>

enum LogLevel {
    LOG_NORMAL,
    LOG_WARNING,
    LOG_ERROR
};

struct LogEvent {
    std::time_t timestamp = 0;
    LogLevel level = LOG_NORMAL;
    std::string message;
};

class Log {
private:
    inline static Log* instance = nullptr;

    std::deque<LogEvent> events;
    const size_t MAX_EVENTS = 10;

    Log() {}

    Log(const Log&) = delete;
    Log& operator=(const Log&) = delete;

public:
    static Log* Instance() {
        if (instance == nullptr) {
            instance = new Log();
        }
        return instance;
    }

    // Запись сообщения
    void message(LogLevel level, const std::string& msg) {
        LogEvent event;
        event.timestamp = std::time(nullptr);
        event.level = level;
        event.message = msg;

        events.push_back(event);

        if (events.size() > MAX_EVENTS) {
            events.pop_front();
        }
    }

    // Вывод последних событий
    void print() {
        std::cout << "Last " << events.size() << " Events" << std::endl;

        for (const auto& event : events) {
            char time_str[100];
            std::tm timeinfo;

            localtime_s(&timeinfo, &event.timestamp);

            std::strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", &timeinfo);
            std::string level_str;

            switch (event.level) {
            case LOG_NORMAL:
                level_str = "NORMAL";
                break;
            case LOG_WARNING:
                level_str = "WARNING";
                break;
            case LOG_ERROR:
                level_str = "ERROR";
                break;
            }

            std::cout << "[" << time_str << "] [" << level_str << "] " << event.message << std::endl;
        }
        std::cout << "--------------------------" << std::endl;
    }
};


int main() {
    Log* log = Log::Instance();

    log->message(LOG_NORMAL, "program loaded");
    log->message(LOG_WARNING, "badly memory usage");

    for (int i = 1; i <= 12; ++i) {
        log->message(LOG_NORMAL, "check #" + std::to_string(i));
    }

    log->message(LOG_ERROR, "error happens! help me!");

    log->print();

    return 0;
}