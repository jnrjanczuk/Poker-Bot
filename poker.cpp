#include <vector>

using namespace std;

// Enum Classes

enum Suit { Spades, Clubs, Hearts, Diamonds };

enum Rank {ACE = 1, TWO, THREE, FOUR, FIVE, SIX,
            SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING };

// Custom Containers

struct Card {
    Suit suit;
    Rank rank;
}; // Card

struct Player {
    pair<Card, Card> hand;
    double chipCount;
}; // Player

struct Deck {
    vector<Card> deck;

    Deck() {
        deck.reserve(52);
    }
}; // Deck

struct Game {

}; // Game

// Helper Functions

void shuffle(Deck& deck) {

} // shuffle

int main() {
    return 0;
}