// -----------------------------------------
// Remote Chess Board
/* -----------------------------------------

Some comment here about the project

------------------------------------------*/

#include "globals.h"
//#include "ui.h"
//#include "sensors.h"

void setup() {
    // Setup pins
    setupButtonInterrupts();
    setupSensors();
    setupMotion();

    board.clearLCD();

    // Setup cloud functions
    Particle.function("startGame", startGame);
    Particle.function("movePiece", moveOpponentPiece);
    Particle.function("error", handleError);
    Particle.function("gameOver", gameOver);
    Particle.function("squareX", squareX);
    Particle.function("squareY", squareY);
    Particle.function("stepX", stepX);
    Particle.function("stepY", stepY);
    Particle.function("move", move);
    //Particle.function("movePiece", movePiece);
    Particle.function("magOn", magOn);
    Particle.function("magOff", magOff);
    Particle.function("magInv", magInv);

    // Listen to webhook response
    //Particle.subscribe("hook-response/make_move", myMoveHandler , MY_DEVICES);
}

void loop() {
    // Clear LCD
    //board.clearLCD();

    switch(board.state()) {
      case START:
        if (board.m_first) {
          board.clearLCD();
          board.print("Press a button\nto start a game\n");
          board.m_first = false;
        }
        break;

      case WAIT_FOR_GAME:
        if (board.m_first) {
          board.requestGame(0);
          board.clearLCD();
          board.print("Waiting for a\ngame\n");
          board.m_first = false;
        }
        break;

      case WAIT_FOR_MOVE:
        if (board.m_first) {
          board.clearLCD();
          if (board.hasCaptured()) {
            board.print("Capturing\n");
          } else {
            board.print("UP = move\nRIGHT = capture\n");
          }
          board.m_first = false;
        }
        break;

      case READ_MOVE:
        board.sendMove();

        board.changeState(WAIT_FOR_SERVER);
        break;

      case READ_CAPTURE:
        if (!board.hasCaptured()) {
          board.readCapture();
        }

        board.changeState(WAIT_FOR_MOVE);
        break;

      case WAIT_FOR_SERVER:
        if (board.m_first) {
          board.clearLCD();
          board.print("Waiting for\nserver response\n");
          board.m_first = false;
        }
        break;

      case MOVE_OPP_PIECE:
        if (board.m_first) {
          board.clearLCD();
          board.print(String("Opponent's move\n" + board.getLastOppMove() + "\n"));
          board.m_first = false;

          movePiece(board.getLastOppMove());
        }

        delay(5000);

        if (board.isTurn()) {
          board.changeState(WAIT_FOR_MOVE);
        } else {
          board.changeState(WAIT_FOR_SERVER);
        }
        break;

      case MOVE_ERROR:
        if (board.m_first) {
          board.clearLCD();

          switch(board.getErrorCode()) {
            case 1:
              // other player's turn
              board.print("Other player's\nturn\n");
              break;

            case 2:
              board.print("Invalid move\nTry again\n");
              break;

            case 3:
              board.print("Illegal move\nTry again\n");
              break;

            default:
              break;
          }
          board.m_first = false;
        }

        delay(5000);

        board.changeState(WAIT_FOR_MOVE);

        break;

      case GAME_OVER:
        if (board.m_first) {
          board.clearLCD();

          switch(board.getGameOver()) {
            case 1:
              board.print("You win! Press\nUP to restart\n");
              break;

            case 2:
              board.print("You lose. Press\nUP to restart\n");
              break;

            default:
              break;
          }
          board.m_first = false;
        }
        break;

      case DEBUG_SENSORS:
      {
        startRead();
        unsigned int data = read4Lines();
        unsigned int data2 = read4Lines();

        board.clearLCD();
        String top = String(data, HEX);       // white half of the board
        while (top.length() < 8) {
          top = String("0" + top);
        }

        String bottom = String(data2, HEX);   // black half of the board
        while (bottom.length() < 8) {
          bottom = String("0" + bottom);
        }
        board.print(String(top + "\n" + bottom));

        break;
      }
      default:
        break;
    }

    delay(200);
}

// Start game function, called when game is ready
int startGame(String command) {
    // Convert command to char array
    // format: game_id ~ opponent_id ~ turn ~ color ~
    char strBuffer[125] = "";
    command.toCharArray(strBuffer, 125);

    // Set variables accordingly
    String gameID = strtok(strBuffer, "\"~");
    String opponentID = strtok(NULL, "~");
    bool turn = atoi(strtok(NULL, "~"));
    bool color = atoi(strtok(NULL, "~"));

    board.setGameID(gameID);

    // wait for player's move
    board.changeState(WAIT_FOR_MOVE);

    return 0;
}

// Move opponent's piece function, called when opponent moves
int moveOpponentPiece(String command) {
    // Convert command to char array
    // format: move ~ state ~ turn ~
    char strBuffer[125] = "";
    command.toCharArray(strBuffer, 125);

    // Set variables accordingly
    String move = strtok(strBuffer, "\"~");
    int state = atoi(strtok(NULL, "~"));
    bool turn = atoi(strtok(NULL, "~"));

    // save variables
    board.setTurn(turn);
    board.setLastOppMove(move);

    if (state == 3) {
      board.setGameOver(2);
    }

    board.changeState(MOVE_OPP_PIECE);

    // Move opponent's piece
    // TODO: issue command to motion module

    return 0;
}

int handleError(String command) {
  // save the error code
  board.setErrorCode(atoi(command));

  // switch to error state
  board.changeState(MOVE_ERROR);
}

int gameOver(String command) {
    board.setGameOver(1);
    board.changeState(GAME_OVER);
    return 0;
}

/*
// Handler for response from sending this board's move
void myMoveHandler(const char *event, const char *data) {
    // Convert data to char array
    // format: error ~ status ~ result ~ turn ~
    String str = String(data);
    char strBuffer[125] = "";
    str.toCharArray(strBuffer, 125);

    // Set variables accordingly
    String error = strtok(strBuffer, "\"~");

    // Check for errors
    if (strcmp(error, "") != 0) {
        // Notify player of error
        // TODO: print to LCD

        // Move player's piece back
        // TODO: issue command to motion module
        return;
    }

    int state = atoi(strtok(NULL, "~"));
    String result = strtok(NULL, "~");
    bool turn = atoi(strtok(NULL, "~"));

    if (gameType == 0) {
        if (strcmp(move, "") != 0) {
            // Move AI piece
            // TODO: issue command to motion module

            turn = true;
            return;
        }

        // Move was empty. Player wins. Notify player
        // TODO: print to LCD
        resetBoard();
        return;
    }

    // Check game status
    if (state == 3) {
        // Game over. Notify player and prompt for new game
        // TODO: print to LCD

        board.reset();
    }
}
*/
