package logic;

import Constants.LogicConst;

public class King extends Figure{

	public King(boolean white) {
		super(white);
	}

	@Override
	public byte legalMove(Move move, Field[][] field) {
		int rowDiff = Math.abs(move.getStartRow() - move.getEndRow());
		int columnDiff = Math.abs(move.getStartColumn() - move.getEndColumn());
		if(rowDiff <= 1 && columnDiff <= 1) {
			if(checkEndField(move.getEndRow(), move.getEndColumn(), field)) {
				return LogicConst.OK;
			}
			return LogicConst.ILLEGAL;
		}
		if(this.isMoved()) {
			return LogicConst.ILLEGAL;
		}
		
		if(move.getEndRow() == LogicConst.LOWEST_ROW && move.getEndColumn() == 6) {
			if(field[0][7].getFigure() != null && field[0][7].getFigure() instanceof Rook && !field[0][7].getFigure().isMoved() 
					&& field[0][5].getFigure() == null && field[0][6].getFigure() == null
					&& white) {
				return LogicConst.WHITE_SHORT_CASTLE;
			}
		}else if(move.getEndRow() == LogicConst.LOWEST_ROW && move.getEndColumn() == 2) {
			if(field[0][0].getFigure() != null && field[0][0].getFigure() instanceof Rook && !field[0][0].getFigure().isMoved() 
					&& field[0][1].getFigure() == null && field[0][2].getFigure() == null
					&& field[0][3].getFigure() == null
					&& white) {
				return LogicConst.WHITE_LONG_CASTLE;
			}
		} else if(move.getEndRow() == LogicConst.ROWS - 1 && move.getEndColumn() == 6) {
			if(field[7][7].getFigure() != null && field[7][7].getFigure() instanceof Rook && !field[7][7].getFigure().isMoved() 
					&& field[7][5].getFigure() == null && field[7][6].getFigure() == null
					&& !white) {
				return LogicConst.BLACK_SHORT_CASTLE;
			}
		}else if(move.getEndRow() == LogicConst.ROWS - 1 && move.getEndColumn() == 2) {
			if(field[7][0].getFigure() != null && field[7][0].getFigure() instanceof Rook && !field[7][0].getFigure().isMoved() 
					&& field[7][1].getFigure() == null && field[7][2].getFigure() == null
					&& field[7][3].getFigure() == null
					&& !white) {
				return LogicConst.BLACK_LONG_CASTLE;
			}
		} 
		
		return LogicConst.ILLEGAL;
	}

	@Override
	public String toString() {
		return "K";
	}
}
