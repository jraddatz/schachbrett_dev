package logic;


import Constants.LogicConst;

public class Board {

	private boolean whitesTurn;
	private Position whiteKing;
	private Position blackKing;
	private Move lastMove;

	private Field[][] field;

	public Board() {
		whitesTurn = true;
		field = new Field[LogicConst.ROWS][LogicConst.COLUMS];
		for (int i = 0; i < LogicConst.ROWS; i++) {
			for (int j = 0; j < LogicConst.COLUMS; j++) {
				field[i][j] = new Field();
			}
		}
		for (int i = 0; i < LogicConst.COLUMS; i++) {
			field[LogicConst.WHITE_PAWN_ROW][i].setFigure(new Pawn(true));
			field[LogicConst.BLACK_PAWN_ROW][i].setFigure(new Pawn(false));
		}

		field[0][0].setFigure(new Rook(true));
		field[0][1].setFigure(new Knight(true));
		field[0][2].setFigure(new Bishop(true));
		field[0][3].setFigure(new Queen(true));
		field[0][4].setFigure(new King(true));
		field[0][5].setFigure(new Bishop(true));
		field[0][6].setFigure(new Knight(true));
		field[0][7].setFigure(new Rook(true));
		field[7][0].setFigure(new Rook(false));
		field[7][1].setFigure(new Knight(false));
		field[7][2].setFigure(new Bishop(false));
		field[7][3].setFigure(new Queen(false));
		field[7][4].setFigure(new King(false));
		field[7][5].setFigure(new Bishop(false));
		field[7][6].setFigure(new Knight(false));
		field[7][7].setFigure(new Rook(false));

		whiteKing = new Position(0, 4);
		blackKing = new Position(7, 4);
	}

	public boolean checkGenerallConditions(Move move) {
		if (!(LogicConst.LOWEST_ROW <= move.getStartRow() || move.getStartRow() <= LogicConst.ROWS - 1
				|| LogicConst.LOWEST_ROW <= move.getEndRow() || move.getEndRow() <= LogicConst.ROWS - 1
				|| LogicConst.LOWEST_COLUMN <= move.getStartColumn() || move.getEndColumn() <= LogicConst.COLUMS - 1
				|| LogicConst.LOWEST_COLUMN <= move.getEndColumn() || move.getEndColumn() <= LogicConst.COLUMS - 1)) {
			return false;
		}
		if (field[move.getStartRow()][move.getStartColumn()].getFigure() == null) {
			return false;
		}
		if (field[move.getStartRow()][move.getStartColumn()].getFigure().isWhite() != whitesTurn) {
			return false;
		}
		if (move.getStartRow() == move.getEndRow() && move.getStartColumn() == move.getEndColumn()) {
			return false;
		}
		return true;
	}

	public byte[] playerTurn(Move move) {
		byte[] retVal = new byte[LogicConst.COMMAND_LENGTH];
		if (!checkGenerallConditions(move)) {
			retVal[0] = LogicConst.ILLEGAL;
			return retVal;
		}

		byte kind = field[move.getStartRow()][move.getStartColumn()].getFigure().legalMove(move, field);

		switch (kind) {
		case LogicConst.OK:
			retVal = normalMove(move);
			break;
		case LogicConst.WHITE_SHORT_CASTLE:
			retVal = whiteShortCastle(move);
			break;
		case LogicConst.WHITE_LONG_CASTLE:
			retVal = whiteLongCastle(move);
			break;
		case LogicConst.BLACK_SHORT_CASTLE:
			retVal = blackShortCastle(move);
			break;
		case LogicConst.BLACK_LONG_CASTLE:
			retVal = blackLongCastle(move);
			break;
		case LogicConst.EN_PASSANT:
			retVal = enPassant(move);
			break;
		case LogicConst.PROMOTION:
			retVal = normalMove(move);
			if (retVal[0] != LogicConst.ILLEGAL) {
				retVal[0] = LogicConst.PROMOTION;
				retVal[1] = (byte) move.getEndColumn();
				retVal[2] = (byte) move.getEndRow();
			}
			break;
		case LogicConst.ILLEGAL:
			return illegal(move);
		default:
			break;
		}

		if (retVal[0] != LogicConst.ILLEGAL) {
			lastMove = move;
			whitesTurn = !whitesTurn;
			field[move.getEndRow()][move.getEndColumn()].getFigure().setMoved(true);
		}

		return retVal;
	}

