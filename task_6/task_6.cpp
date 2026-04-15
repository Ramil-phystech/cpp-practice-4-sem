#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <clocale>

struct ControlPoint {
    std::string name;
    double latitude;  
    double longitude;
    bool isMandatory;
    double penaltyHours; 
};


class ICPBuilder {
public:
    virtual ~ICPBuilder() = default;
    virtual void buildMandatoryCP(const std::string& name, double lat, double lon) = 0;
    virtual void buildOptionalCP(const std::string& name, double lat, double lon, double penalty) = 0;
    virtual void reset() = 0;
};


class TextOutputBuilder : public ICPBuilder {
private:
    int index;
    std::stringstream ss;

public:
    TextOutputBuilder() { 
        reset(); 
    }

    void reset() override {
        index = 1;
        ss.str("");
        ss.clear();
    }

    void buildMandatoryCP(const std::string& name, double lat, double lon) override {
        ss << index++ << ". " << name
            << " | Координаты: (" << lat << "°, " << lon << "°) | "
            << "незачёт СУ\n";
    }

    void buildOptionalCP(const std::string& name, double lat, double lon, double penalty) override {
        ss << index++ << ". " << name
            << " | Координаты: (" << lat << "°, " << lon << "°) | "
            << "Штраф: " << penalty << " ч.\n";
    }

    std::string getResult() const {
        return ss.str();
    }
};


class PenaltyCalculatorBuilder : public ICPBuilder {
private:
    double totalPenalty;

public:
    PenaltyCalculatorBuilder() { 
        reset(); 
    }

    void reset() override {
        totalPenalty = 0.0;
    }

    void buildMandatoryCP(const std::string& name, double lat, double lon) override {

    }

    void buildOptionalCP(const std::string& name, double lat, double lon, double penalty) override {
        totalPenalty += penalty;
    }

    double getResult() const {
        return totalPenalty;
    }
};


class RouteDirector {
private:
    ICPBuilder* builder;

public:
    void setBuilder(ICPBuilder* b) {
        builder = b;
    }

    void processRoute(const std::vector<ControlPoint>& route) {
        if (!builder) 
            return;

        builder->reset();
        for (const auto& cp : route) {
            if (cp.isMandatory) {
                builder->buildMandatoryCP(cp.name, cp.latitude, cp.longitude);
            }
            else {
                builder->buildOptionalCP(cp.name, cp.latitude, cp.longitude, cp.penaltyHours);
            }
        }
    }
};


int main() {
    setlocale(LC_ALL, "Russian");

    std::vector<ControlPoint> track = {
        {"Старт", 55.75, 37.61, true, 0.0},
        {"Брод", 55.80, 37.65, false, 2.5},
        {"Подъем", 55.82, 37.70, true, 0.0},
        {"Лужа", 55.85, 37.75, false, 4.0},
        {"Финиш", 55.90, 37.80, true, 0.0}
    };

    RouteDirector director;

    TextOutputBuilder textBuilder;
    director.setBuilder(&textBuilder);
    director.processRoute(track);

    std::cout << "Список контрольных пунктов\n";
    std::cout << textBuilder.getResult();

    std::cout << "\n----------------------------------\n\n";

    PenaltyCalculatorBuilder penaltyBuilder;
    director.setBuilder(&penaltyBuilder);
    director.processRoute(track);

    std::cout << "Суммарный возможный штраф за пропуск необязательных КП: "
        << penaltyBuilder.getResult() << " часов.\n";

    return 0;
}
