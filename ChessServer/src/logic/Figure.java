package logic;

public abstract class Figure {
	
	protected boolean moved;
	protected boolean white;

	
	
	public Figure(boolean white) {
		this.white = white;
		moved = false;
	}

	public boolean isWhite() {
		return white;
	}

	public boolean isMoved() {
		return moved;
	}
	
	public void setMoved(boolean moved) {
		this.moved = moved;
	}

	public boolean checkEndField(int endRow, int endColumn, Field[][] fields) {
		if(fields[endRow][endColumn].getFigure() == null) {
			return true;
		}
		if(fields[endRow][endColumn].getFigure().white == white) {
			return false;
		}
		return true;
	}

	abstract public byte legalMove(Move move, Field[][] field);
}