	public byte[] engineTurn(Move move) {
		byte[] retVal = new byte[LogicConst.ENGINE_TURN_LENGTH];

		byte kind = field[move.getStartRow()][move.getStartColumn()].getFigure().legalMove(move, field);
		
		retVal[0] = 0;
		
		retVal[1] = (byte) move.getStartColumn();
		retVal[2] = (byte) move.getStartRow();
		retVal[3] = (byte) move.getEndColumn();
		retVal[4] = (byte) move.getEndRow();
		
		setMove(move);
		Move tmp;
		switch (kind) {
		case LogicConst.WHITE_SHORT_CASTLE:
			tmp = new Move(0,7,0,5);
			setMove(tmp);
			retVal[0] += LogicConst.ENGINE_CASTLING;
			break;
		case LogicConst.WHITE_LONG_CASTLE:
			tmp = new Move(0,0,0,3);
			setMove(tmp);
			retVal[0] += LogicConst.ENGINE_CASTLING;
			break;
		case LogicConst.BLACK_SHORT_CASTLE:
			tmp = new Move(7,7,7,5);
			setMove(tmp);
			retVal[0] += LogicConst.ENGINE_CASTLING;
			break;
		case LogicConst.BLACK_LONG_CASTLE:
			tmp = new Move(7,0,7,3);
			setMove(tmp);
			retVal[0] += LogicConst.ENGINE_CASTLING;
			break;
		case LogicConst.PROMOTION:
			setFigure(move.getEndRow(), move.getEndColumn(), whitesTurn, move.getPromotion());
			retVal[0] += LogicConst.PROMOTION;
			break;
		case LogicConst.EN_PASSANT:
			field[move.getStartRow()][move.getEndColumn()].setFigure(null);
			retVal[0] += LogicConst.ENGINE_EN_PASSANT;
		default:
			break;
		}
		
		int index = 5;
		boolean check = false;
		if(whitesTurn) {
			check = blackKingChecked();
			if(check) {
				retVal[0] += LogicConst.ENGINE_CHECK;
				if(blackKingCheckmated()) {
					retVal[0] += LogicConst.ENGINE_CHECK;
				}
				retVal[index++] = (byte) blackKing.getColumn();
				retVal[index++] = (byte) blackKing.getRow();
			}
		} else {
			check = whiteKingChecked();
			if(check) {
				retVal[0] += LogicConst.ENGINE_CHECK;
				if(whiteKingCheckmated()) {
					retVal[0] += LogicConst.ENGINE_CHECK;
				}
				retVal[index++] = (byte) whiteKing.getColumn();
				retVal[index++] = (byte) whiteKing.getRow();
			}
		}
		switch (kind) {
		case LogicConst.WHITE_SHORT_CASTLE:
			retVal[index++] = 7;
			retVal[index++] = 0;
			retVal[index++] = 5;
			retVal[index++] = 0;
			break;
		case LogicConst.WHITE_LONG_CASTLE:
			retVal[index++] = 0;
			retVal[index++] = 0;
			retVal[index++] = 3;
			retVal[index++] = 0;
			break;
		case LogicConst.BLACK_SHORT_CASTLE:
			retVal[index++] = 7;
			retVal[index++] = 7;
			retVal[index++] = 5;
			retVal[index++] = 7;
			break;
		case LogicConst.BLACK_LONG_CASTLE:
			retVal[index++] = 0;
			retVal[index++] = 7;
			retVal[index++] = 3;
			retVal[index++] = 7;
			break;
		case LogicConst.PROMOTION:
			retVal[index++] = move.getPromotion();
			break;
		case LogicConst.EN_PASSANT:
			retVal[index++] = (byte) move.getEndColumn();
			retVal[index++] = (byte) move.getStartRow();
		default:
			break;
		}
		
		lastMove = move;
		whitesTurn = !whitesTurn;
		field[move.getEndRow()][move.getEndColumn()].getFigure().setMoved(true);

		return retVal;
	}

