#include "../src/Game.h"

#include <cassert>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

void test_start_new_pet() {
    std::istringstream input("");
    std::ostringstream output;
    Game game(input, output);

    game.startNewPet("Mochi", "dog");

    assert(game.hasPet());
    assert(game.getPet() != 0);
    assert(game.getPet()->getType() == "Dog");
    assert(game.getPet()->getName() == "Mochi");
    assert(game.getDay() == 0);
    assert(game.getStepsToday() == 0);
    assert(game.currentStatus().find("Mochi") != std::string::npos);
}

void test_care_actions_advance_time() {
    std::istringstream input("");
    std::ostringstream output;
    Game game(input, output);

    game.startNewPet("Mochi", "1");

    assert(game.performCareAction(1) == "Mochi ate a meal.");
    assert(game.getStepsToday() == 1);
    assert(game.performCareAction(6) == "Mochi went for a walk.");
    assert(game.getStepsToday() == 2);
    assert(game.performCareAction(3) == "Mochi took a nap.");
    assert(game.getStepsToday() == 0);
    assert(game.getDay() == 1);

    const std::vector<std::string> events = game.getLog().toVector();
    assert(events.size() >= 4);
    assert(events[0] == "A new day begins.");
    assert(events[1] == "Mochi took a nap.");
    assert(events[2] == "Mochi went for a walk.");
    assert(events[3] == "Mochi ate a meal.");
}

void test_invalid_pet_type_throws() {
    std::istringstream input("");
    std::ostringstream output;
    Game game(input, output);

    bool threw = false;
    try {
        game.startNewPet("Mystery", "dragon");
    } catch (const std::invalid_argument&) {
        threw = true;
    }

    assert(threw);
    assert(!game.hasPet());
}

void test_invalid_new_pet_preserves_existing_pet() {
    std::istringstream input("");
    std::ostringstream output;
    Game game(input, output);
    game.startNewPet("Mochi", "dog");
    Pet* originalPet = game.getPet();

    bool threw = false;
    try {
        game.startNewPet("Mystery", "dragon");
    } catch (const std::invalid_argument&) {
        threw = true;
    }

    assert(threw);
    assert(game.hasPet());
    assert(game.getPet() == originalPet);
    assert(game.getPet()->getName() == "Mochi");
    assert(game.getPet()->getType() == "Dog");
}

void test_save_and_load_through_game() {
    const std::string path = "build/test_game_save.txt";
    {
        std::istringstream input("");
        std::ostringstream output;
        Game game(input, output);
        game.startNewPet("Luna", "cat");
        game.performCareAction(6);
        game.saveToFile(path);
    }

    std::istringstream input("");
    std::ostringstream output;
    Game loaded(input, output);
    loaded.loadFromFile(path);

    assert(loaded.hasPet());
    assert(loaded.getPet() != 0);
    assert(loaded.getPet()->getType() == "Cat");
    assert(loaded.getPet()->getName() == "Luna");
    assert(loaded.getStepsToday() == 1);

    const std::vector<std::string> events = loaded.getLog().toVector();
    assert(!events.empty());
    assert(events[0] == "Loaded game from " + path + ".");
    assert(events.size() >= 2);
    assert(events[1] == "Saved game to " + path + ".");

    std::remove(path.c_str());
}

void test_failed_load_preserves_existing_pet() {
    std::istringstream input("");
    std::ostringstream output;
    Game game(input, output);
    game.startNewPet("Mochi", "dog");
    game.performCareAction(1);
    Pet* originalPet = game.getPet();
    const std::vector<std::string> originalEvents = game.getLog().toVector();

    bool threw = false;
    try {
        game.loadFromFile("build/missing_dir/missing_save.txt");
    } catch (const std::runtime_error&) {
        threw = true;
    }

    assert(threw);
    assert(game.hasPet());
    assert(game.getPet() == originalPet);
    assert(game.getPet()->getName() == "Mochi");
    assert(game.getStepsToday() == 1);
    assert(game.getLog().toVector() == originalEvents);
}

void test_failed_save_without_pet_does_not_log_success() {
    std::istringstream input("");
    std::ostringstream output;
    Game game(input, output);
    const std::vector<std::string> originalEvents = game.getLog().toVector();

    bool threw = false;
    try {
        game.saveToFile("build/test_game_save.txt");
    } catch (const std::runtime_error&) {
        threw = true;
    }

    assert(threw);
    assert(game.getLog().toVector() == originalEvents);
}

