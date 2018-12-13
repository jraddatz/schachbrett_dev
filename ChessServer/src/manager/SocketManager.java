package manager;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

import Constants.ServerConst;
import decoder.Decoder;
import Constants.EngineConst;

import java.io.InputStream;
import java.io.OutputStream;
import java.util.NoSuchElementException;
import java.util.Scanner;
import java.io.BufferedInputStream;

public class SocketManager {

	private Scanner inputScanner;
	private Socket clientSocket;
	private OutputStream socketOut;
	private ServerSocket serverSocket;

	private boolean gameStarted = false;

	private EngineManager stockfish;
	private FileManager fileManager;

	public SocketManager() {
		stockfish = new EngineManager(EngineConst.PATH);
		fileManager = new FileManager();
	}

	private void setupScanner(Socket socket) {
		try {
			InputStream is = socket.getInputStream();
			inputScanner = new Scanner(new BufferedInputStream(is), ServerConst.CHARSET_NAME);
		} catch (IOException ioe) {
			throw new IllegalArgumentException("Could not open " + socket, ioe);
		}
	}

	private String readLine() {
		String line;
		try {
			line = inputScanner.nextLine();
		} catch (NoSuchElementException e) {
			line = null;
		}
		return line;
	}

	private void messageToClient(String message) {

		try {
			socketOut.write(message.getBytes());
			socketOut.flush();
		} catch (IOException e) {
			System.err.println("Couldn´t send Message");
		}
	}

	public void processMessage(String message) {
		String engineTurn;
		try {
			message = Decoder.decodeMessage(message);
		} catch (NumberFormatException e) {
			System.err.println("invalid Message");
		}
		if (message.equals(ServerConst.NEW_GAME_MESSAGE)) {
			gameStarted = true;
			fileManager.createNewHistory();
			stockfish.messageWithoutAnswer(EngineConst.NEW_GAME_COMMAND);
			stockfish.clearMoveHistory();
			messageToClient(ServerConst.NEW_GAME_RESPONSE);
		} else if (message.matches(ServerConst.TURN_REGEX) && gameStarted) {
			engineTurn = stockfish.getTurn(message);
			fileManager.appendTurnToHistory(message);
			fileManager.appendTurnToHistory(engineTurn);
			engineTurn = Decoder.encodeTurn(engineTurn);
			messageToClient(engineTurn);
		} else {
			messageToClient(ServerConst.ERROR_RESPONSE);
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
			setupScanner(clientSocket);

			String buffer = "";
			buffer = readLine();
			processMessage(buffer);

			System.err.println("Closing connection with client");
			socketOut.close();
			inputScanner.close();
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