	public void setFigure(int row, int column, boolean color, byte figureCode) {
		switch (figureCode) {
		case LogicConst.KNIGHT:
			field[lastMove.getEndRow()][lastMove.getEndColumn()].setFigure(new Knight(color));
			break;
		case LogicConst.BISHOP:
			field[lastMove.getEndRow()][lastMove.getEndColumn()].setFigure(new Bishop(color));
			break;
		case LogicConst.ROOK:
			field[lastMove.getEndRow()][lastMove.getEndColumn()].setFigure(new Rook(color));
			break;
		case LogicConst.QUEEN:
			field[lastMove.getEndRow()][lastMove.getEndColumn()].setFigure(new Queen(color));
			break;
		default:
			break;
		}
	}

	public byte[] setPromotedFigure(byte figureCode) {
		byte[] retVal = new byte[LogicConst.COMMAND_LENGTH];
		boolean color = true;
		if (whitesTurn) {
			color = false;
		}
		lastMove.setPromotion(figureCode);
		setFigure(lastMove.getEndRow(), lastMove.getEndColumn(), color, figureCode);
		if (whiteKingChecked()) {
			retVal[1] = (byte) whiteKing.getColumn();
			retVal[2] = (byte) whiteKing.getRow();
			if (whiteKingCheckmated()) {
				retVal[0] = LogicConst.CHECKMATE;
				return retVal;
			}
			retVal[0] = LogicConst.CHECK;
			return retVal;
		}
		if (blackKingChecked()) {
			retVal[1] = (byte) blackKing.getColumn();
			retVal[2] = (byte) blackKing.getRow();
			if (blackKingCheckmated()) {
				retVal[0] = LogicConst.CHECKMATE;
				return retVal;
			}
			retVal[0] = LogicConst.CHECK;
			return retVal;
		}
		retVal[0] = LogicConst.OK;
		return retVal;
	}

	public boolean whiteKingChecked() {
		if (lastMove == null) {
			return false;
		}
		for (int row = 0; row < LogicConst.ROWS; row++) {
			for (int column = 0; column < LogicConst.COLUMS; column++) {
				if (field[row][column].getFigure() != null) {
					Move move = new Move(row, column, whiteKing.getRow(), whiteKing.getColumn());
					if (!field[row][column].getFigure().isWhite()
							&& field[row][column].getFigure().legalMove(move, field) != LogicConst.ILLEGAL) {
						return true;
					}
				}
			}
		}
		return false;
	}

	public boolean blackKingChecked() {
		if (lastMove == null) {
			return false;
		}
		for (int row = 0; row < LogicConst.ROWS; row++) {
			for (int column = 0; column < LogicConst.COLUMS; column++) {
				if (field[row][column].getFigure() != null) {
					Move move = new Move(row, column, blackKing.getRow(), blackKing.getColumn());
					if (field[row][column].getFigure().isWhite()
							&& field[row][column].getFigure().legalMove(move, field) != LogicConst.ILLEGAL) {
						return true;
					}
				}
			}
		}
		return false;
	}

	public boolean blackKingCheckmated() {
		for (int figureRow = 0; figureRow < LogicConst.ROWS; figureRow++) {
			for (int figureColumn = 0; figureColumn < LogicConst.COLUMS; figureColumn++) {
				for (int row = 0; row < LogicConst.ROWS; row++) {
					for (int column = 0; column < LogicConst.COLUMS; column++) {
						if (field[figureRow][figureColumn].getFigure() != null
								&& !field[figureRow][figureColumn].getFigure().isWhite()) {
							Figure figure = field[figureRow][figureColumn].getFigure();
							Move move = new Move(figureRow, figureColumn, row, column);
							if (checkGenerallConditions(move) && figure.legalMove(move, field) != LogicConst.ILLEGAL) {
								setMove(move);
								if (figure instanceof King) {
									blackKing.setPosition(move.getEndRow(), move.getEndColumn());
								}
								if (!blackKingChecked()) {
									undoMove(move);
									if (figure instanceof King) {
										blackKing.setPosition(figureRow, figureColumn);
									}
									return false;
								}
								undoMove(move);
								if (figure instanceof King) {
									blackKing.setPosition(figureRow, figureColumn);
								}
							}
						}
					}
				}
			}
		}
		return true;
	}

