package logic;

public class Move {
	private int startRow;
	private int startColumn;
	private int endRow;
	private int endColumn;
	private byte promotion;
	
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
}
