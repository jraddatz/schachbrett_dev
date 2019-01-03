package logic;

import Constants.LogicConst;

public class Rook extends Figure{

	public Rook(boolean white) {
		super(white);
	}

	@Override
	public byte legalMove(Move move, Field[][] field) {
		int rowDiff = Math.abs(move.getStartRow() - move.getEndRow());
		int columnDiff = Math.abs(move.getStartColumn() - move.getEndColumn());
		if(!((rowDiff == 0 && columnDiff != 0) || (rowDiff != 0 && columnDiff == 0))) {
			return LogicConst.ILLEGAL;
		}
		if(rowDiff == 0) {
			int column = Math.min(move.getStartColumn(), move.getEndColumn()) + 1;
			int maxColumn = Math.max(move.getStartColumn(),move.getEndColumn()) + 1;
			while(column < maxColumn) {
				if(field[move.getStartRow()][column].getFigure() != null) {
					return LogicConst.ILLEGAL;
				}
				column++;
			}
		} else{
			int row = Math.min(move.getStartRow(), move.getEndRow()) + 1;
			int maxRow = Math.max(move.getStartRow(),move.getEndRow()) + 1;
			while(row < maxRow) {
				if(field[row][move.getStartColumn()].getFigure() != null) {
					return LogicConst.ILLEGAL;
				}
				row++;
			}
		}
		if(!checkEndField(move.getEndRow(), move.getEndColumn(), field)) {
			return LogicConst.ILLEGAL;
		}
		return LogicConst.OK;
	}

	
	@Override
	public String toString() {
		return "R";
	}
}