void test_invalid_care_action_throws() {
    std::istringstream input("");
    std::ostringstream output;
    Game game(input, output);
    game.startNewPet("Mochi", "dog");

    bool threw = false;
    try {
        game.performCareAction(99);
    } catch (const std::invalid_argument&) {
        threw = true;
    }

    assert(threw);
    assert(game.getStepsToday() == 0);
}

void test_care_action_rejects_already_dead_pet_without_mutation() {
    std::istringstream input("");
    std::ostringstream output;
    Game game(input, output);
    game.startNewPet("Mochi", "dog");
    game.getPet()->setState(30, 70, 70, 70, 0, 0);
    const int originalLogSize = game.getLog().size();
    const std::string originalStatus = game.currentStatus();

    bool threw = false;
    try {
        game.performCareAction(5);
    } catch (const std::runtime_error&) {
        threw = true;
    }

    assert(threw);
    assert(game.getPet()->getHealth() == 0);
    assert(game.getLog().size() == originalLogSize);
    assert(game.currentStatus() == originalStatus);
}

void test_care_action_logs_when_health_reaches_zero() {
    std::istringstream input("");
    std::ostringstream output;
    Game game(input, output);
    game.startNewPet("Mochi", "dog");
    game.getPet()->setState(100, 70, 0, 70, 1, 0);

    game.performCareAction(1);
    game.performCareAction(1);
    game.performCareAction(1);

    assert(game.getPet()->getHealth() == 0);
    const std::vector<std::string> events = game.getLog().toVector();
    assert(!events.empty());
    assert(events[0] == "Mochi's health reached 0.");
}

void test_run_immediate_quit_prints_start_menu_and_goodbye() {
    std::istringstream input("3\n");
    std::ostringstream output;
    Game game(input, output);

    game.run();

    const std::string text = output.str();
    assert(text.find("Virtual Pet Care") != std::string::npos);
    assert(text.find("1. Start a new pet") != std::string::npos);
    assert(text.find("2. Load a saved pet") != std::string::npos);
    assert(text.find("3. Quit") != std::string::npos);
    assert(text.find("Goodbye.") != std::string::npos);
}

void test_run_scripted_short_session() {
    std::istringstream input("1\nMochi\n1\n1\n6\n7\n10\n3\n");
    std::ostringstream output;
    Game game(input, output);

    game.run();

    const std::string text = output.str();
    assert(text.find("Created Mochi the Dog.") != std::string::npos);
    assert(text.find("Mochi ate a meal.") != std::string::npos);
    assert(text.find("Mochi went for a walk.") != std::string::npos);
    assert(text.find("Mochi the Dog") != std::string::npos);
    assert(text.find("Goodbye.") != std::string::npos);
}

std::string null_padded(const std::string& text) {
    std::string padded;
    for (std::string::const_iterator it = text.begin(); it != text.end(); ++it) {
        padded += *it;
        padded += '\0';
    }
    return padded;
}

void test_run_accepts_null_padded_stdin_tokens() {
    std::istringstream input(std::string(1, '\0') + null_padded("3\n"));
    std::ostringstream output;
    Game game(input, output);

    game.run();

    const std::string text = output.str();
    assert(text.find("Goodbye.") != std::string::npos);
}

void test_run_accepts_utf8_bom_prefixed_stdin() {
    std::istringstream input(std::string("\xEF\xBB\xBF") + "3\n");
    std::ostringstream output;
    Game game(input, output);

    game.run();

    const std::string text = output.str();
    assert(text.find("Goodbye.") != std::string::npos);
}

void test_run_save_uses_normalized_filename_input() {
    const std::string path = "build/run_save_test.txt";
    std::remove(path.c_str());

    std::istringstream input(std::string("1\nMochi\n1\n9\n") + "\xEF\xBB\xBF" + path + "\n10\n3\n");
    std::ostringstream output;
    Game game(input, output);

    game.run();

    const std::string text = output.str();
    assert(text.find("Saved to " + path + ".") != std::string::npos);
    std::ifstream saved(path.c_str());
    assert(saved.good());
    saved.close();
    std::remove(path.c_str());
}

