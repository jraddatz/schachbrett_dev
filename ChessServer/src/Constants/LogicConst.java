package Constants;

public class LogicConst {
	public static final byte OK = 0b00000000;
	public static final byte CASTLING = 0b000000010;
	public static final byte CASTLING_AND_CHECK = 0b00010010;
	public static final byte CASTLING_AND_CHECKMATE = 0b00100010;
	public static final byte EN_PASSANT = 0b00000100;
	public static final byte EN_PASSANT_AND_CHECK = 0b00010100;
	public static final byte EN_PASSANT_AND_CHECKMATE = 0b00101000;
	public static final byte PROMOTION = 0b00001000;
	public static final byte CHECK = 0b00010000;
	public static final byte CHECKMATE = 0b00100000;
	public static final byte ILLEGAL = 0b01000000;
	
	public static final byte WHITE_SHORT_CASTLE = 0b1000001;
	public static final byte WHITE_LONG_CASTLE = 0b1000010;
	public static final byte BLACK_SHORT_CASTLE = 0b1000100;
	public static final byte BLACK_LONG_CASTLE = 0b1001000;
	
	public static final byte KNIGHT = 'N';
	public static final byte BISHOP = 'B';
	public static final byte ROOK = 'R';
	public static final byte QUEEN = 'Q';
			
	
	public static final int ROWS = 8;
	public static final int COLUMS = 8;
	public static final int LOWEST_ROW = 0;
	public static final int LOWEST_COLUMN = 0;
	
	public static final int WHITE_PAWN_ROW = 1;
	public static final int BLACK_PAWN_ROW = 6;
	
	public static final int COMMAND_LENGTH = 7;
	public static final int ENGINE_TURN_LENGTH = 11;
	
	public static final byte ENGINE_OK = 64;
	public static final byte ENGINE_CASTLING = 2;
	public static final byte ENGINE_EN_PASSANT = 4;
	public static final byte ENGINE_PROMOTION = 8;
	public static final byte ENGINE_CHECK = 16;
	public static final byte ENGINE_CHECKMATE = 32;
}
