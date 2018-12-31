package manager;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

import Constants.ServerConst;
import decoder.Decoder;
import logic.Board;
import logic.Move;
import Constants.EngineConst;
import Constants.LogicConst;

import java.io.InputStream;
import java.io.OutputStream;

public class SocketManager {

	private Socket clientSocket;
	private OutputStream socketOut;
	private ServerSocket serverSocket;

	private boolean gameStarted = false;
	private boolean versusEngine = false;
	private boolean awaitingAck = false;

	private EngineManager stockfish;
	private FileManager fileManager;
	private Board board;

	public SocketManager() {
		stockfish = new EngineManager(EngineConst.PATH);
		fileManager = new FileManager();
	}


	private void messageToClient(byte[] message) {
		try {
			socketOut.write(message);
			socketOut.flush();
		} catch (IOException e) {
			System.err.println("Couldn´t send Message");
		}
	}

	private void processStartCommand(byte input) {
		if(input == ServerConst.PLAYER) {
			board = new Board();
			gameStarted = true;
			versusEngine = false;
			messageToClient(ServerConst.Ok_RESPONSE);
		} else if (input == ServerConst.ENGINE) {
			board = new Board();
			gameStarted = true;
			versusEngine = true;
			stockfish.clearMoveHistory();
			fileManager.createNewHistory();
			messageToClient(ServerConst.Ok_RESPONSE);
		}
		messageToClient(ServerConst.ERROR_RESPONSE);
	}
	
	private void reset() {
		if(gameStarted) {
			board = new Board();
			stockfish.clearMoveHistory();
			fileManager.createNewHistory();
			messageToClient(ServerConst.Ok_RESPONSE);
		}
		messageToClient(ServerConst.ERROR_RESPONSE);
	}
	
	private void processTurn(byte[] input) {
		if(!gameStarted || awaitingAck) {
			messageToClient(ServerConst.ERROR_RESPONSE);
			return;
		}
		Move playerMove = Decoder.playerTurnToMove(input);
		byte []boardRetPlayer = board.playerTurn(playerMove);
		if(boardRetPlayer[0] != LogicConst.ILLEGAL && boardRetPlayer[0] != LogicConst.PROMOTION) {
			fileManager.appendTurnToHistory(playerMove.toString());
		}
		if(boardRetPlayer[0] != LogicConst.ILLEGAL && boardRetPlayer[0] != LogicConst.PROMOTION && versusEngine) {
			String engineTurn = stockfish.getTurn(playerMove.toString());
			Move engineMove = Decoder.engineTurnToMove(engineTurn);
			byte[]boardRetEngine = board.engineTurn(engineMove);
			fileManager.appendTurnToHistory(engineTurn);
			messageToClient(boardRetPlayer);
			messageToClient(boardRetEngine);
			return;
		}
		messageToClient(boardRetPlayer);
	}
	
	private void processPromotionAck(byte figure) {
		if(!awaitingAck) {
			messageToClient(ServerConst.ERROR_RESPONSE);
			return;
		}
		awaitingAck = false;
		byte[] boardRet = board.setPromotedFigure(figure);
		fileManager.appendTurnToHistory(board.getLastMove().toString());
		if(versusEngine) {
			String engineTurn = stockfish.getTurn(board.getLastMove().toString());
			Move engineMove = Decoder.engineTurnToMove(engineTurn);
			byte[]boardRetEngine = board.engineTurn(engineMove);
			fileManager.appendTurnToHistory(engineTurn);
			messageToClient(boardRet);
			messageToClient(boardRetEngine);
			return;
		}
		messageToClient(boardRet);
	}
	
	public void processMessage(byte[] input) {
		String engineTurn;
		
		switch(input[0]) {
		case ServerConst.START:
			processStartCommand(input[1]);
			break;
		case ServerConst.RESET:
			reset();
			break;
		case ServerConst.NEW_TURN:
			processTurn(input);
			break;
		case ServerConst.PROMOTION_ACK:
			processPromotionAck(input[1]);
			break;
		default:
			messageToClient(ServerConst.ERROR_RESPONSE);
			break;
		}
		
	}

	public void manageConnection() throws IOException {
		// create socket
		serverSocket = new ServerSocket(ServerConst.PORT);
		System.err.println("Started server on port " + ServerConst.PORT);

		while (true) {
			clientSocket = serverSocket.accept();
			System.err.println("Accepted connection from client");

			socketOut = clientSocket.getOutputStream();
			InputStream socketIn = clientSocket.getInputStream();
			byte[] buffer = new byte[ServerConst.MAX_INPUT_LENGTH];
			socketIn.read(buffer);
			processMessage(buffer);

			System.err.println("Closing connection with client");
			socketOut.close();
			clientSocket.close();
		}

	}

	public static void main(String[] args) {
		SocketManager mySocketManager = new SocketManager();
		try {
			mySocketManager.manageConnection();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}
