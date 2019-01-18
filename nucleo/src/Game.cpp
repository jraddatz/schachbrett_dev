#include <Game.h>

Game::Game() : buttonQueen(constants::PIN_BUTTON_QUEEN, PullUp),
               buttonKnight(constants::PIN_BUTTON_KNIGHT, PullUp),
               buttonBishop(constants::PIN_BUTTON_BISHOP, PullUp),
               buttonRook(constants::PIN_BUTTON_ROOK, PullUp),
               buttonAI(constants::PIN_BUTTON_AI, PullUp),
               buttonPVP(constants::PIN_BUTTON_PVP, PullUp),
               buttonStart(constants::PIN_BUTTON_START),
               lcd(constants::PIN_LCD_RESET, constants::PIN_LCD_ENABLE, constants::PIN_LCD_DATA_4,
                   constants::PIN_LCD_DATA_5, constants::PIN_LCD_DATA_6, constants::PIN_LCD_DATA_7),
               status(constants::NEWGAME) {

}

void Game::init() {
    b.init();
//    buttonStart.rise(&Game::startPressed);
    //net.set_network(constants::OWN_ADDRESS, constants::NETMASK, constants::GATEWAY);
    net.connect();
    b.updateAll();
    thread.start(callback(Game::Checker_Thread_Start, this));
}

