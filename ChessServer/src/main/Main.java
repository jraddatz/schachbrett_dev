package main;

import manager.*;

public class Main {
	public static void main(String [] args) {
		EngineManager stockfish = new EngineManager("/Users/jendrikmuller/Desktop/stockfish-9-mac/Mac/stockfish-9-64");
		System.out.println("uci: " + stockfish.messageToEngine("uci\n"));
		System.out.println("isready: " + stockfish.messageToEngine("isready\n"));
		stockfish.messageWithoutAnswer("ucinewgame\n");
		System.out.println(stockfish.getTurn("e2e4"));
		System.out.println(stockfish.getTurn("d2d4"));
		stockfish.messageToEngine("quit\n");
		
	}
}
