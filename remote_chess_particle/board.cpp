#include "board.h"

Board::Board(const String& boardID) {
    _boardID = String(boardID);  // board identifier
    _gameID = "";
    _opponentID = "";
    _lastMove = "";
    _lastOppMove = "";
    _state = START;
    _gameState = 0;
    _gameType = 0;
    _color = false;
    _turn = false;
    _error = 0;
    _gameOver = 0;

    _capture = "";
    _move = "";

    _lcd = Serial_LCD_SparkFun();

    m_first = true;
}

void Board::reset() {
    // Reset all members to idle state
    _gameID = "";
    _opponentID = "";
    _lastMove = "";
    _lastOppMove = "";
    _state = START;
    _gameState = 0;
    _gameType = 0;
    _color = false;
    _turn = false;

    _error = 0;
    _gameOver = 0;

    m_first = true;
}

String Board::getBoardID() { return _boardID; }
void Board::setBoardID(const String& boardID) { _boardID = String(boardID); }

String Board::getGameID() { return _gameID; }
void Board::setGameID(const String& gameID) { _gameID = String(gameID); }

State Board::state() { return _state; }
void Board::changeState(State state) {
  if (_state != state) {
    m_first = true;
    _state = state;
  }
}

int Board::getGameState() { return _gameState; }
void Board::setGameState(int gameState) { _gameState = gameState; }

int Board::getGameType() { return _gameType; }
void Board::setGameType(int gameType) { _gameType = gameType; }

bool Board::isTurn() { return _turn; }
void Board::setTurn(bool turn) { _turn = turn; }

String Board::getLastOppMove() { return _lastOppMove; }
void Board::setLastOppMove(const String& move) {
  _lastOppMove = String(move);
}

int Board::getErrorCode() { return _error; }
void Board::setErrorCode(int error) { _error = error; }

int Board::getGameOver() { return _gameOver; }
void Board::setGameOver(int gameOver) { _gameOver = gameOver; }

bool Board::hasCaptured() { return _capture != ""; }

void Board::clearLCD() { _lcd.clear(); }

void Board::requestGame(int gameType) {
  // Request a game
  Particle.publish(
    "create_game",
    String("{ \"board_id\": \"" + _boardID + "\", \"type\": \"" + String(gameType) + "\"}"),
    PUBLIC
  );
}

String Board::readConfiguration() {
  startRead();
  unsigned int data = read4Lines();
  unsigned int data2 = read4Lines();

  clearLCD();
  String top = String(data, BIN);       // black half of the board
  while (top.length() < 32) {
    top = String("0" + top);
  }

  String bottom = String(data2, BIN);   // white half of the board
  while (bottom.length() < 32) {
    bottom = String("0" + bottom);
  }

  return String(top + bottom);
}

void Board::readCapture() {
  _capture = readConfiguration();
}

void Board::sendMove() {
  _move = readConfiguration();

  // Send move
  Particle.publish(
    "make_move",
    String("{ \"board_id\": \"" + _boardID + "\", \"game_id\": \"" + _gameID + "\", \"move\": \"" + _move + "\", \"capture\": \"" + _capture + "\"}"),
    PUBLIC
  );

  // reset capture
  _capture = "";
}

void Board::print(String msg) {
  Serial1.print(msg);
}

/*
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
*/
