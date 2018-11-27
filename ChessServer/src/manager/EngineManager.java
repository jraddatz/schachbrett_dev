package manager;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import Constants.EngineConst;

public class EngineManager {

	private final String enginePath;
	
	private String moveHistory = "";

	private ProcessBuilder uciEngineProcessBuilder;
	private Process uciEngineProcess;

	private InputStream engineIn;
	private OutputStream engineOut;

	public EngineManager(String path) {
		this.enginePath = path;
		loadEngine();
		setupEngine();
	}

	public void clearMoveHistory() {
		moveHistory = "";
	}
	
	private void loadEngine() {
		uciEngineProcessBuilder = new ProcessBuilder(enginePath);

		try {
			uciEngineProcess = uciEngineProcessBuilder.start();
		} catch (IOException ex) {
			System.out.println("error loading engine");
		}

		engineIn = uciEngineProcess.getInputStream();
		engineOut = uciEngineProcess.getOutputStream();
	}

	private void setupEngine() {
		messageToEngine(EngineConst.UCI_COMMAND);
		messageToEngine(EngineConst.REDAY_COMMAND);
		messageWithoutAnswer(EngineConst.NEW_GAME_COMMAND);
	}
	
	public String readOutputStream() {
		String buffer = "";
		boolean reading = true;
		while (reading) {
			try {
				char chunk = (char) engineIn.read();
				buffer += chunk;
				if (engineIn.available() == 0) {
					reading = false;
				}
			} catch (IOException ex) {
				System.out.println("engine read IO exception");
				reading = false;

			}
		}
		return buffer;
	}

	public void messageWithoutAnswer(String message) {
		try {
			engineOut.write(message.getBytes());
			engineOut.flush();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	public String messageToEngine(String message) {
		messageWithoutAnswer(message);
		return readOutputStream();
	}
	
	public String getTurn(String userTurn) {
		String engineTurn = "";
		String engineOutput = "";
		moveHistory += userTurn + " ";
		String [] outputLines;
		String [] bestmoveLine;
		boolean bestmoveFound = false;
		messageWithoutAnswer(EngineConst.POSITION_COMMAND + moveHistory +"\n");
		engineOutput = messageToEngine(EngineConst.GO_DEPTH_COMMAND + EngineConst.DEPTH + EngineConst.COMMAND_END);
		while(!bestmoveFound) {
			if(engineOutput.contains("bestmove")) {
				bestmoveFound = true;
			}else {
				engineOutput = readOutputStream();
			}
		}
		outputLines = engineOutput.split("\n");
		bestmoveLine = outputLines[outputLines.length - 1].split(" ");
		engineTurn = bestmoveLine[EngineConst.MOVEPOS];
		moveHistory += engineTurn + " ";
		return engineTurn;
	}
}
