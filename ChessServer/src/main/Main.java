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
		tmp = board.playerTurn(new Move(0,1,2,2));
		System.out.println(tmp[0]);
		System.out.println(board);
		tmp = board.playerTurn(new Move(7,1,5,2));
		System.out.println(tmp[0]);
		System.out.println(board);
		tmp = board.playerTurn(new Move(1,3,2,3));
		System.out.println(tmp[0]);
		System.out.println(board);
		tmp = board.playerTurn(new Move(6,3,4,3));
		System.out.println(tmp[0]);
		System.out.println(board);
		tmp = board.playerTurn(new Move(3,4,4,3));
		System.out.println(tmp[0]);
		System.out.println(board);
		tmp = board.playerTurn(new Move(7,2,6,3));
		System.out.println(tmp[0]);
		System.out.println(board);
		tmp = board.playerTurn(new Move(0,2,2,4));
		System.out.println(tmp[0]);
		System.out.println(board);
		tmp = board.playerTurn(new Move(6,0,5,0));
		System.out.println(tmp[0]);
		System.out.println(board);
		tmp = board.playerTurn(new Move(0,3,1,3));
		System.out.println(tmp[0]);
		System.out.println(board);
		tmp = board.playerTurn(new Move(5,0,4,0));
		System.out.println(tmp[0]);
		System.out.println(board);
		tmp = board.playerTurn(new Move(0,4,0,2));
		System.out.println(tmp[0]);
		System.out.println(board);
	}
}
