#include "SaveManager.h"

#include <cstdlib>
#include <fstream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace {

std::string readRequiredLine(std::istream& in, const std::string& fieldName) {
    std::string line;
    if (!std::getline(in, line)) {
        throw std::runtime_error("Missing " + fieldName + " in save file.");
    }
    return line;
}

}

LoadResult::LoadResult()
    : pet(0), stepsToday(0), log() {
}

void SaveManager::save(const std::string& filename, const Pet& pet, int stepsToday, const EventLog& log) {
    std::ofstream out(filename.c_str());
    if (!out) {
        throw std::runtime_error("Could not open save file for writing: " + filename);
    }

    out << "PET_SAVE_V1\n";
    out << "type " << pet.getType() << '\n';
    out << "name " << pet.getName() << '\n';
    out << "day " << pet.getAgeDays() << '\n';
    out << "stepsToday " << stepsToday << '\n';
    out << "hunger " << pet.getHunger() << '\n';
    out << "happiness " << pet.getHappiness() << '\n';
    out << "energy " << pet.getEnergy() << '\n';
    out << "cleanliness " << pet.getCleanliness() << '\n';
    out << "health " << pet.getHealth() << '\n';

    const std::vector<std::string> events = log.toVector();
    out << "events " << events.size() << '\n';
    for (std::vector<std::string>::const_iterator it = events.begin(); it != events.end(); ++it) {
        out << *it << '\n';
    }
    out << "END\n";

    if (!out) {
        throw std::runtime_error("Failed while writing save file: " + filename);
    }
}

LoadResult SaveManager::load(const std::string& filename) {
    std::ifstream in(filename.c_str());
    if (!in) {
        throw std::runtime_error("Could not open save file for reading: " + filename);
    }

    const std::string header = readRequiredLine(in, "header");
    if (header != "PET_SAVE_V1") {
        throw std::runtime_error("Invalid save file header.");
    }

    const std::string type = valueAfterKey(readRequiredLine(in, "type"), "type");
    const std::string name = valueAfterKey(readRequiredLine(in, "name"), "name");
    if (name.empty()) {
        throw std::runtime_error("Pet name cannot be empty.");
    }

    const int day = parseInt(valueAfterKey(readRequiredLine(in, "day"), "day"), "day");
    if (day < 0) {
        throw std::runtime_error("day must be at least 0.");
    }

    const int stepsToday = parseInt(valueAfterKey(readRequiredLine(in, "stepsToday"), "stepsToday"), "stepsToday");
    if (stepsToday < 0 || stepsToday > 2) {
        throw std::runtime_error("stepsToday must be between 0 and 2.");
    }

    const int hunger = parseInt(valueAfterKey(readRequiredLine(in, "hunger"), "hunger"), "hunger");
    const int happiness = parseInt(valueAfterKey(readRequiredLine(in, "happiness"), "happiness"), "happiness");
    const int energy = parseInt(valueAfterKey(readRequiredLine(in, "energy"), "energy"), "energy");
    const int cleanliness = parseInt(valueAfterKey(readRequiredLine(in, "cleanliness"), "cleanliness"), "cleanliness");
    const int health = parseInt(valueAfterKey(readRequiredLine(in, "health"), "health"), "health");

    requireStatRange(hunger, "hunger");
    requireStatRange(happiness, "happiness");
    requireStatRange(energy, "energy");
    requireStatRange(cleanliness, "cleanliness");
    requireStatRange(health, "health");

    const int eventCount = parseInt(valueAfterKey(readRequiredLine(in, "events"), "events"), "events");
    if (eventCount < 0) {
        throw std::runtime_error("events must be at least 0.");
    }

    std::vector<std::string> events;
    for (int index = 0; index < eventCount; ++index) {
        events.push_back(readRequiredLine(in, "event"));
    }

    const std::string endMarker = readRequiredLine(in, "END marker");
    if (endMarker != "END") {
        throw std::runtime_error("Missing END marker in save file.");
    }

    std::unique_ptr<Pet> pet(createPet(type, name));
    pet->setState(hunger, happiness, energy, cleanliness, health, day);

    LoadResult result;
    result.stepsToday = stepsToday;
    for (std::vector<std::string>::reverse_iterator it = events.rbegin(); it != events.rend(); ++it) {
        result.log.add(*it);
    }
    result.pet = pet.release();

    return result;
}

std::string SaveManager::valueAfterKey(const std::string& line, const std::string& key) {
    const std::string prefix = key + " ";
    if (line.size() < prefix.size() || line.substr(0, prefix.size()) != prefix) {
        throw std::runtime_error("Expected field: " + key);
    }

    return line.substr(prefix.size());
}

int SaveManager::parseInt(const std::string& text, const std::string& fieldName) {
    std::istringstream input(text);
    int value = 0;
    char extra = '\0';
    if (!(input >> value) || (input >> extra)) {
        throw std::runtime_error("Invalid integer for " + fieldName + ": " + text);
    }

    return value;
}

void SaveManager::requireStatRange(int value, const std::string& fieldName) {
    if (value < 0 || value > 100) {
        throw std::runtime_error(fieldName + " must be between 0 and 100.");
    }
}

Pet* SaveManager::createPet(const std::string& type, const std::string& name) {
    if (type == "Dog") {
        return new Dog(name);
    }
    if (type == "Cat") {
        return new Cat(name);
    }
    if (type == "Rabbit") {
        return new Rabbit(name);
    }

    throw std::runtime_error("Unknown pet type: " + type);
}
