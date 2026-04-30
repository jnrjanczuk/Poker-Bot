#include <vector>
#include <algorithm>
#include <random>
#include <queue>
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
    vector<Card> hand;
    double chipCount;
    bool CPU;
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

struct Hand {
    vector<Player> players;
    queue<Card> cards;
    int bigBlind;
}; // Hand

// Helper Functions

// Shuffle function using Fisher-Yates Shuffle
void shuffle(Deck& deck) {
    static random_device rd;
    static mt19937 gen(rd);
    shuffle(deck.cards.begin(), deck.cards.end(), gen);
} // shuffle

// returns index of next player
int next(int pos, int n) {
    return (pos + 1) % n;
}

// returns index of previous player
int prev(int pos, int n) {
    return (pos - 1) % n;
}

void deal(Hand& hand, Deck& deck) {
    // clear players' previous hands
    for (auto& player : hand.players) {
        player.hand.clear();
    }
    
    // shuffle deck
    shuffle(deck);

    // deal cards to players (starting with small blind)
    int n = hand.players.size();
    int smallBlind = prev(hand.bigBlind, n);
    // outer loop to deal around twice
    for (int i = 0; i < 2; i++) {
        // inner loop to deal to each player
        for (int j = 0; j < n; j++) {
            int index = (smallBlind + i) % n;
            hand.players[index].hand.emplace_back(deck.cards.back());
            deck.cards.pop_back();
        }
    }
} // deal

int main() {
    return 0;
}