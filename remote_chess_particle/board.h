#ifndef BOARD_H
#define BOARD_H

#include "application.h"
//#include "long_int.h"
#include "Serial_LCD_SparkFun.h"
#include "sensors.h"
#include "ui.h"
#include "motor.h"

enum State {
  START,
  WAIT_FOR_GAME,
  WAIT_FOR_MOVE,
  READ_CAPTURE,
  READ_MOVE,
  WAIT_FOR_SERVER,
  MOVE_OPP_PIECE,
  GAME_OVER,
  MOVE_ERROR,
  DEBUG_SENSORS
};

class Board {
  private:
    String _boardID;        // board identifier
    String _gameID;         // game identifier, set when game is started
    String _opponentID;     // opponent identifier, set when game is started
    String _lastMove;       // player's last move used to undo errors
    String _lastOppMove;    // opponent's last move
    State _state;
    int _gameState;            // 0 for idle, 1 for requesting game, 2 for game in progress, 3 for game over
    int _gameType;          // 0 for AI, 1 for human
    bool _color;            // 0 for white, 1 for black
    bool _turn;             // 1 if currently this board's turn to move, 0 otherwise

    int _error;
    int _gameOver;

    String _move;
    String _capture;

    Serial_LCD_SparkFun _lcd;

  public:
    bool m_first;

    Board(const String& boardID);

    void reset();

    String getBoardID();
    void setBoardID(const String& boardID);

    String getGameID();
    void setGameID(const String& gameID);

    State state();
    void changeState(State state);

    int getGameState();
    void setGameState(int gameState);

    int getGameType();
    void setGameType(int gameType);

    bool isTurn();
    void setTurn(bool turn);

    String getLastOppMove();
    void setLastOppMove(const String& move);

    int getErrorCode();
    void setErrorCode(int error);

    int getGameOver();
    void setGameOver(int gameOver);

    bool hasCaptured();

    void clearLCD();

    void requestGame(int gameType);

    String readConfiguration();

    void readCapture();

    void sendMove();

    void print(String msg);

    //String getUCIMove();
    //String toSquare(int index);
};

#endif
