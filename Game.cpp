#include "Game.h"

#include "SaveManager.h"

#include <algorithm>
#include <cctype>
#include <memory>
#include <sstream>
#include <stdexcept>

namespace {

std::string toLower(std::string text) {
    std::transform(text.begin(), text.end(), text.begin(),
                   static_cast<int (*)(int)>(std::tolower));
    return text;
}

std::string readToken(std::istream& input) {
    std::string token;
    char ch = '\0';
    while (input.get(ch)) {
        const unsigned char byte = static_cast<unsigned char>(ch);
        if (ch == '\0') {
            continue;
        }

        if (token.empty() && (byte == 0xEF || byte == 0xBB || byte == 0xBF)) {
            continue;
        }

        if (std::isspace(byte)) {
            if (token.empty()) {
                continue;
            }
            break;
        }

        token += ch;
    }

    return token;
}

}

Game::Game(std::istream& inputStream, std::ostream& outputStream)
    : input_(inputStream),
      output_(outputStream),
      pet_(0),
      log_(),
      stepsToday_(0) {
}

Game::~Game() {
    delete pet_;
}

void Game::run() {
    bool running = true;
    while (running && input_) {
        printStartMenu();
        const int choice = readInt();
        try {
            switch (choice) {
            case 1:
                handleNewPet();
                if (hasPet()) {
                    runPetSession();
                }
                break;
            case 2:
                handleLoad();
                if (hasPet()) {
                    runPetSession();
                }
                break;
            case 3:
                output_ << "Goodbye.\n";
                running = false;
                break;
            default:
                output_ << "Invalid choice.\n";
                break;
            }
        } catch (const std::exception& error) {
            output_ << error.what() << '\n';
        }
    }
}

void Game::startNewPet(const std::string& name, const std::string& typeChoice) {
    Pet* newPet = createPet(name, typeChoice);
    replacePet(newPet);
    stepsToday_ = 0;
    log_.clear();
    log_.add("Started caring for " + pet_->getName() + " the " + pet_->getType() + ".");
}

void Game::loadFromFile(const std::string& filename) {
    LoadResult result = SaveManager::load(filename);
    std::unique_ptr<Pet> loadedPet(result.pet);
    result.pet = 0;

    EventLog loadedLog(result.log);
    loadedLog.add("Loaded game from " + filename + ".");
    const int loadedStepsToday = result.stepsToday;

    log_.swap(loadedLog);
    replacePet(loadedPet.release());
    stepsToday_ = loadedStepsToday;
}

void Game::saveToFile(const std::string& filename) {
    if (!pet_) {
        throw std::runtime_error("No pet to save.");
    }

    EventLog savedLog(log_);
    savedLog.add("Saved game to " + filename + ".");
    SaveManager::save(filename, *pet_, stepsToday_, savedLog);
    log_.swap(savedLog);
}

bool Game::hasPet() const {
    return pet_ != 0;
}

int Game::getStepsToday() const {
    return stepsToday_;
}

int Game::getDay() const {
    return pet_ ? pet_->getAgeDays() : 0;
}

Pet* Game::getPet() const {
    return pet_;
}

EventLog& Game::getLog() {
    return log_;
}

std::string Game::currentStatus() const {
    if (!pet_) {
        return "No pet.";
    }

    std::ostringstream status;
    status << pet_->statusLine() << " | Steps today: " << stepsToday_ << "/3";
    return status.str();
}

std::string Game::performCareAction(int choice) {
    if (!pet_) {
        throw std::runtime_error("No pet to care for.");
    }
    if (!pet_->isAlive()) {
        throw std::runtime_error("Game over. No care actions can be performed.");
    }

    std::string message;
    switch (choice) {
    case 1:
        pet_->feed();
        message = pet_->getName() + " ate a meal.";
        break;
    case 2:
        pet_->play();
        message = "You played with " + pet_->getName() + ".";
        break;
    case 3:
        pet_->rest();
        message = pet_->getName() + " took a nap.";
        break;
    case 4:
        pet_->clean();
        message = pet_->getName() + " got cleaned up.";
        break;
    case 5:
        pet_->visitDoctor();
        message = pet_->getName() + " visited the doctor.";
        break;
    case 6:
        message = pet_->specialAction();
        break;
    default:
        throw std::invalid_argument("Invalid care action.");
    }

    log_.add(message);
    advanceTime();
    if (!pet_->isAlive()) {
        log_.add(pet_->getName() + "'s health reached 0.");
    }

    return message;
}

