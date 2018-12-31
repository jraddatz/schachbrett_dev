package decoder;

import Constants.*;
import logic.Move;

public class Decoder {

	public static Move playerTurnToMove(byte[] input) {
		Move retVal = new Move(input[1], input[2], input[3], input[4]);
		return retVal;
	}
	
	public static Move engineTurnToMove(String turn) {
		int startColumn = DecoderConst.COLUMNMAP.get(turn.charAt(1));
		int endColumn = DecoderConst.COLUMNMAP.get(turn.charAt(2));
		int startRow = Integer.parseInt(turn.substring(0, 1));
		startRow--;
		int endRow = Integer.parseInt(turn.substring(3, 4));
		endRow--;
		Move retVal = new Move(startColumn, startRow, endColumn, endRow);
		return retVal;
	}
}
