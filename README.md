# Pet-simulator
=======
# Virtual Pet Care

Virtual Pet Care is a command-line C++ project practicing CSCI 103 concepts. Create or load one pet, care for it through menu choices, watch its stats change over time, and save it to a readable text file.

## Build

Run these commands from the project root in PowerShell:

```powershell
if (!(Test-Path build)) { New-Item -ItemType Directory build | Out-Null }
g++ -std=c++11 -Wall -Wextra -pedantic src/main.cpp src/Pet.cpp src/EventLog.cpp src/SaveManager.cpp src/Game.cpp -o build/pet_simulator.exe
```

## Run

```powershell
.\build\pet_simulator.exe
```

## Tests

Compile and run each test from the project root in PowerShell:

```powershell
if (!(Test-Path build)) { New-Item -ItemType Directory build | Out-Null }

g++ -std=c++11 -Wall -Wextra -pedantic tests/test_pet.cpp src/Pet.cpp -o build/test_pet.exe
.\build\test_pet.exe

g++ -std=c++11 -Wall -Wextra -pedantic tests/test_event_log.cpp src/EventLog.cpp -o build/test_event_log.exe
.\build\test_event_log.exe

g++ -std=c++11 -Wall -Wextra -pedantic tests/test_save_manager.cpp src/Pet.cpp src/EventLog.cpp src/SaveManager.cpp -o build/test_save_manager.exe
.\build\test_save_manager.exe

g++ -std=c++11 -Wall -Wextra -pedantic tests/test_game.cpp src/Pet.cpp src/EventLog.cpp src/SaveManager.cpp src/Game.cpp -o build/test_game.exe
.\build\test_game.exe
```

## Features

- Choose a dog, cat, or rabbit.
- Care for your pet with menu actions such as feeding, walking, playing, sleeping, and vet visits.
- Track stats including hunger, happiness, energy, health, cleanliness, and age.
- Advance to a new day after three care actions.
- Review a hand-written linked list event log.
- Save and load a readable text file.

## CSCI 103 Concepts

- Classes and inheritance.
- Polymorphism through `Pet*`.
- Dynamic allocation with `new` and `delete`.
- Linked lists and copy semantics.
- File streams and parsing.
- Exceptions.
- `assert` tests.
