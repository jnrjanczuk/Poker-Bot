#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <random>
#include <unordered_map>
using namespace std;

// ENUM CLASSES

enum class Suit { HEARTS, DIAMONDS, CLUBS, SPADES };

enum class Rank { TWO = 2, THREE, FOUR, FIVE, SIX,
            SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING, ACE };

enum class Street { PREFLOP, FLOP, TURN, RIVER };

// overloaded Street increment operator
Street& operator++(Street& street) {
    street = static_cast<Street>(static_cast<int>(street) + 1);
    return street;
}

enum class HandCategory { HIGH_CARD, PAIR, TWO_PAIR, TRIPS, STRAIGHT, FLUSH, FULL_HOUSE, QUADS, STRAIGHT_FLUSH };

enum class Action { FOLD, CHECK, CALL, BET, RAISE };

// CUSTOM CONTAINERS

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
    int players;
    int bigBlind;

    Street street;
}; // GameState

struct HandRank {
    HandCategory category;
    vector<int> ranks;
}; // HandRank

struct Decision {
    Action action;
    double betSize;
}; // Decision

// HELPER FUNCTIONS

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
} // initializeDeck()

void shuffleDeck(Deck& deck) {
    random_device rd;
    mt19937 gen(rd());
    shuffle(deck.cards.begin(), deck.cards.end(), gen);
} // shuffleDeck()

Card dealCard(Deck& deck) {
    Card card = deck.cards.back();
    deck.cards.pop_back();
    return card;
} // dealCard()

void removeKnown(Deck& deck, const Card& target) {
    for (auto it = deck.cards.begin(); it != deck.cards.end(); it++) {
        if (it->rank == target.rank && it->suit == target.suit) {
            deck.cards.erase(it);
            return;
        }
    }
} // removeKnown()

