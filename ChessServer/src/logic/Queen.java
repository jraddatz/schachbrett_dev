package logic;

import Constants.LogicConst;

public class Queen extends Figure{

	public Queen(boolean white) {
		super(white);
	}

	@Override
	public byte legalMove(Move move, Field[][] field) {
		if(validBishopMove(move, field) == LogicConst.OK || validRookMove(move, field) == LogicConst.OK) {
			return LogicConst.OK;
		}
		return LogicConst.ILLEGAL;
	}

	public byte validBishopMove(Move move, Field[][] field) {
		int rowDiff = Math.abs(move.getStartRow() - move.getEndRow());
		int columnDiff = Math.abs(move.getStartColumn() - move.getEndColumn());
		if(rowDiff != columnDiff) {
			return LogicConst.ILLEGAL;
		}
		int rowStep = 1;
		if(move.getStartRow() > move.getEndRow()) {
			rowStep = -1;
		}
		int columnStep = 1;
		if(move.getStartColumn() > move.getEndColumn()) {
			columnStep = -1;
		}
		int row = move.getStartRow() + rowStep;
		int column = move.getStartColumn() + columnStep;
		while(row != move.getEndRow()) {
			if(field[row][column].getFigure() != null) {
				return LogicConst.ILLEGAL;
			}
			row += rowStep;
			column += columnStep;
		}
		if(!checkEndField(move.getEndRow(), move.getEndColumn(), field)) {
			return LogicConst.ILLEGAL;
		}
		return LogicConst.OK;
	}
	
	public byte validRookMove(Move move, Field[][] field) {
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
		return "Q";
	}
}
