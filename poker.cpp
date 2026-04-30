#include <vector>
#include <algorithm>
#include <random>
#include <queue>
#include <iostream>
using namespace std;

// Enum Classes

enum Suit { Spades, Clubs, Hearts, Diamonds };

enum Rank {ACE = 1, TWO, THREE, FOUR, FIVE, SIX,
            SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING };

// Custom Containers

struct Card {
    Suit suit;
    Rank rank;

    // Card constructor
    Card(int s, int r) {
        suit = static_cast<Suit>(s);
        rank = static_cast<Rank>(r);
    }
}; // Card

struct Player {
    string name;
    vector<Card> hand;
    int chipCount = 0;
    bool CPU = true;

    // Player constructor
    Player(string n, bool b) {
        name = n;
        CPU = b;
    }
}; // Player

struct Deck {
    vector<Card> cards;

    // Deck default constructor
    Deck() {
        cards.reserve(52);
        for (int s = 0; s < 4; s++) {
            for (int r = 0; r < 13; r++) {
                cards.push_back(Card(s, r));
            }
        }
    }
}; // Deck

struct Game {
    vector<Player> players;
    queue<Card> cards;
    size_t bigBlind;
    vector<Card> board;
}; // Game

// Helper Functions

// Shuffle function using Fisher-Yates Shuffle
void shuffle(Deck& deck) {
    static random_device rd;
    static mt19937 gen(rd());
    shuffle(deck.cards.begin(), deck.cards.end(), gen);
} // shuffle

// returns index of next player
size_t next(size_t pos, size_t n) {
    return (pos + 1) % n;
}

// returns index of previous player
size_t prev(size_t pos, size_t n) {
    return (pos - 1) % n;
}

void deal(Game& game, Deck& deck) {
    // clear board
    game.board.clear();
    
    // clear players' previous hands
    for (auto& player : game.players) {
        player.hand.clear();
    }

    
    // shuffle deck
    shuffle(deck);

    // deal cards to players (starting with small blind)
    size_t n = game.players.size();
    size_t smallBlind = prev(game.bigBlind, n);
    // outer loop to deal around twice
    for (size_t i = 0; i < 2; i++) {
        // inner loop to deal to each player
        for (size_t j = 0; j < n; j++) {
            size_t index = (smallBlind + i) % n;
            game.players[index].hand.emplace_back(deck.cards.back());
            deck.cards.pop_back();
        }
    }
} // deal

void dealFlop(Game& game, Deck& deck) {
    for (int i = 0; i < 3; i++) {
        game.board.emplace_back(deck.cards.back());
        deck.cards.pop_back();
    }
} // dealFlop

void dealOne(Game& game, Deck& deck) { // (for dealing turn and river)
    game.board.emplace_back(deck.cards.back());
    deck.cards.pop_back();
} // dealOne

void playTurn(Player& player, int currBet) {
    return; // placeholder
}

void playHand(Game& game, Deck& deck) {
    return; // placeholder
} // playHand

// Main Functionality

int main() {
    // welcome message
    cout << "Welcome to No-Limit Hold 'Em! Please enter your name: ";
    string name;
    cin >> name;
    cout << "Welcome to the table " << name << "! Blinds are 50/100\n"; // change blinds if needed?
    cout << "Would you like to learn how to play? (Y/N)";
    char input;
    cin >> input;
    if (toupper(input) == 'Y') {
        cout << "<help message here>";
    }
    
    // create game object and human player
    Game game;
    game.players.emplace_back(Player(name, false));
    
    // create CPU players
    for (int i = 1; i < 6; i++) {
        string name = "bot";
        name += static_cast<char>(i);
        game.players.emplace_back(Player(name, true));
    }


    
    return 0;
}