HandRank evaluateHand(vector<Card> hand) {
    int rankCount[13] = {0};
    int suitCount[4] = {0};

    // count number of ranks and suits
    for (const Card& c : hand) {
        rankCount[static_cast<int>(c.rank) - 2]++;
        suitCount[static_cast<int>(c.suit)]++;
    }
    
    // check if flush exists
    bool flush = false;
    Suit flushSuit;
    vector<Card> flushCards;
    for (int i = 0; i < 4; i++) {
        if (suitCount[i] >= 5) {
            flush = true;
            flushSuit = static_cast<Suit>(i);
            break;
        }
    }
    if (flush) {
        for (const Card& c : hand) {
            if (c.suit == flushSuit) flushCards.push_back(c);
        }
    }

    // check if straight exists
    bool rankExists[13] = {false};
    for (int i = 0; i < 13; i++) {
        if (rankCount[i] > 0) { rankExists[i] = true; }
    }
    bool straight = false;
    int straightHigh = -1;
    int streak = 0;
    for (int i = 12; i >= 0; i--) {
        if (rankExists[i]) {
            streak++;
            if (streak >= 5) {
                straight = true;
                straightHigh = i;
                break;
            }
        }
        else {
            streak = 0;
        }
    }
    // check wheel straight
    if (!straight && rankExists[12] && rankExists[0] && rankExists[1] && rankExists[2] && rankExists[3]) {
        straight = true;
        straightHigh = 3;
    }

    // straight flush
    if (flush) {
        bool flushRankExists[13] = {false};
        for (const Card& c : flushCards) {
            int rankIndex = static_cast<int>(c.rank) - 2;
            flushRankExists[rankIndex] = true;
        }
        streak = 0;
        for (int i = 12; i >=0; i--) {
            if (flushRankExists[i]) {
                streak++;
                if (streak >= 5) {
                    return {HandCategory::STRAIGHT_FLUSH, {i + 4}};
                }
            }
            else {
                streak = 0;
            }
        }
        // check wheel straight flush
        if (!straight && flushRankExists[12] && flushRankExists[0] && flushRankExists[1] && flushRankExists[2] && flushRankExists[3]) {
            return {HandCategory::STRAIGHT_FLUSH, {3}};
        }
    }

    // quads
    for (int i = 12; i >= 0; i--) {
        if (rankCount[i] == 4) {
            vector<int> ranks;
            ranks.push_back(i);
            // check kicker
            for (int j = 12; j >= 0; j--) {
                if (j == i) continue;
                if (rankCount[j] > 0) {
                    ranks.push_back(j);
                    break;
                }
            }
            return {HandCategory::QUADS, ranks};
        }
    }

    // full house
    int tripsRank = -1;
    int pairRank = -1;
    for (int i = 12; i >= 0; i--) {
        if (rankCount[i] == 3) {
            tripsRank = i;
        }
        else if (rankCount[i] == 2) {
            pairRank = i;
        }
        if (tripsRank != -1 && pairRank != -1) {
            return {HandCategory::FULL_HOUSE, {tripsRank, pairRank}};
        }
    }
    if (tripsRank != -1 && pairRank == -1) {
        for (int i = 12; i >= 0; i--) {
            if (i == tripsRank) continue;
            if (rankCount[i] >= 2) { pairRank = i; break; }
        }
    }

    // flush
    if (flush) {
        int flushHigh = -1;
        for (const Card& c : flushCards) {
            int rankIndex = static_cast<int>(c.rank) - 2;
            flushHigh = max(rankIndex, flushHigh);
        }
        return {HandCategory::FLUSH, {flushHigh}};
    }

    // straight
    if (straight) {
        return {HandCategory::STRAIGHT, {straightHigh}};
    }

    // trips
    for (int i = 12; i >= 0; i--) {
        if (rankCount[i] == 3) {
            vector<int> ranks;
            ranks.push_back(i);
            // check kickers
            for (int j = 12; j >= 0; j--) {
                if (j == i) { continue; }
                if (rankCount[j] > 0) {
                    ranks.push_back(j);
                    if (ranks.size() == 3) { break; }
                }
            }
            return {HandCategory::TRIPS, ranks};
        }
    }

    // two pair
    int pair1 = -1;
    int pair2 = -1;
    for (int i = 12; i >= 0; i--) {
        if (rankCount[i] == 2) {
            if (pair1 == -1) {
                pair1 = i;
            }
            else {
                pair2 = i;
                break;
            }
        }
    }
    // check kicker
    if (pair1 != -1 && pair2 != -1) {
        vector<int> ranks;
        ranks.push_back(pair1);
        ranks.push_back(pair2);
        for (int i = 12; i >= 0; i--) {
            if (i == pair1 || i == pair2) { continue; }
            if (rankCount[i] > 0) {
                ranks.push_back(i);
                break;
            }
        }
        return {HandCategory::TWO_PAIR, ranks};
    }

    // pair
    if (pair1 != -1) {
        vector<int> ranks;
        ranks.push_back(pair1);
        // check kicker
        for (int i = 12; i >= 0; i--) {
            if (i == pair1) { continue; }
            if (rankCount[i] > 0) {
                ranks.push_back(i);
                if (ranks.size() == 4) { break; }
            }
        }
        return {HandCategory::PAIR, ranks};
    }

    // high card
    vector<int> highCards;
    for (int i = 12; i >= 0; i--) {
        if (rankCount[i] > 0) {
            highCards.push_back(i);
            if (highCards.size() == 5) { break; }
        }
    }
    return {HandCategory::HIGH_CARD, highCards};
} // evaluateHand()

Card parseCard(string s) {
    Card card;

    // read in rank
    if (s[0] == 'J') { card.rank = Rank::JACK; }
    else if (s[0] == 'Q') { card.rank = Rank::QUEEN; }
    else if (s[0] == 'K') { card.rank = Rank::KING; }
    else if (s[0] == 'A') { card.rank = Rank::ACE; }
    else {
        card.rank = static_cast<Rank>(s[0] - '0');
    }

    // read in suit
    if (s[1] == 'h') { card.suit = Suit::HEARTS; }
    else if (s[1] == 'd') { card.suit = Suit::DIAMONDS; }
    else if (s[1] == 'c') { card.suit = Suit::CLUBS; }
    else { card.suit = Suit::SPADES; }

    return card;
} // parseCard()

