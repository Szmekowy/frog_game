# Frog Game

### General info

Frog Game is an arcade-style game inspired by Crossy Road. The player controls a frog attempting to cross a busy road while avoiding cars. Unlike traditional obstacle-dodging games, this version introduces a unique mechanic where green cars can provide the player with a ride to safety. Additionally, a stalking stork adds an extra layer of challenge, keeping the player on their toes. The game was developed using C/C++ with the ncurses library.
This game was created as a project during the first semester of Computer Science at Gdańsk University of Technology.

#### Key Features:

* Dynamic Traffic System: Cars move across the screen, and collisions result in a loss.

* Ride Mechanic: Green cars allow the player to hitch a ride, offering strategic movement opportunities.

* Challenging AI Opponent: A stork chases the player, adding urgency to movement decisions.

* Point System: Players collect coins to increase their score.

* Save & Load Feature: The game state can be saved and reloaded.

### Technologies

The project is created with:

* C/C++

* ncurses library

### Setup

To compile and run the game, follow these steps:

1) Ensure you have ncurses installed on your system.

    a) On Ubuntu/Debian: sudo apt-get install libncurses5-dev libncursesw5-dev

    b) On macOS: brew install ncurses

2) Clone the repository

3) Navigate to the project directory: cd frog_game

4) Compile the game: g++ -o frog_game frog.cpp -lncurses

5) Run the game: ./frog_game

### How to play

* Use WSAD to move the frog.

* Avoid collisions with cars, except green ones, which offer a ride.

* Collect coins by pressing Q when near them to earn points.

* Save the game by pressing P.

* Beware of the stork, which chases you throughout the level.

* Reach the other side of the road safely to win!

### Conclusions

The game is currently fully functional, but future improvements could include:

* Enhanced AI for the stork to make its movements more unpredictable.

* Additional levels with different environmental challenges.

* Improved visual feedback using more advanced graphics libraries.

### License

This project is licensed under the MIT License.

### Author & Contact

This project is maintained by Szymon Drywa.

For any suggestions, feedback, or inquiries, please reach out via:

Email: szmekdrywa@wp.pl

GitHub: https://github.com/Szmekowy
