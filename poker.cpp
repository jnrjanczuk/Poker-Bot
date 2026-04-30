#include <vector>
#include <algorithm>
#include <random>
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
    pair<Card, Card> hand;
    double chipCount;
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

}; // Game

// Helper Functions

// Shuffle function using Fisher-Yates Shuffle
void shuffle(Deck& deck) {
    static random_device rd;
    static mt19937 gen(rd);
    shuffle(deck.cards.begin(), deck.cards.end(), gen);
} // shuffle

int main() {
    return 0;
}