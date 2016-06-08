// -----------------------------------------
// Remote Chess Board
/* -----------------------------------------

Some comment here about the project

------------------------------------------*/

#include "Serial_LCD_SparkFun.h"
#include "board.h"
#include "ui.h"
#include "sensors.h"

extern Serial_LCD_SparkFun lcd = Serial_LCD_SparkFun();
Board board;

void setup() {

    // Setup pins
    lcd.clear();
    setupButtonInterrupts();
    setupSensors();

    // Setup cloud functions
    Particle.function("startGame", startGame);
    Particle.function("movePiece", moveOpponentPiece);

    // Listen to webhook response
    Particle.subscribe("hook-response/make_move", myMoveHandler , MY_DEVICES);
}

void loop() {
    // Refresh LCD
    lcd.clear();

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

        // Wait 30 seconds
        delay(30000);
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

    int state = 2;

    board.init(gameID, opponentID, state, turn, color);

    // Notify player of game
    // TODO: print to LCD

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

    board.setState(state);
    board.setTurn(turn);

    // Move opponent's piece
    // TODO: issue command to motion module

    // Check game status
    if (state == 3) {
        // Game over. Notify player and prompt for new game
        // TODO: print to LCD

        board.setState(0);
    }

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