void Game::step() {
    switch (status) {
        case constants::NEWGAME:
            printf("NEWGAME\n");

            gameEnded = 0;
            player = constants::WHITE;

            lcd.cls();
            lcd.printf("Choose Mode!");

            buttonPressed = false;
            while (!buttonPressed) {
                if (!buttonAI.read()) {
                    gameType = protocol::AI;
                    buttonPressed = true;
                } else if (!buttonPVP.read()) {
                    gameType = protocol::PVP;
                    buttonPressed = true;
                }
            }


            if (sendTelegram(&socket, protocol::START, gameType, 0, 0, 0) == 1) {
                if (socket.recv(rbuffer, sizeof rbuffer) < 0) {
                    status = constants::ERROR;
                } else {
                    if (rbuffer[0] == 0) {
                        status = constants::BOARDSETUP;
                    } else {
                        status = constants::ERROR;
                    }
                }
                socket.close();
            } else {
                status = constants::ERROR;
            }
            break;

        case constants::BOARDSETUP:
            if (checkBoardSetup() == 0) {
                clearMails();
                status = constants::START;
            }

            wait(constants::TIMEOUT_BOARDSETUP);
            break;

        case constants::START:
            printf("Start\n");
            evtCommunication = communication.get();
            if (evtCommunication.status == osEventMail) {
                coords *nextCoord = (coords *) evtCommunication.value.p;
                bufferPlayerMoves[0].x = nextCoord->x;
                bufferPlayerMoves[0].y = nextCoord->y;
                bufferPlayerMoves[0].up = nextCoord->up;
                communication.free(nextCoord);
                if (bufferPlayerMoves[0].up) {
                    status = constants::ONEUP;
                } else {
                    addPendingMove(bufferPlayerMoves[0].x, bufferPlayerMoves[0].y, !bufferPlayerMoves[0].up);
                    lcd.cls();
                    lcd.printf("Bitte Figur anheben.\n");
                    status = constants::WAITINGPLAYER;
                }
            }
            break;

        case constants::ONEUP:
            printf("Oneup\n");
            b.ledToggle(bufferPlayerMoves[0].x, bufferPlayerMoves[0].y);
            evtCommunication = communication.get();
            if (evtCommunication.status == osEventMail) {
                coords *nextCoord = (coords *) evtCommunication.value.p;
                bufferPlayerMoves[1].x = nextCoord->x;
                bufferPlayerMoves[1].y = nextCoord->y;
                bufferPlayerMoves[1].up = nextCoord->up;
                communication.free(nextCoord);
                if (bufferPlayerMoves[1].up) {

                    status = constants::TWOUP;
                } else {
                    if (bufferPlayerMoves[0].x == bufferPlayerMoves[1].x &&
                        bufferPlayerMoves[0].y == bufferPlayerMoves[1].y) {
                        b.ledsOff();
                        status = constants::START;
                    } else {
                        status = constants::SEND;
                    }
                }
            }
            break;

        case constants::TWOUP:
            printf("Twoup\n");
            b.ledToggle(bufferPlayerMoves[1].x, bufferPlayerMoves[1].y);
            evtCommunication = communication.get();
            if (evtCommunication.status == osEventMail) {
                coords *nextCoord = (coords *) evtCommunication.value.p;
                bufferPlayerMoves[2].x = nextCoord->x;
                bufferPlayerMoves[2].y = nextCoord->y;
                bufferPlayerMoves[2].up = nextCoord->up;
                communication.free(nextCoord);
                if (bufferPlayerMoves[2].up) {
                    addPendingMove(bufferPlayerMoves[2].x, bufferPlayerMoves[2].y, !bufferPlayerMoves[2].up);
                    addPendingMove(bufferPlayerMoves[1].x, bufferPlayerMoves[1].y, !bufferPlayerMoves[1].up);
                    addPendingMove(bufferPlayerMoves[0].x, bufferPlayerMoves[0].y, !bufferPlayerMoves[0].up);

                    lcd.cls();
                    lcd.printf("3 Figuren anheben nicht möglich!\n");
                    status = constants::WAITINGPLAYER;
                } else {
                    if (bufferPlayerMoves[1].x == bufferPlayerMoves[2].x &&
                        bufferPlayerMoves[1].y == bufferPlayerMoves[2].y) {
                        status = constants::SEND;
                        b.ledToggle(bufferPlayerMoves[1].x, bufferPlayerMoves[1].y, constants::OFF);
                        wait(constants::TIMEOUT_BLINK);
                    } else {
                        addPendingMove(bufferPlayerMoves[2].x, bufferPlayerMoves[2].y, !bufferPlayerMoves[2].up);
                        addPendingMove(bufferPlayerMoves[1].x, bufferPlayerMoves[1].y, !bufferPlayerMoves[1].up);
                        addPendingMove(bufferPlayerMoves[0].x, bufferPlayerMoves[0].y, !bufferPlayerMoves[0].up);

                        lcd.cls();
                        lcd.printf("Figur muss an gleiche Stelle wie Geschlagene\n");
                        status = constants::WAITINGPLAYER;
                    }
                }
            }
            break;

        case constants::SEND:
            printf("Send\n");

            b.ledToggle(bufferPlayerMoves[1].x, bufferPlayerMoves[1].y);

            if (sendTelegram(&socket, protocol::TURN, bufferPlayerMoves[0].x, bufferPlayerMoves[0].y,
                             bufferPlayerMoves[1].x, bufferPlayerMoves[1].y) == 1) {
                status = constants::WAITINGSERVER;
            } else {
                status = constants::ERROR;
            }
            break;

        case constants::WAITINGSERVER:
            printf("Waitingserver\n");
            b.ledsOff();
            if (socket.recv(rbuffer, sizeof rbuffer) < 0) {
                status = constants::ERROR;
            } else {
                socket.close();
                offset = 0;
                if (!(rbuffer[0] & protocol::ERROR)) {
                    if (!(rbuffer[0] & protocol::ILLEGAL)) {
                        if (rbuffer[0] & protocol::CASTLING) {
                            b.ledToggle(rbuffer[1 + offset], rbuffer[2 + offset]);
                            b.ledToggle(rbuffer[3 + offset], rbuffer[4 + offset]);
                            lcd.cls();
                            lcd.printf("Rochade: Turm setzen\n");

                            printf("Player: Castling\n");


                            addPendingMove(rbuffer[1 + offset], rbuffer[2 + offset], constants::UP);
                            addPendingMove(rbuffer[3 + offset], rbuffer[4 + offset], constants::DOWN);
                            offset += 4;
                        }

                        if (rbuffer[0] & protocol::ENPASSANT) {
                            b.ledToggle(rbuffer[1 + offset], rbuffer[2 + offset]);

                            printf("Player: Enpassant\n");

                            addPendingMove(rbuffer[1 + offset], rbuffer[2 + offset], constants::UP);
                            offset += 2;

                            lcd.cls();
                            lcd.printf("En passante\n");

                        }

                        if (rbuffer[0] & protocol::PROMOTION) {
                            isPromoted = true;

                            b.ledToggle(rbuffer[1 + offset], rbuffer[2 + offset]);
                            while ((buttonQueen.read() && buttonKnight.read() && buttonBishop.read() &&
                                    buttonRook.read())) {
                                wait(0.1);
                            }

                            {
                                uint8_t promotionChar = protocol::QUEEN;

                                if (!buttonQueen.read()) {
                                    promotionChar = protocol::QUEEN;
                                } else if (!buttonKnight.read()) {
                                    promotionChar = protocol::KNIGHT;
                                } else if (!buttonBishop.read()) {
                                    promotionChar = protocol::BISHOP;
                                } else if (!buttonRook.read()) {
                                    promotionChar = protocol::ROOK;
                                }

                                if (sendTelegram(&socket, protocol::PROMOTION, promotionChar, 0, 0, 0) == 1) {
                                    addPendingMove(rbuffer[1 + offset], rbuffer[2 + offset], constants::UP);
                                    addPendingMove(rbuffer[1 + offset], rbuffer[2 + offset], constants::DOWN);

                                    offset += 2;
                                } else {
                                    status = constants::ERROR;
                                }
                            }
                        }

                        if (rbuffer[0] & protocol::CHECK) {
                            b.ledToggle(rbuffer[1 + offset], rbuffer[2 + offset]);

                            printf("Player: Check\n");

                            offset += 2;
                        }

                        if (rbuffer[0] & protocol::CHECKMATE) {
                            b.ledsOff();

                            printf("Player: Checkmate\n");

                            checkmate.x = rbuffer[1 + offset];
                            checkmate.y = rbuffer[2 + offset];

                            gameEnded = 1;
                        }

                        b.ledsOff();
                        if (gameType == protocol::AI) {

                            // ***************************************************
                            // Ab hier AI-Move
                            // ***************************************************

                            offset = protocol::AI_MOVE;

                            if (rbuffer[0 + protocol::AI_MOVE] & protocol::ILLEGAL) {
                                b.ledToggle(rbuffer[1 + offset], rbuffer[2 + offset]);
                                b.ledToggle(rbuffer[3 + offset], rbuffer[4 + offset]);

                                printf("AI OK\n");


                                addPendingMove(rbuffer[1 + offset], rbuffer[2 + offset], constants::UP);
                                if (b.checkField(rbuffer[3 + offset], rbuffer[4 + offset]))
                                    addPendingMove(rbuffer[3 + offset], rbuffer[4 + offset], constants::UP);
                                addPendingMove(rbuffer[3 + offset], rbuffer[4 + offset], constants::DOWN);

                                offset += 2;

                                lcd.cls();
                                lcd.printf("KI: Normaler Move\n");

                                if (rbuffer[0 + protocol::AI_MOVE] & protocol::CASTLING) {
                                    b.ledToggle(rbuffer[3 + offset], rbuffer[4 + offset]);
                                    b.ledToggle(rbuffer[5 + offset], rbuffer[6 + offset]);

                                    printf("AI Castling\n");

                                    addPendingMove(rbuffer[3 + offset], rbuffer[4 + offset], constants::UP);
                                    addPendingMove(rbuffer[5 + offset], rbuffer[6 + offset], constants::DOWN);

                                    offset += 6;

                                    lcd.cls();
                                    lcd.printf("KI: Rochade\n");
                                }

                                if (rbuffer[0 + protocol::AI_MOVE] & protocol::ENPASSANT) {
                                    b.ledToggle(rbuffer[3 + offset], rbuffer[4 + offset]);

                                    printf("AI Enpassant\n");

                                    addPendingMove(rbuffer[3 + offset], rbuffer[4 + offset], constants::UP);

                                    offset += 4;

                                    lcd.cls();
                                    lcd.printf("KI: En passante\n");
                                }

                                if (rbuffer[0 + protocol::AI_MOVE] & protocol::PROMOTION) {
                                    printf("AI Promotion\n");

                                    addPendingMove(rbuffer[1 + offset], rbuffer[2 + offset], constants::UP);
                                    addPendingMove(rbuffer[1 + offset], rbuffer[2 + offset], constants::DOWN);

                                    offset += 6;

                                    lcd.cls();
                                    lcd.printf("KI: Promotion\n");
                                }

                            }

                            if (rbuffer[0 + protocol::AI_MOVE] & protocol::CHECK) {
                                b.ledToggle(rbuffer[3 + offset], rbuffer[4 + offset]);

                                printf("AI Check\n");
                            }

                            if (rbuffer[0 + protocol::AI_MOVE] & protocol::CHECKMATE) {
                                b.ledsOff();

                                printf("AI Checkmate\n");

                                checkmate.x = rbuffer[3 + offset];
                                checkmate.y = rbuffer[4 + offset];

                                gameEnded = 1;
                            }

                        }

                        if (!isPromoted && gameType == protocol::PVP) {
                            player = !player;
                        }

                    } else {
                        printf("Player Illegal\n");

                        b.ledToggle(rbuffer[1 + offset], rbuffer[2 + offset]);
                        b.ledToggle(rbuffer[3 + offset], rbuffer[4 + offset]);

                        addPendingMove(rbuffer[1 + offset], rbuffer[2 + offset], constants::UP);
                        addPendingMove(rbuffer[3 + offset], rbuffer[4 + offset], constants::DOWN);
                        if (bufferPlayerMoves[2].x == bufferPlayerMoves[1].x &&
                            bufferPlayerMoves[1].y == bufferPlayerMoves[2].y &&
                            bufferPlayerMoves[1].up != bufferPlayerMoves[2].up) {
                            addPendingMove(rbuffer[1 + offset], rbuffer[2 + offset], constants::DOWN);
                        }

                        lcd.cls();
                        lcd.printf("Illegaler Move\n");
                    }
                } else {
                    status = constants::ERROR;
                }
            }

            if (status != constants::ENDGAME && status != constants::ERROR) status = constants::WAITINGPLAYER;
            break;

        case constants::WAITINGPLAYER:
            printf("Waitingplayer\n");
            evtPendingMoves = pendingMoves.get(constants::TIMEOUT_GET_MAIL);
            while (evtPendingMoves.status == osEventMail) {
                coords *nextPending = (coords *) evtPendingMoves.value.p;
                b.ledToggle(nextPending->x, nextPending->y, constants::ON);
                bool moveMade = false;
                while (!moveMade) {
                    evtCommunication = communication.get(constants::TIMEOUT_GET_MAIL);
                    if (evtCommunication.status == osEventMail) {
                        coords *nextMade = (coords *) evtCommunication.value.p;
                        if ((nextPending->x == nextMade->x && nextPending->y == nextMade->y &&
                             nextPending->up == nextMade->up)) {
                            b.ledToggle(nextPending->x, nextPending->y, constants::OFF);
                            moveMade = true;
                            wait(constants::TIMEOUT_BLINK);
                        }
                        communication.free(nextMade);
                    }
                }
                pendingMoves.free(nextPending);
                evtPendingMoves = pendingMoves.get(constants::TIMEOUT_GET_MAIL);
            }

            if (gameEnded) {
                status = constants::ENDGAME;
            } else {
                if (isPromoted) {
                    status = constants::WAITINGSERVER;
                    isPromoted = false;
                } else {
                    status = constants::START;
                }
            }
            break;

        case constants::ENDGAME:
            printf("ENDGAME\n");
            b.ledToggle(checkmate.x, checkmate.y, gameEnded);
            gameEnded ^= 1;
            wait(constants::TIMEOUT_BLINK);
            break;

        case constants::ERROR:
            printf("----\nError\n-----");
            break;
    }

}

