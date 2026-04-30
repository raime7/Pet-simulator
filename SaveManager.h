#ifndef SAVE_MANAGER_H
#define SAVE_MANAGER_H

#include "EventLog.h"
#include "Pet.h"

#include <string>

struct LoadResult {
    LoadResult();

    Pet* pet;
    int stepsToday;
    EventLog log;
};

class SaveManager {
public:
    static void save(const std::string& filename, const Pet& pet, int stepsToday, const EventLog& log);
    static LoadResult load(const std::string& filename);

private:
    static std::string valueAfterKey(const std::string& line, const std::string& key);
    static int parseInt(const std::string& text, const std::string& fieldName);
    static void requireStatRange(int value, const std::string& fieldName);
    static Pet* createPet(const std::string& type, const std::string& name);
};

#endif