void test_run_final_smoke_save_load_script() {
    const std::string path = "build/final_smoke_save.txt";
    std::remove(path.c_str());

    std::istringstream input(
        "1\n"
        "Mochi\n"
        "1\n"
        "1\n"
        "6\n"
        "8\n"
        "9\n"
        "build/final_smoke_save.txt\n"
        "10\n"
        "2\n"
        "build/final_smoke_save.txt\n"
        "7\n"
        "10\n"
        "3\n");
    std::ostringstream output;
    Game game(input, output);

    game.run();

    const std::string text = output.str();
    assert(text.find("Created Mochi the Dog.") != std::string::npos);
    assert(text.find("Mochi ate a meal.") != std::string::npos);
    assert(text.find("Mochi went for a walk.") != std::string::npos);
    assert(text.find("Saved to " + path + ".") != std::string::npos);
    assert(text.find("Loaded Mochi the Dog.") != std::string::npos);
    assert(text.find("Mochi the Dog") != std::string::npos);
    assert(text.find("Goodbye.") != std::string::npos);

    std::ifstream saved(path.c_str());
    assert(saved.good());
    saved.close();
    std::remove(path.c_str());
}

void test_run_ends_pet_session_when_care_action_causes_death() {
    const std::string path = "build/test_game_dying_pet_save.txt";
    std::remove(path.c_str());
    {
        std::ofstream save(path.c_str());
        save << "PET_SAVE_V1\n"
             << "type Dog\n"
             << "name Mochi\n"
             << "day 0\n"
             << "stepsToday 0\n"
             << "hunger 100\n"
             << "happiness 70\n"
             << "energy 0\n"
             << "cleanliness 70\n"
             << "health 1\n"
             << "events 0\n"
             << "END\n";
    }

    std::istringstream input("2\nbuild/test_game_dying_pet_save.txt\n1\n1\n1\n3\n");
    std::ostringstream output;
    Game game(input, output);

    game.run();

    const std::string text = output.str();
    const std::string gameOver = "Game over.";
    const std::string startMenu = "Virtual Pet Care";
    const std::string::size_type gameOverPosition = text.find(gameOver);
    assert(gameOverPosition != std::string::npos);
    assert(text.find(startMenu, gameOverPosition + gameOver.size()) != std::string::npos);
    assert(text.find("Goodbye.", gameOverPosition + gameOver.size()) != std::string::npos);

    std::remove(path.c_str());
}

void test_run_loaded_dead_pet_ends_session_before_care_menu() {
    const std::string path = "build/test_game_dead_pet_save.txt";
    std::remove(path.c_str());
    {
        std::ofstream save(path.c_str());
        save << "PET_SAVE_V1\n"
             << "type Dog\n"
             << "name Ghost\n"
             << "day 4\n"
             << "stepsToday 2\n"
             << "hunger 30\n"
             << "happiness 70\n"
             << "energy 70\n"
             << "cleanliness 70\n"
             << "health 0\n"
             << "events 0\n"
             << "END\n";
    }

    std::istringstream input("2\nbuild/test_game_dead_pet_save.txt\n3\n");
    std::ostringstream output;
    Game game(input, output);

    game.run();

    const std::string text = output.str();
    assert(text.find("Loaded Ghost the Dog.") != std::string::npos);
    assert(text.find("Game over.") != std::string::npos);
    assert(text.find("Goodbye.") != std::string::npos);

    std::remove(path.c_str());
}

}

int main() {
    test_start_new_pet();
    test_care_actions_advance_time();
    test_invalid_pet_type_throws();
    test_invalid_new_pet_preserves_existing_pet();
    test_save_and_load_through_game();
    test_failed_load_preserves_existing_pet();
    test_failed_save_without_pet_does_not_log_success();
    test_invalid_care_action_throws();
    test_care_action_rejects_already_dead_pet_without_mutation();
    test_care_action_logs_when_health_reaches_zero();
    test_run_immediate_quit_prints_start_menu_and_goodbye();
    test_run_scripted_short_session();
    test_run_accepts_null_padded_stdin_tokens();
    test_run_accepts_utf8_bom_prefixed_stdin();
    test_run_save_uses_normalized_filename_input();
    test_run_final_smoke_save_load_script();
    test_run_ends_pet_session_when_care_action_causes_death();
    test_run_loaded_dead_pet_ends_session_before_care_menu();

    return 0;
}