void Game::startPressed() {

}

uint8_t Game::checkBoardSetup() {
    b.ledsOff();
    uint8_t misplaced = 0;
    for (uint8_t x = 0; x < 8; x++) {
        for (uint8_t y = 0; y < 2; y++) {
            if (!b.checkField(x, y)) {
                b.ledToggle(x, y);
                misplaced++;
            }
        }
        for (uint8_t y = 6; y < 8; y++) {
            if (!b.checkField(x, y)) {
                b.ledToggle(x, y);
                misplaced++;
            }
        }
    }
    return misplaced;
}

void Game::checker_thread() {
    while (true) {
        coords c = b.getChange();
        if (c.x != 255) { 
            coords *cp = communication.alloc();
            if (cp == NULL) {
                // Not Enough Memory Available

            } else {
                cp->x = c.x;
                cp->y = c.y;
                cp->up = c.up;
                communication.put(cp);
            }
        }
        wait(constants::TIMEOUT_WHILE_LOOP);
    }
}

void Game::Checker_Thread_Start(void const *p) {
    Game *instance = (Game *) p;
    instance->checker_thread();
}

bool Game::addPendingMove(uint8_t x, uint8_t y, bool up) {
    coords *pointer = pendingMoves.alloc();
    if (pointer == NULL) {
        return false;
    } else {
        pointer->x = x;
        pointer->y = y;
        pointer->up = up;
        pendingMoves.put(pointer);
    }
    return true;
}

