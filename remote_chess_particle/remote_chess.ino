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

    board.clearLCD();

    // Setup cloud functions
    Particle.function("startGame", startGame);
    Particle.function("movePiece", moveOpponentPiece);
    Particle.function("error", handleError);
    Particle.function("win", win);

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
          board.print("UP = capture\nRIGHT = move\n");
          board.m_first = false;
        }
        break;

      case READ_MOVE:
        board.sendMove();

        board.setState(WAIT_FOR_SERVER);
        break;

      case READ_CAPTURE:
        break;

      case WAIT_FOR_SERVER:
        break;

      case INVALID_MOVE:
        if (board.m_first) {
          board.clearLCD();
          board.print("Invalid move\nTry again\n");
          board.m_first = false;
        }
        break;

      case WAIT_FOR_OPP_MOVE:
        if (board.m_first) {
          board.clearLCD();
          board.print("Waiting for\nopponent's move\n");
          board.m_first = false;
        }
        break;

      default:
        break;
    }

    delay(200);

    /*
    Serial1.print(getMenuString());
    Serial1.print("\n");
    Serial1.print(getLine2());

    delay(100);
    */

    /*
    // Publish create game event if not currently in a game
    if (digitalRead(startButton) == HIGH && board.getState() == 0) {
        // Get and save the game type
        int gameType = 0;
        board.setGameType(gameType);

        // Create a game
        Particle.publish("create_game", "{ \"board_id\": \"" + board.getBoardID() + "\", \"type\": \"" + String(gameType) + "\"}", PRIVATE);
        board.setState(1);

        // Wait 10 seconds
        delay(10000);
    }

    // Read board configuration before move
    if (digitalRead(startMove) == HIGH && board.isTurn()) {
        // Read configuration before the move
        // TODO: read bitstring

        // TODO: Get the move in UCI format somehow?

    }

    if (digitalRead(captureMove) == HIGH && board.isTurn()) {
        // Read configuration before the capture
        // TODO: read bitstring
        int beforeCapture = 0;

        // Read configuration after the capture
        // TODO: read bitstring
        int afterCapture = 0;

        beforeMove = afterCapture;
    }

    // Read board configuration after move
    if (digitalRead(confirmMove) == HIGH && board.isTurn()) {
        // Read configuration after the move
        // TODO: read bitstring
        afterMove = 0;

        String move = "";//toUCI(beforeMove, afterMove);

        // Send move
        Particle.publish("make_move", "{ \"board_id\": \"" + boardID + "\", \"game_id\": \"" + gameID + "\", \"move\": \"" + move + "\"}", PRIVATE);

        // Wait 5 seconds
        delay(5000);
    }
    */
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

    board.set(gameID);

    // wait for player's move
    board.setState(WAIT_FOR_MOVE);

    return 0;
}

// Move opponent's piece function, called when opponent moves
int moveOpponentPiece(String command) {
    // Convert command to char array
    // format: move ~ status ~ turn ~
    char strBuffer[125] = "";
    command.toCharArray(strBuffer, 125);

    // Set variables accordingly
    String move = strtok(strBuffer, "\"~");
    int state = atoi(strtok(NULL, "~"));
    bool turn = atoi(strtok(NULL, "~"));

    // Move opponent's piece
    // TODO: issue command to motion module

    board.clearLCD();
    board.print(move);
    //board.setState(WAIT_FOR_MOVE);

    // Check game status
    if (state == 3) {
        // Game over. Notify player and prompt for new game
        // TODO: print to LCD

        board.setState(START);
    }

    return 0;
}

int handleError(String command) {
  if (command == "0") {
    // other player's turn
    board.clearLCD();
    board.print("WTF");
  } else if (command == "1") {
    // unrecognized move
    board.clearLCD();
    board.print("Invalid move");

  } else if (command == "2") {
    // illegal move
    board.clearLCD();
    board.print("Illegal move");
  }
}

int win(String command) {
    return 0;
}

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

/*
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
*/

    // Check game status
    if (state == 3) {
        // Game over. Notify player and prompt for new game
        // TODO: print to LCD

        board.reset();
    }
}
