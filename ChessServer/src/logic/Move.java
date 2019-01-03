package logic;

import Constants.DecoderConst;

public class Move {
	private int startRow;
	private int startColumn;
	private int endRow;
	private int endColumn;
	private byte promotion = 0;
	
	public Move(int startRow, int startColumn, int endRow, int endColumn) {
		this.startRow = startRow;
		this.startColumn = startColumn;
		this.endRow = endRow;
		this.endColumn = endColumn;
	}
	public int getStartRow() {
		return startRow;
	}
	public int getStartColumn() {
		return startColumn;
	}
	public int getEndRow() {
		return endRow;
	}
	public int getEndColumn() {
		return endColumn;
	}
	
	public byte getPromotion() {
		return promotion;
	}
	public void setPromotion(byte promotion) {
		this.promotion = promotion;
	}
	
	@Override
	public String toString() {
		String retVal = "";
		retVal += DecoderConst.FIELD_COLUMN[startColumn];
		int row = startRow + 1;
		retVal += row;
		retVal += DecoderConst.FIELD_COLUMN[endColumn];
		row = endRow + 1;
		retVal += row;
		if(promotion != 0) {
			retVal += DecoderConst.PROMOTION_MAP_REV.get(promotion);
		}
		return retVal;
	}
}
