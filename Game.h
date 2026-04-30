#ifndef GAME_H
#define GAME_H

#include "EventLog.h"
#include "Pet.h"

#include <iosfwd>
#include <string>

class Game {
public:
    Game(std::istream& inputStream, std::ostream& outputStream);
    ~Game();

    void run();
    void startNewPet(const std::string& name, const std::string& typeChoice);
    void loadFromFile(const std::string& filename);
    void saveToFile(const std::string& filename);

    bool hasPet() const;
    int getStepsToday() const;
    int getDay() const;
    Pet* getPet() const;
    EventLog& getLog();

    std::string currentStatus() const;
    std::string performCareAction(int choice);

private:
    Game(const Game&);
    Game& operator=(const Game&);

    void replacePet(Pet* pet);
    Pet* createPet(const std::string& name, const std::string& typeChoice) const;
    void advanceTime();
    void printStartMenu() const;
    void printMainMenu() const;
    int readInt();
    std::string readLine();
    void handleNewPet();
    void handleLoad();
    void runPetSession();
    void playCurrentPet();

    std::istream& input_;
    std::ostream& output_;
    Pet* pet_;
    EventLog log_;
    int stepsToday_;
};

#endif
