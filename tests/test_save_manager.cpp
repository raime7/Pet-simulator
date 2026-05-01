#include "../src/SaveManager.h"

#include <cassert>
#include <cstdio>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

void writeFile(const std::string& path, const std::string& contents) {
    std::ofstream out(path.c_str());
    out << contents;
}

bool loadThrowsRuntimeError(const std::string& path) {
    try {
        LoadResult loaded = SaveManager::load(path);
        delete loaded.pet;
    } catch (const std::runtime_error&) {
        return true;
    }

    return false;
}

}

int main() {
    const std::string roundTripPath = "build/test_save_round_trip.txt";
    Dog dog("Mochi");
    dog.setState(35, 82, 60, 70, 90, 4);

    EventLog log;
    log.add("Saved game.");
    log.add("Mochi went for a walk.");
    log.add("Fed Mochi.");

    SaveManager::save(roundTripPath, dog, 1, log);
    LoadResult loaded = SaveManager::load(roundTripPath);

    assert(loaded.pet != 0);
    Dog* loadedDog = dynamic_cast<Dog*>(loaded.pet);
    assert(loadedDog != 0);
    assert(loadedDog->getName() == "Mochi");
    assert(loadedDog->getAgeDays() == 4);
    assert(loadedDog->getHunger() == 35);
    assert(loadedDog->getHappiness() == 82);
    assert(loadedDog->getEnergy() == 60);
    assert(loadedDog->getCleanliness() == 70);
    assert(loadedDog->getHealth() == 90);
    assert(loaded.stepsToday == 1);

    const std::vector<std::string> loadedEvents = loaded.log.toVector();
    assert(loadedEvents.size() == 3);
    assert(loadedEvents[0] == "Fed Mochi.");
    assert(loadedEvents[1] == "Mochi went for a walk.");
    assert(loadedEvents[2] == "Saved game.");

    delete loaded.pet;
    std::remove(roundTripPath.c_str());

    const std::string badHeaderPath = "build/test_save_bad_header.txt";
    writeFile(badHeaderPath,
              "WRONG_HEADER\n"
              "type Dog\n"
              "name Mochi\n"
              "day 4\n"
              "stepsToday 1\n"
              "hunger 35\n"
              "happiness 82\n"
              "energy 60\n"
              "cleanliness 70\n"
              "health 90\n"
              "events 0\n"
              "END\n");
    assert(loadThrowsRuntimeError(badHeaderPath));
    std::remove(badHeaderPath.c_str());

    const std::string badStatPath = "build/test_save_bad_stat.txt";
    writeFile(badStatPath,
              "PET_SAVE_V1\n"
              "type Dog\n"
              "name Mochi\n"
              "day 4\n"
              "stepsToday 1\n"
              "hunger 101\n"
              "happiness 82\n"
              "energy 60\n"
              "cleanliness 70\n"
              "health 90\n"
              "events 0\n"
              "END\n");
    assert(loadThrowsRuntimeError(badStatPath));
    std::remove(badStatPath.c_str());

    const std::string malformedDayPath = "build/test_save_malformed_day.txt";
    writeFile(malformedDayPath,
              "PET_SAVE_V1\n"
              "type Dog\n"
              "name Mochi\n"
              "day 4abc\n"
              "stepsToday 1\n"
              "hunger 35\n"
              "happiness 82\n"
              "energy 60\n"
              "cleanliness 70\n"
              "health 90\n"
              "events 0\n"
              "END\n");
    assert(loadThrowsRuntimeError(malformedDayPath));
    std::remove(malformedDayPath.c_str());

    return 0;
}
