#include "board.h"

Board::Board(String boardID) {
    _boardID = boardID;  // board identifier
    _gameID = "";
    _opponentID = "";
    _lastMove = "";
    _state = 0;
    _gameType = 0;
    _color = false;
    _turn = false;

    _lcd = Serial_LCD_SparkFun();
}

void Board::set(const String& gameID, const String& opponentID, int state, bool color, bool turn) {
    _gameID = String(gameID);
    _opponentID = String(opponentID);
    _state = state;
    _color = color;
    _turn = turn;
}

void Board::reset() {
    // Reset all members to idle state
    _gameID = "";
    _opponentID = "";
    _lastMove = "";
    _gameType = 0;
    _state = 0;
    _color = false;
    _turn = false;

    _before = LongInt(0, 0);
    _after = LongInt(0, 0);
}

String Board::getBoardID() { return _boardID; }
void Board::setBoardID(const String& boardID) { _boardID = String(boardID); }

int Board::getState() { return _state; }
void Board::setState(int state) { _state = state; }

int Board::getGameType() { return _gameType; }
void Board::setGameType(int gameType) { _gameType = gameType; }

bool Board::isTurn() { return _turn; }
void Board::setTurn(bool turn) { _turn = turn; }

void Board::clearLCD() { _lcd.clear(); }

void Board::requestGame(int gameType) {
  // board is not idle
  if (_state != 0) {
    return;
  }

  // Request a game
  Particle.publish("create_game", "{ \"board_id\": \"" + _boardID + "\", \"type\": \"" + String(gameType) + "\"}", PRIVATE);
  _state = 1;

  // Wait 10 seconds
  delay(10000);
}

void Board::readReedSwitches() {}

String Board::getUCIMove() {
    String move = "";
    LongInt diff = _before ^ _after;

    // Get indices of set bits
    int* indices = diff.locateSetBits();
    int count = indices[64];

    // Convert to UCI based on type of move
    switch(count) {
        case 1: // capturing pieces
            break;

        case 2: // moving pieces
        {
            int source = -1, dest = -1;

            if (_before.isSet(indices[0])) {
                // Moved from index 0 to 1
                source = 0;
                dest = 1;
            } else {
                // Moved from index 1 to 0
                source = 1;
                dest = 0;
            }

            move = toSquare(indices[source]) + toSquare(indices[dest]);
            break;
        }

        case 3: // capturing en passant?
            break;

        case 4: // castling
            break;

        default: // WTF?
            return "????";
            break;
    }

    return move;
}

String Board::toSquare(int index) {
    if (index >= 64 ) {
        return "??";
    }

    int row = index / 8;
    int col = index % 8;

    return String("a" + col) + String("0" + row);
}
