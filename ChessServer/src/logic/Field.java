package logic;

public class Field {
	private Figure figure;
	
	public Figure getFigure() {
		return figure;
	}

	public void setFigure(Figure figure) {
		this.figure = figure;
	}
	
	@Override
	public String toString() {
		if(figure == null) {
			return "_";
		}
		return figure.toString();
	}
}