void getStreet(GameState& game) {
    char input;
    cout << "Enter street (enter the first letter of preflop/flop/turn/river): ";
    cin >> input;
    if (input == 'p') { game.street = Street::PREFLOP; }
    else if (input == 'f') { game.street = Street::FLOP; }
    else if (input == 't') { game.street = Street::TURN; }
    else { game.street = Street::RIVER; }
} // getStreet()

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
} // getHandInfo()

double computePotOdds(const GameState& game) {
    return (double)game.bet / (game.pot + game.bet);
} // computePotOdds()

double estimateEquity(const GameState& game, int simulations) {    // uses Monte Carlo simulation (equity = wins / simulations)
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

        // shuffle deck and deal opponent(s) hand
        shuffleDeck(deck);
        vector<vector<Card>> opponentHands;
        for (int i = 0; i < game.players - 1; i++) {
            vector<Card> opponentHand;
            opponentHand.push_back(dealCard(deck));
            opponentHand.push_back(dealCard(deck));
            opponentHands.push_back(opponentHand);
        }

        // complete board if neeeded
        vector<Card> completeBoard = game.board;
        while (completeBoard.size() < 5) {
            completeBoard.push_back(dealCard(deck));
        }

        // build full 7-card hands and evaluate
        vector<Card> myHand = game.hole_cards;
        myHand.insert(myHand.end(), completeBoard.begin(), completeBoard.end());
        HandRank villain = {HandCategory::HIGH_CARD, {0}};
        for (int i = 0; i < game.players - 1; i++) {
            opponentHands[i].insert(opponentHands[i].end(), completeBoard.begin(), completeBoard.end());
            HandRank curr = evaluateHand(opponentHands[i]);
            if ((curr.category > villain.category) || (curr.category == villain.category && curr.ranks > villain.ranks)) {
                villain = curr;
            }
        }
        HandRank hero = evaluateHand(myHand);
        if ((hero.category > villain.category) || (hero.category == villain.category && hero.ranks > villain.ranks)) {
            wins++;
        }
        else if (hero.category == villain.category && hero.ranks == villain.ranks) {
            ties++;
        }
    }

    // compute and return equity
    return (wins + ties * 0.5) / simulations;
} // estimateEquity()

Decision recommendAction(const GameState& game) {
    Decision result;
    result.betSize = 0;

    // compute pot odds and equity
    double potOdds = computePotOdds(game);
    double equity = estimateEquity(game, 10000);

    // adjust threshold for number of players
    double threshold = potOdds + 0.02 * (game.players - 2);

    // fold
    if (equity < threshold) {
        result.action = Action::FOLD;
        return result;
    }
    
    // check/call
    if (equity >= threshold && equity < (threshold + 0.1)) {
        if (game.bet == 0) {
            result.action = Action::CHECK;
            return result;
        }
        else {
            result.action = Action::CALL;
            result.betSize = min(game.stack, game.bet);
            return result;
        }
    }
    
    // bet/raise
    if (game.bet == 0) {
        result.action = Action::BET;
    }
    else {
        result.action = Action::RAISE;
    }
    if (equity >= (threshold + 0.1) && equity < (threshold + 0.2)) {
        result.betSize = min(static_cast<double>(game.stack), (game.pot * 0.5));
        return result;
    }
    else if (equity >= (threshold + 0.2) && equity < (threshold + 0.3)) {
        result.betSize = min(static_cast<double>(game.stack), (game.pot * 0.75));
        return result;
    }
    else {
        result.betSize = min(game.stack, game.pot);
    }

    return result;
} // recommendAction()

void outputAction(Decision decision) {
    cout << "Recommended Action: ";
    if (decision.action == Action::FOLD) {
        cout << "Fold\n";
        return;
    }
    else if (decision.action == Action::CHECK) {
        cout << "Check\n";
        return;
    }
    else if (decision.action == Action::CALL) {
        cout << "Call\n";
        return;
    }
    else if (decision.action == Action::BET) {
        cout << "Bet " << decision.betSize << "\n";
        return;
    }
    else {
        cout << "Raise " << decision.betSize << "\n";
        return;
    }
} // outputAction()

// MAIN FUNCTIONALITY

int main() {
    GameState game;
    getHandInfo(game);
    Decision decision = recommendAction(game);
    outputAction(decision);
} // main()