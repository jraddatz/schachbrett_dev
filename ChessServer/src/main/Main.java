package main;

import logic.Board;
import logic.Move;

public class Main {
	public static void main(String [] args) {
		Board board = new Board();
		byte [] tmp = new byte[5];
		tmp = board.playerTurn(new Move(1,4,3,4));
		System.out.println(tmp[0]);
		System.out.println(board);
		tmp = board.playerTurn(new Move(6,4,4,4));
		System.out.println(tmp[0]);
		System.out.println(board);
		tmp = board.playerTurn(new Move(0,5,3,2));
		System.out.println(tmp[0]);
		tmp = board.playerTurn(new Move(7,5,4,2));
		System.out.println(tmp[0]);
		tmp = board.playerTurn(new Move(0,3,2,5));
		System.out.println(tmp[0]);
		tmp = board.playerTurn(new Move(7,1,5,2));
		System.out.println(tmp[0]);
		tmp = board.playerTurn(new Move(2,5,6,5));
		System.out.println(tmp[0]);
		System.out.println(board);
	}
}