	public boolean whiteKingCheckmated() {
		for (int figureRow = 0; figureRow < LogicConst.ROWS; figureRow++) {
			for (int figureColumn = 0; figureColumn < LogicConst.COLUMS; figureColumn++) {
				for (int row = 0; row < LogicConst.ROWS; row++) {
					for (int column = 0; column < LogicConst.COLUMS; column++) {
						if (field[figureRow][figureColumn].getFigure() != null
								&& field[figureRow][figureColumn].getFigure().isWhite()) {
							Figure figure = field[figureRow][figureColumn].getFigure();
							Move move = new Move(figureRow, figureColumn, row, column);
							if (checkGenerallConditions(move) && figure.legalMove(move, field) != LogicConst.ILLEGAL) {
								setMove(move);
								if (figure instanceof King) {
									whiteKing.setPosition(move.getEndRow(), move.getEndColumn());
								}
								if (!whiteKingChecked()) {
									undoMove(move);
									if (figure instanceof King) {
										whiteKing.setPosition(figureRow, figureColumn);
									}
									return false;
								}
								undoMove(move);
								if (figure instanceof King) {
									whiteKing.setPosition(figureRow, figureColumn);
								}
							}
						}
					}
				}
			}
		}
		return true;
	}

	public byte[] normalMove(Move move) {
		byte[] retVal = new byte[LogicConst.COMMAND_LENGTH];
		Figure figure = field[move.getStartRow()][move.getStartColumn()].getFigure();
		setMove(move);
		if (whitesTurn) {
			if (figure instanceof King) {
				whiteKing.setPosition(move.getEndRow(), move.getEndColumn());
			}
			if (whiteKingChecked()) {
				if (figure instanceof King) {
					whiteKing.setPosition(move.getStartRow(), move.getStartColumn());
				}
				undoMove(move);
				return illegal(move);
			}
			if (blackKingChecked()) {
				retVal[1] = (byte) blackKing.getColumn();
				retVal[2] = (byte) blackKing.getRow();
				if (blackKingCheckmated()) {
					retVal[0] = LogicConst.CHECKMATE;
					return retVal;
				}
				retVal[0] = LogicConst.CHECK;
				return retVal;
			}
		} else {
			if (figure instanceof King) {
				blackKing.setPosition(move.getEndRow(), move.getEndColumn());
			}
			if (blackKingChecked()) {
				if (figure instanceof King) {
					blackKing.setPosition(move.getStartRow(), move.getStartColumn());
				}
				undoMove(move);
				return illegal(move);
			}
			if (whiteKingChecked()) {
				retVal[1] = (byte) whiteKing.getColumn();
				retVal[2] = (byte) whiteKing.getRow();
				if (whiteKingCheckmated()) {
					retVal[0] = LogicConst.CHECKMATE;
					return retVal;
				}
				retVal[0] = LogicConst.CHECK;
				return retVal;
			}
		}
		retVal[0] = LogicConst.OK;
		return retVal;
	}

	public byte[] whiteShortCastle(Move move) {
		byte[] retVal = new byte[LogicConst.COMMAND_LENGTH];
		if (whiteKingChecked()) {
			return illegal(move);
		}
		setMove(move);
		whiteKing.setPosition(0, 5);
		if (whiteKingChecked()) {
			undoMove(move);
			whiteKing.setPosition(0, 4);
			return illegal(move);
		}
		whiteKing.setPosition(0, 6);
		if (whiteKingChecked()) {
			undoMove(move);
			whiteKing.setPosition(0, 4);
			return illegal(move);
		}
		
		Move tmp = new Move(0, 7, 0, 5);
		setMove(tmp);
		if (blackKingChecked()) {
			retVal[1] = (byte) blackKing.getColumn();
			retVal[2] = (byte) blackKing.getRow();
			retVal[3] = 7;
			retVal[4] = 0;
			retVal[5] = 5;
			retVal[6] = 0;
			if (blackKingCheckmated()) {
				retVal[0] = LogicConst.CASTLING_AND_CHECKMATE;
				return retVal;
			}
			retVal[0] = LogicConst.CASTLING_AND_CHECK;
			return retVal;
		}
		retVal[0] = LogicConst.CASTLING;
		retVal[1] = 7;
		retVal[2] = 0;
		retVal[3] = 5;
		retVal[4] = 0;
		return retVal;
	}

