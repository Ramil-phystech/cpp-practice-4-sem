#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <utility>
#include <vector>

class JsonFileWriter {
public:
    explicit JsonFileWriter(const std::string& filename)
        : is_valid_(true), is_first_element_(true) {

        file_.open(filename);
        if (!file_.is_open()) {
            throw std::runtime_error("Couldn't open the file: " + filename);
        }
        file_ << "{\n";
    }

    ~JsonFileWriter() {
        Close();
    }

    // Запрещаем копирование (так как файл один и эксклюзивный)
    JsonFileWriter(const JsonFileWriter&) = delete;
    JsonFileWriter& operator=(const JsonFileWriter&) = delete;

    JsonFileWriter(JsonFileWriter&& other) noexcept
        : file_(std::move(other.file_)),
        is_valid_(other.is_valid_),
        is_first_element_(other.is_first_element_) {
        other.is_valid_ = false;
    }

    JsonFileWriter& operator=(JsonFileWriter&& other) noexcept {
        if (this != &other) {
            Close();

            file_ = std::move(other.file_);
            is_valid_ = other.is_valid_;
            is_first_element_ = other.is_first_element_;

            other.is_valid_ = false;
        }
        return *this;
    }

    void Add(const std::string& key, const std::string& value) {
        if (!is_valid_ || !file_.is_open()) {
            throw std::logic_error("Attempt to write to a closed or moved JsonFileWriter");
        }

        if (!is_first_element_) {
            file_ << ",\n";
        }
        is_first_element_ = false;

        file_ << "  \"" << EscapeJson(key) << "\": \"" << EscapeJson(value) << "\"";
    }

private:
    std::ofstream file_;
    bool is_valid_;           // Флаг для отслеживания состояния (владеет ли объект файлом)
    bool is_first_element_;   // Флаг для правильной расстановки запятых

    void Close() {
        if (is_valid_ && file_.is_open()) {
            file_ << "\n}\n";
            file_.close();
            is_valid_ = false;
        }
    }

    static std::string EscapeJson(const std::string& s) {
        std::string result;
        for (char c : s) {
            if (c == '"') result += "\\\"";
            else if (c == '\\') result += "\\\\";
            else if (c == '\n') result += "\\n";
            else result += c;
        }
        return result;
    }
};


// Класс-сущность: Sensor
class Sensor {
public:
    Sensor(const std::string& id, const std::string& type)
        : id_(id), logger_(id + "_data.json") {

        logger_.Add("sensor_id", id_);
        logger_.Add("sensor_type", type);
    }

    Sensor(Sensor&&) = default;
    Sensor& operator=(Sensor&&) = default;

    void RecordEvent(const std::string& event_name) {
        logger_.Add("last_event", event_name);
    }

    void RecordMetric(const std::string& metric_name, double value) {
        logger_.Add(metric_name, std::to_string(value));
    }

    std::string GetId() const { return id_; }

private:
    std::string id_;
    JsonFileWriter logger_;
};

Sensor CreateAndInitializeSensor(const std::string& id, const std::string& type) {
    Sensor new_sensor(id, type);
    new_sensor.RecordEvent("Initialization_started");

    return new_sensor;
}

int main() {
    {
        std::cout << "1. Creating temporary system log:\n";
        Sensor sys_log("system_core", "diagnostic");
        sys_log.RecordEvent("Startup");
        sys_log.RecordMetric("cpu_load_percent", 12.5);

    } 

    std::cout << "\n-----------------------\n\n";
    std::cout << "2. Working with sensor array:\n";

    std::vector<Sensor> network;

    network.reserve(2);

    network.push_back(CreateAndInitializeSensor("sensor_alpha", "temperature"));
    network.push_back(CreateAndInitializeSensor("sensor_beta", "humidity"));

    std::cout << "\nRecording data to sensors in a loop:\n";
    for (auto& sensor : network) {
        std::cout << "Writing metric for " << sensor.GetId() << "...\n";

        if (sensor.GetId() == "sensor_alpha") {
            sensor.RecordMetric("current_temp", 24.5);
        }
        else {
            sensor.RecordMetric("current_humidity", 60.2);
        }
        sensor.RecordEvent("Data_collected");
    }

    try {
        std::cout << "Creating original sensor...\n";
        Sensor original_sensor("sensor_gamma", "pressure");

        std::cout << "Moving original sensor to a new sensor...\n";
        Sensor moved_sensor = std::move(original_sensor);

        std::cout << "Writing to the NEW sensor (should succeed)...\n";
        moved_sensor.RecordEvent("Data for moved sensor");

        std::cout << "Writing to the ORIGINAL sensor (should fail)...\n";
        original_sensor.RecordEvent("This will cause an error");
    }
    catch (const std::exception& e) {
        std::cout << "[CAUGHT EXCEPTION]: " << e.what() << "\n";
    }

    return 0; 
}
