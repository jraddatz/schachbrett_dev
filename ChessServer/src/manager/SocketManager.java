package manager;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.logging.Level;
import java.util.logging.Logger;

import Constants.EngineConst;

import java.io.InputStream;
import java.io.OutputStream;
import java.util.NoSuchElementException;
import java.util.Scanner;
import java.io.BufferedInputStream;

public class SocketManager {

	private static final int PORT = 3000;
	private static final String CHARSET_NAME = "UTF-8";
	private static final String NEW_GAME_RESPONSE = "New Game started";

	private Scanner inputScanner;
	private Socket clientSocket;
	private OutputStream socketOut;

	private EngineManager stockfish;
	private FileManager fileManager;

	public SocketManager() {
		stockfish = new EngineManager(EngineConst.PATH);
		fileManager = new FileManager();
	}

	private void setupScanner(Socket socket) {
		try {
			InputStream is = socket.getInputStream();
			inputScanner = new Scanner(new BufferedInputStream(is), CHARSET_NAME);
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
		if (message.equals("new")) {
			fileManager.createNewHistory();
			stockfish.messageWithoutAnswer(EngineConst.NEW_GAME_COMMAND);
			stockfish.clearMoveHistory();
			messageToClient(NEW_GAME_RESPONSE);
		} else {
			engineTurn = stockfish.getTurn(message);
			fileManager.appendTurnToHistory(message);
			fileManager.appendTurnToHistory(engineTurn);
			messageToClient(engineTurn);
		}
	}

	public void manageConnection() throws IOException {
		// create socket
		ServerSocket serverSocket = new ServerSocket(PORT);
		System.err.println("Started server on port " + PORT);

		// repeatedly wait for connections, and process
		while (true) {

			// a "blocking" call which waits until a connection is requested
			clientSocket = serverSocket.accept();
			System.err.println("Accepted connection from client");

			socketOut = clientSocket.getOutputStream();
			setupScanner(clientSocket);

			// waits for data and reads it in until connection dies
			// readLine() blocks until the server receives a new line from client
			String buffer = "";
			buffer = readLine();
			processMessage(buffer);
			// close IO streams, then socket
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
