#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <algorithm>
#include <memory>  
#include <clocale> 


class Group;


class User {
public:
    std::string id;
    std::string username;
    std::string extraInfo;

    std::weak_ptr<Group> group;

    User(std::string _id, std::string _username, std::string _extraInfo)
        : id(_id), username(_username), extraInfo(_extraInfo) {}
};

class Group {
public:
    std::string id;

    // Список слабых указателей на пользователей.
    std::vector<std::weak_ptr<User>> users;

    Group(std::string _id) : id(_id) {}
};


class SystemManager {
private:
    std::unordered_map<std::string, std::shared_ptr<User>> users;
    std::unordered_map<std::string, std::shared_ptr<Group>> groups;

public:
    void createUser(const std::string& id, const std::string& username, const std::string& info) {
        if (users.count(id)) {
            std::cout << "Ошибка: Пользователь с ID '" << id << "' уже существует.\n";
            return;
        }
        users[id] = std::make_shared<User>(id, username, info);
        std::cout << "Пользователь '" << username << "' успешно создан.\n";
    }

    void deleteUser(const std::string& id) {
        if (!users.count(id)) {
            std::cout << "Ошибка: Пользователь не найден.\n";
            return;
        }

        users.erase(id);
        std::cout << "Пользователь '" << id << "' удален.\n";
    }

    void createGroup(const std::string& id) {
        if (groups.count(id)) {
            std::cout << "Ошибка: Группа с ID '" << id << "' уже существует.\n";
            return;
        }
        groups[id] = std::make_shared<Group>(id);
        std::cout << "Группа '" << id << "' успешно создана.\n";
    }

    void deleteGroup(const std::string& id) {
        if (!groups.count(id)) {
            std::cout << "Ошибка: Группа не найдена.\n";
            return;
        }

        groups.erase(id);
        std::cout << "Группа '" << id << "' удалена.\n";
    }

    void assignUser(const std::string& uId, const std::string& gId) {
        if (!users.count(uId)) { std::cout << "Ошибка: Пользователь не найден.\n"; return; }
        if (!groups.count(gId)) { std::cout << "Ошибка: Группа не найдена.\n"; return; }

        auto userPtr = users[uId];
        auto groupPtr = groups[gId];

        // Проверяем, не был ли пользователь уже в другой группе
        if (auto oldGroup = userPtr->group.lock()) {
            if (oldGroup->id == gId) return; // Уже в этой группе

            // Если был в другой группе, чистим старую группу от него
            auto& oldUsers = oldGroup->users;
            oldUsers.erase(
                std::remove_if(oldUsers.begin(), oldUsers.end(),
                    [&](const std::weak_ptr<User>& wp) {
                        auto sp = wp.lock();
                        return !sp || sp == userPtr; // Удаляем, если указатель протух или это наш юзер
                    }),
                oldUsers.end()
            );
        }

        // Назначаем новую группу
        userPtr->group = groupPtr;             
        groupPtr->users.push_back(userPtr);    

        std::cout << "Пользователь '" << uId << "' добавлен в группу '" << gId << "'.\n";
    }

    void allUsers() {
        if (users.empty()) { std::cout << "Список пользователей пуст.\n"; return; }
        for (const auto& pair : users) printUser(pair.second);
    }

    void getUser(const std::string& id) {
        if (!users.count(id)) { std::cout << "Ошибка: Пользователь не найден.\n"; return; }
        printUser(users.at(id));
    }

    void allGroups() {
        if (groups.empty()) { std::cout << "Список групп пуст.\n"; return; }
        for (const auto& pair : groups) printGroup(pair.second);
    }

    void getGroup(const std::string& id) {
        if (!groups.count(id)) { std::cout << "Ошибка: Группа не найдена.\n"; return; }
        printGroup(groups.at(id));
    }

private:
    void printUser(const std::shared_ptr<User>& u) {
        std::cout << "[User " << u->id << "] Имя: " << u->username
            << " | Инфо: " << (u->extraInfo.empty() ? "-" : u->extraInfo);

        if (auto g = u->group.lock()) {
            std::cout << " | Группа: " << g->id << "\n";
        }
        else {
            std::cout << " | Группа: Нет\n";
        }
    }

    void printGroup(const std::shared_ptr<Group>& g) {
        // Перед выводом очистим вектор от "протухших" пользаков 
        g->users.erase(
            std::remove_if(g->users.begin(), g->users.end(),
                [](const std::weak_ptr<User>& wp) { return wp.expired(); }),
            g->users.end()
        );

        std::cout << "[Group " << g->id << "] Пользователи (" << g->users.size() << "):\n";
        if (g->users.empty()) {
            std::cout << "  - нет пользователей\n";
        }
        else {
            for (const auto& wp : g->users) {
                if (auto u = wp.lock()) { // Если пользователь жив, выводим
                    std::cout << "  - " << u->username << " (ID: " << u->id << ")\n";
                }
            }
        }
    }
};


void printHelp() {
    std::cout << "\nДоступные команды:\n";
    std::cout << "  createUser {userId} {username} {доп. инфо...}\n";
    std::cout << "  deleteUser {userId}\n";
    std::cout << "  allUsers\n";
    std::cout << "  getUser {userId}\n";
    std::cout << "  createGroup {groupId}\n";
    std::cout << "  deleteGroup {groupId}\n";
    std::cout << "  allGroups\n";
    std::cout << "  getGroup {groupId}\n";
    std::cout << "  assignUser {userId} {groupId}  <-- Привязка юзера к группе\n";
    std::cout << "  exit\n";
    std::cout << "> ";
}

int main() {
    std::setlocale(LC_ALL, "Russian");

    SystemManager sys;
    std::string line;

    std::cout << "Система управления пользователями запущена (Умные указатели).";
    printHelp();

    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;

        std::istringstream iss(line);
        std::string cmd;
        iss >> cmd;

        if (cmd == "createUser") {
            std::string id, name, info;
            if (iss >> id >> name) {
                std::getline(iss >> std::ws, info);
                sys.createUser(id, name, info);
            }
            else { std::cout << "Ошибка синтаксиса.\n"; }
        }
        else if (cmd == "deleteUser") { std::string id; if (iss >> id) sys.deleteUser(id); }
        else if (cmd == "allUsers") { sys.allUsers(); }
        else if (cmd == "getUser") { std::string id; if (iss >> id) sys.getUser(id); }
        else if (cmd == "createGroup") { std::string id; if (iss >> id) sys.createGroup(id); }
        else if (cmd == "deleteGroup") { std::string id; if (iss >> id) sys.deleteGroup(id); }
        else if (cmd == "allGroups") { sys.allGroups(); }
        else if (cmd == "getGroup") { std::string id; if (iss >> id) sys.getGroup(id); }
        else if (cmd == "assignUser") { std::string uId, gId; if (iss >> uId >> gId) sys.assignUser(uId, gId); }
        else if (cmd == "exit") { break; }
        else { std::cout << "Неизвестная команда.\n"; }
        std::cout << "> ";
    }

    return 0;
}
