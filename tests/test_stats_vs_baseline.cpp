#include "player/my_player.hpp"
#include "core/baseline.hpp"
#include "test_stats.hpp"
#include <iostream>
#include <cstdlib>
#include <fstream>

int main(int argc, char *argv[]) {
    std::cout << "Testing MyBot vs baseline easy player\n";
    if (argc >= 2) {
        std::srand(atoi(argv[1]));
    }

    ttt::my_player::MyPlayer p1("MyBot");
    
    // ttt::game::IPlayer *p2 = ttt::baseline::get_harder_player("BaselineHard");
    ttt::game::IPlayer *p2 = ttt::baseline::get_easy_player("BaselineEasy");

    auto result = ttt::test::run_game_tests(p1, *p2, 100);

    ttt::test::print_test_results(result, "MyBot", "BaselineEasy");
    
    std::ofstream file("test_results.md");
    int decisive = result.x_wins + result.o_wins;
    double win_rate = decisive > 0 ? (100.0 * result.x_wins / decisive) : 0.0;
    
    file << "# Test Results: MyBot vs Easy Baseline\n\n";
    file << "## Parameters\n- Games: 100\n- Board: 20x20\n- WinLen: 5\n\n";
    file << "## Results\n";
    file << "- MyBot wins: " << result.x_wins << "\n";
    file << "- BaselineEasy wins: " << result.o_wins << "\n";
    file << "- Draws: " << result.draws << "\n";
    file << "- Errors: " << result.errors << "\n";
    file << "- **Win Rate: " << win_rate << "%**\n\n";
    file << "## Timing\n";
    file << "- MyBot move time: " << result.x_move_time << " ms\n";
    file << "- MyBot event time: " << result.x_event_time << " ms\n\n";
    file << (win_rate > 50.0 ? "Passed\n" : "Failed\n");
    file.close();
    
    std::cout << "\nResults saved to test_results.md\n";
    
    delete p2;
    return 0;
}