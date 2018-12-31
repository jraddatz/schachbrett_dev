package logic;

import Constants.LogicConst;

public class Pawn extends Figure{

	public Pawn(boolean white) {
		super(white);
	}

	@Override
	public byte legalMove(Move move, Field[][] field) {
		int stepDir = -1;
		if(white) {
			stepDir = 1;
		}
		int rowStep = move.getEndRow() - move.getStartRow();
		int columnDiff = Math.abs(move.getStartColumn() - move.getEndColumn());
		Field endField = field[move.getEndRow()][move.getEndColumn()];
		Field startField = field[move.getStartRow()][move.getStartColumn()];
		
		if((rowStep == stepDir && endField.getFigure() == null && columnDiff == 0)
				|| (rowStep == stepDir && endField.getFigure() != null && endField.getFigure().isWhite() != white && columnDiff == 1)) {
			if(move.getEndRow() == 0 || move.getEndRow() == 7) {
				return LogicConst.PROMOTION;
			}
			return LogicConst.OK;
		}
		
		if(rowStep == 2 * stepDir 
				&& endField.getFigure() == null 
				&& columnDiff == 0 
				&& field[move.getStartRow() + stepDir][move.getEndColumn()].getFigure() == null 
				&& startField.getFigure().moved == false) {
			return LogicConst.OK;
		}
		
		if(rowStep == stepDir && columnDiff == 1 && endField.getFigure() == null
				&& field[move.getStartRow()][move.getEndColumn()].getFigure() != null
				&& field[move.getStartRow()][move.getEndColumn()].getFigure().isWhite() != white
				&& field[move.getStartRow()][move.getEndColumn()].getFigure() instanceof Pawn) {
			return LogicConst.EN_PASSANT;
		}
		return LogicConst.ILLEGAL;
	}
	
	@Override
	public String toString() {
		return "P";
	}

}