	public byte[] whiteLongCastle(Move move) {
		byte[] retVal = new byte[LogicConst.COMMAND_LENGTH];
		if (whiteKingChecked()) {
			return illegal(move);
		}
		setMove(move);
		whiteKing.setPosition(0, 3);
		if (whiteKingChecked()) {
			undoMove(move);
			whiteKing.setPosition(0, 4);
			return illegal(move);
		}
		whiteKing.setPosition(0, 2);
		if (whiteKingChecked()) {
			undoMove(move);
			whiteKing.setPosition(0, 4);
			return illegal(move);
		}
		
		Move tmp = new Move(0, 0, 0, 3);
		setMove(tmp);
		if (blackKingChecked()) {
			retVal[1] = (byte) blackKing.getColumn();
			retVal[2] = (byte) blackKing.getRow();
			retVal[3] = 0;
			retVal[4] = 0;
			retVal[5] = 3;
			retVal[6] = 0;
			if (blackKingCheckmated()) {
				retVal[0] = LogicConst.CASTLING_AND_CHECKMATE;
				return retVal;
			}
			retVal[0] = LogicConst.CASTLING_AND_CHECK;
			return retVal;
		}
		retVal[0] = LogicConst.CASTLING;
		retVal[1] = 0;
		retVal[2] = 0;
		retVal[3] = 3;
		retVal[4] = 0;
		return retVal;
	}

	public byte[] blackShortCastle(Move move) {
		byte[] retVal = new byte[LogicConst.COMMAND_LENGTH];
		if (blackKingChecked()) {
			return illegal(move);
		}
		setMove(move);
		blackKing.setPosition(7, 5);
		if (blackKingChecked()) {
			blackKing.setPosition(7, 4);
			undoMove(move);
			return illegal(move);
		}
		blackKing.setPosition(7, 6);
		if (blackKingChecked()) {
			blackKing.setPosition(7, 4);
			undoMove(move);
			return illegal(move);
		}
		Move tmp = new Move(7, 7, 7, 5);
		setMove(tmp);
		if (whiteKingChecked()) {
			retVal[1] = (byte) whiteKing.getColumn();
			retVal[2] = (byte) whiteKing.getRow();
			retVal[3] = 7;
			retVal[4] = 7;
			retVal[5] = 5;
			retVal[6] = 7;
			if (blackKingCheckmated()) {
				retVal[0] = LogicConst.CASTLING_AND_CHECKMATE;
				return retVal;
			}
			retVal[0] = LogicConst.CASTLING_AND_CHECK;
			return retVal;
		}
		retVal[0] = LogicConst.CASTLING;
		retVal[1] = 7;
		retVal[2] = 7;
		retVal[3] = 5;
		retVal[4] = 7;
		return retVal;
	}

	public byte[] blackLongCastle(Move move) {
		byte[] retVal = new byte[LogicConst.COMMAND_LENGTH];
		if (blackKingChecked()) {
			return illegal(move);
		}
		setMove(move);
		blackKing.setPosition(7, 3);
		if (blackKingChecked()) {
			undoMove(move);
			blackKing.setPosition(7, 4);
			return illegal(move);
		}
		blackKing.setPosition(7, 2);
		if (blackKingChecked()) {
			undoMove(move);
			blackKing.setPosition(7, 4);
			return illegal(move);
		}
		Move tmp = new Move(7, 0, 7, 3);
		setMove(tmp);
		if (whiteKingChecked()) {
			retVal[1] = (byte) whiteKing.getColumn();
			retVal[2] = (byte) whiteKing.getRow();
			retVal[3] = 7;
			retVal[4] = 7;
			retVal[5] = 3;
			retVal[6] = 7;
			if (blackKingCheckmated()) {
				retVal[0] = LogicConst.CASTLING_AND_CHECKMATE;
				return retVal;
			}
			retVal[0] = LogicConst.CASTLING_AND_CHECK;
			return retVal;
		}
		retVal[0] = LogicConst.CASTLING;
		retVal[1] = 7;
		retVal[2] = 7;
		retVal[3] = 3;
		retVal[4] = 7;
		return retVal;
	}

