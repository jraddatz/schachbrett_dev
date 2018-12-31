package logic;

import Constants.LogicConst;

public class Knight extends Figure{

	public Knight(boolean white) {
		super(white);
	}

	@Override
	public byte legalMove(Move move, Field[][] field) {
		int rowDiff = Math.abs(move.getStartRow() - move.getEndRow());
		int columnDiff = Math.abs(move.getStartColumn() - move.getEndColumn());
		if(!((rowDiff == 2 && columnDiff == 1) || rowDiff == 1 && columnDiff == 2)) {
			return LogicConst.ILLEGAL;
		}
		if(!checkEndField(move.getEndRow(), move.getEndColumn(), field)) {
			return LogicConst.ILLEGAL;
		}
		return LogicConst.OK;
	}
	
	@Override
	public String toString() {
		return "N";
	}
}