void Game::replacePet(Pet* pet) {
    if (pet_ != pet) {
        delete pet_;
        pet_ = pet;
    }
}

Pet* Game::createPet(const std::string& name, const std::string& typeChoice) const {
    const std::string normalized = toLower(typeChoice);
    if (normalized == "1" || normalized == "dog") {
        return new Dog(name);
    }
    if (normalized == "2" || normalized == "cat") {
        return new Cat(name);
    }
    if (normalized == "3" || normalized == "rabbit") {
        return new Rabbit(name);
    }

    throw std::invalid_argument("Unknown pet type: " + typeChoice);
}

void Game::advanceTime() {
    ++stepsToday_;
    if (stepsToday_ >= 3) {
        stepsToday_ = 0;
        pet_->advanceDay();
        log_.add("A new day begins.");
    }
}

void Game::printStartMenu() const {
    output_ << "\nVirtual Pet Care\n"
            << "1. Start a new pet\n"
            << "2. Load a saved pet\n"
            << "3. Quit\n"
            << "Choice: ";
}

void Game::printMainMenu() const {
    output_ << "\n1. Feed\n"
            << "2. Play\n"
            << "3. Rest\n"
            << "4. Clean\n"
            << "5. Visit doctor\n"
            << "6. Special action\n"
            << "7. Status\n"
            << "8. Recent events\n"
            << "9. Save\n"
            << "10. Back to start menu\n"
            << "Choice: ";
}

int Game::readInt() {
    const std::string token = readToken(input_);
    if (token.empty()) {
        return -1;
    }

    std::istringstream parser(token);
    int choice = -1;
    char extra = '\0';
    if ((parser >> choice) && !(parser >> extra)) {
        return choice;
    }

    return -1;
}

std::string Game::readLine() {
    return readToken(input_);
}

void Game::handleNewPet() {
    output_ << "Pet name: ";
    const std::string name = readLine();
    output_ << "Type (1 dog, 2 cat, 3 rabbit): ";
    const std::string typeChoice = readLine();
    if (input_) {
        startNewPet(name, typeChoice);
        output_ << "Created " << pet_->getName() << " the " << pet_->getType() << ".\n";
    }
}

void Game::handleLoad() {
    output_ << "Load file: ";
    const std::string filename = readLine();
    if (input_) {
        loadFromFile(filename);
        output_ << "Loaded " << pet_->getName() << " the " << pet_->getType() << ".\n";
    }
}

void Game::runPetSession() {
    bool inSession = true;
    while (inSession && input_) {
        if (pet_ && !pet_->isAlive()) {
            output_ << "Game over.\n";
            inSession = false;
            break;
        }

        printMainMenu();
        const int choice = readInt();
        try {
            switch (choice) {
            case 1:
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
                output_ << performCareAction(choice) << '\n';
                if (pet_ && !pet_->isAlive()) {
                    output_ << "Game over.\n";
                    inSession = false;
                }
                break;
            case 7:
                output_ << currentStatus() << '\n';
                break;
            case 8:
                log_.printRecent(10, output_);
                break;
            case 9: {
                output_ << "Save file: ";
                const std::string filename = readLine();
                if (input_) {
                    saveToFile(filename);
                    output_ << "Saved to " << filename << ".\n";
                }
                break;
            }
            case 10:
                inSession = false;
                break;
            default:
                output_ << "Invalid choice.\n";
                break;
            }
        } catch (const std::exception& error) {
            output_ << error.what() << '\n';
        }
    }
}

void Game::playCurrentPet() {
    if (!pet_) {
        output_ << "No pet to care for.\n";
        return;
    }

    printMainMenu();
    const int choice = readInt();
    const std::string message = performCareAction(choice);
    output_ << message << '\n';
}