void Game::clearMails() {
    osEvent evtCommunication = communication.get(constants::TIMEOUT_GET_MAIL);
    while (evtCommunication.status == osEventMail) {
        coords *nextCoord = (coords *) evtCommunication.value.p;
        communication.free(nextCoord);
        evtCommunication = communication.get(constants::TIMEOUT_GET_MAIL);
    }

    osEvent evtPendingMoves = pendingMoves.get(constants::TIMEOUT_GET_MAIL);
    while (evtPendingMoves.status == osEventMail) {
        coords *nextCoord = (coords *) evtPendingMoves.value.p;
        pendingMoves.free(nextCoord);
        evtPendingMoves = pendingMoves.get(constants::TIMEOUT_GET_MAIL);
    }
}

uint8_t Game::sendTelegram(TCPSocket *socket, char commandByte, char byte1, char byte2, char byte3, char byte4) {
    if (socket->open(&net) != 0) {
        return -1;
    } else {
        printf("Socket open\n");
        if (socket->connect(constants::SERVER_ADDRESS, constants::SERVER_PORT) != 0) {
            return -1;
        } else {
            printf("Socket connected\n");
            char sendBuffer[5] = {commandByte, byte1, byte2, byte3, byte4};
            if (socket->send(sendBuffer, sizeof sendBuffer) != 0) {
                return 1;
            } else {
                return -1;
            }
        }
    }
}