	public byte[] enPassant(Move move) {
		byte[] retVal = new byte[LogicConst.COMMAND_LENGTH];
		if (!(lastMove.getStartRow() == LogicConst.WHITE_PAWN_ROW
				|| lastMove.getStartRow() == LogicConst.BLACK_PAWN_ROW) || lastMove.getEndRow() != move.getStartRow()
				|| lastMove.getEndColumn() != lastMove.getEndColumn()
				|| Math.abs(lastMove.getStartRow() - lastMove.getEndRow()) != 2) {
			return illegal(move);
		}
		setMove(move);
		field[move.getStartRow()][move.getEndColumn()].setFigure(null);
		;
		if (whitesTurn) {
			if (whiteKingChecked()) {
				undoMove(move);
				field[move.getStartRow()][move.getEndColumn()].setFigure(new Pawn(false));
				return illegal(move);
			}
			if (blackKingChecked()) {
				retVal[1] = (byte) blackKing.getColumn();
				retVal[2] = (byte) blackKing.getRow();
				retVal[3] = (byte) lastMove.getEndColumn();
				retVal[4] = (byte) lastMove.getStartColumn();
				if (blackKingCheckmated()) {
					retVal[0] = LogicConst.EN_PASSANT_AND_CHECKMATE;
					return retVal;
				}
				retVal[0] = LogicConst.EN_PASSANT_AND_CHECK;
				return retVal;
			}
		} else {
			if (blackKingChecked()) {
				undoMove(move);
				field[move.getStartRow()][move.getEndColumn()].setFigure(new Pawn(true));
				return illegal(move);
			}
			if (whiteKingChecked()) {
				retVal[1] = (byte) whiteKing.getColumn();
				retVal[2] = (byte) whiteKing.getRow();
				retVal[3] = (byte) lastMove.getEndColumn();
				retVal[4] = (byte) lastMove.getStartColumn();
				if (whiteKingCheckmated()) {
					retVal[0] = LogicConst.EN_PASSANT_AND_CHECKMATE;
					return retVal;
				}
				retVal[0] = LogicConst.EN_PASSANT_AND_CHECK;
				return retVal;
			}
		}
		retVal[0] = LogicConst.EN_PASSANT;
		retVal[1] = (byte) lastMove.getEndColumn();
		retVal[2] = (byte) lastMove.getStartColumn();
		return retVal;
	}

	public byte[] illegal(Move move) {
		byte[] retVal = new byte[LogicConst.COMMAND_LENGTH];
		retVal[0] = LogicConst.ILLEGAL;
		retVal[1] = (byte) move.getStartColumn();
		retVal[2] = (byte) move.getStartRow();
		retVal[3] = (byte) move.getEndColumn();
		retVal[4] = (byte) move.getEndRow();
		return retVal;
	}

	public void setMove(Move move) {
		Figure figure = field[move.getStartRow()][move.getStartColumn()].getFigure();
		field[move.getEndRow()][move.getEndColumn()].setFigure(figure);
		field[move.getStartRow()][move.getStartColumn()].setFigure(null);
	}

	public void undoMove(Move move) {

		Figure figure = field[move.getEndRow()][move.getEndColumn()].getFigure();
		field[move.getStartRow()][move.getStartColumn()].setFigure(figure);
		field[move.getEndRow()][move.getEndColumn()].setFigure(null);
	}

	@Override
	public String toString() {
		String retVal = "";
		for (int row = 0; row < LogicConst.ROWS; row++) {
			for (int column = 0; column < LogicConst.ROWS; column++) {
				retVal += field[row][column].toString();
			}
			retVal += "\n";
		}
		return retVal;
	}

	public Move getLastMove() {
		return lastMove;
	}
}
