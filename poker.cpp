#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <random>
using namespace std;

// Enum Classes

enum class Suit { Hearts, Diamonds, Clubs, Spades };

enum class Rank { TWO = 2, THREE, FOUR, FIVE, SIX,
            SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING, ACE };

enum class Street { PREFLOP, FLOP, TURN, RIVER };

// Custom Containers

struct Card {
    Suit suit;
    Rank rank;
}; // Card

struct Deck {
    vector<Card> cards;
}; // Deck

struct GameState {
    vector<Card> hole_cards;
    vector<Card> board;

    int pot;
    int stack;
    int bet;
    int position;   // 0 = UTG, 1 = MP, etc.
    int players;

    Street street;
}; // GameState

// Helper Functions

void initializeDeck(Deck& deck) {
    deck.cards.clear();
    for (int suit = 0; suit < 4; suit++) {
        for (int rank = 2; rank <= 14; rank++) {
            Card card;
            card.suit = static_cast<Suit>(suit);
            card.rank = static_cast<Rank>(rank);
            deck.cards.push_back(card);
        }
    }
} // initializeDeck

void shuffleDeck(Deck& deck) {
    random_device rd;
    mt19937 gen(rd());
    shuffle(deck.cards.begin(), deck.cards.end(), gen);
} // shuffleDeck

Card dealCard(Deck& deck) {
    Card card = deck.cards.back();
    deck.cards.pop_back();
    return card;
} // dealCard

void removeKnown(Deck& deck, const Card& target) {
    for (auto it = deck.cards.begin(); it != deck.cards.end(); it++) {
        if (it->rank == target.rank && it->suit == target.suit) {
            deck.cards.erase(it);
            return;
        }
    }
} // removeKnown

int evaluateHand() {
    return 1; // placeholder, need to code
} // evaluateHand

Card parseCard(string s) {
    Card card;

    // read in rank
    if (s[0] == 'J') { card.rank = Rank::JACK; }
    else if (s[0] == 'Q') { card.rank = Rank::QUEEN; }
    else if (s[0] == 'K') { card.rank = Rank::KING; }
    else if (s[0] == 'A') { card.rank = Rank::ACE; }
    else {
        card.rank = static_cast<Rank>(s[0]);
    }

    // read in suit
    if (s[1] == 'h') { card.suit = Suit::Hearts; }
    else if (s[1] == 'd') { card.suit = Suit::Diamonds; }
    else if (s[1] == 'c') { card.suit = Suit::Clubs; }
    else { card.suit = Suit::Spades; }

    return card;
} // parseCard

void getStreet(GameState& game) {
    char input;
    cout << "Enter street (enter the first letter of preflop/flop/turn/river): ";
    cin >> input;
    if (input == 'p') { game.street = Street::PREFLOP; }
    else if (input == 'f') { game.street = Street::FLOP; }
    else if (input == 't') { game.street = Street::TURN; }
    else { game.street = Street::RIVER; }
} // getStreet

void getHandInfo(GameState& game) {
    // clear previous cards
    game.hole_cards.clear();
    game.board.clear();
    
    // read in hole cards
    string c1, c2;
    cout << "Enter hole cards: ";
    cin >> c1 >> c2;
    game.hole_cards.push_back(parseCard(c1));
    game.hole_cards.push_back(parseCard(c2));
    
    // check street, read in board cards if necessary
    getStreet(game);
    if (game.street == Street::FLOP) {
        string b1, b2, b3;
        cout << "Enter board cards: ";
        cin >> b1 >> b2 >> b3;
        game.board.push_back(parseCard(b1));
        game.board.push_back(parseCard(b2));
        game.board.push_back(parseCard(b3));
    }
    else if (game.street == Street::TURN) {
        string b1, b2, b3, b4;
        cout << "Enter board cards: ";
        cin >> b1 >> b2 >> b3 >> b4;
        game.board.push_back(parseCard(b1));
        game.board.push_back(parseCard(b2));
        game.board.push_back(parseCard(b3));
        game.board.push_back(parseCard(b4));
    }
    else if (game.street == Street::RIVER) {
        string b1, b2, b3, b4, b5;
        cout << "Enter board cards: ";
        cin >> b1 >> b2 >> b3 >> b4 >> b5;
        game.board.push_back(parseCard(b1));
        game.board.push_back(parseCard(b2));
        game.board.push_back(parseCard(b3));
        game.board.push_back(parseCard(b4));
        game.board.push_back(parseCard(b5));
    }

    // read in betting info
    int pot, bet, stack;
    cout << "Enter pot size: ";
    cin >> pot;
    game.pot = pot;
    cout << "Enter bet to call: ";
    cin >> bet;
    game.bet = bet;
    cout << "Enter stack size: ";
    cin >> stack;
    game.stack = stack;
} // getHandInfo

double computePotOdds(const GameState& game) {
    return (double)game.bet / (game.pot + game.bet);
} // computePotOdds

double estimate_equity(const GameState& game, int simulations) {    // uses Monte Carlo simulation (equity = wins / simulations)
    int wins = 0;
    int ties = 0;
    for (int i = 0; i < simulations; i++) {
        // create and initialize deck
        Deck deck;
        initializeDeck(deck);

        // remove hole and board cards from deck
        for (Card c : game.hole_cards) {
            removeKnown(deck, c);
        }
        for (Card c : game.board) {
            removeKnown(deck, c);
        }

        // shuffle deck and deal opponent hand
        shuffleDeck(deck);
        vector<Card> opponentHand;
        opponentHand.push_back(dealCard(deck));
        opponentHand.push_back(dealCard(deck));

        // complete board if neeeded
        vector<Card> completeBoard = game.board;
        while (completeBoard.size() < 5) {
            completeBoard.push_back(dealCard(deck));
        }

        // build full 7-card hands and evaluate
        vector<Card> heroCards = game.hole_cards;
        heroCards.insert(heroCards.end(), completeBoard.begin(), completeBoard.end());
        vector<Card> villainCards = opponentHand;
        villainCards.insert(villainCards.end(), completeBoard.begin(), completeBoard.end());
        // need to code evaluateHand function and use here
    }

    return wins + ties; // placeholder
}

// Main Functionality

int main() {
    GameState game;
    getHandInfo(game);
    return 1;
}