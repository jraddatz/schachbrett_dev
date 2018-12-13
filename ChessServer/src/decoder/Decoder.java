package decoder;

import Constants.*;

public class Decoder {

	public static String decodeMessage(String encodedmessage) throws NumberFormatException{
		String decoded = "";
		if(encodedmessage.length() < DecoderConst.MIN_LENGTH) {
			return decoded;
		}
		int command = Integer.parseInt(encodedmessage.substring(DecoderConst.START_COMMAND, DecoderConst.END_COMMAND + 1), DecoderConst.HEXADECIMAL);
		
		switch(command) {
		case DecoderConst.NEW_COMMAND:
			decoded = ServerConst.NEW_GAME_MESSAGE;
			break;
		case DecoderConst.TURN_COMMAND:
			decoded = decodeTurn(encodedmessage);
			break;
		default:
			break;
		}
		
		return decoded;
	}
	
	private static String decodeTurn(String encodedTurn) throws NumberFormatException{
		String turn = "";
		if(encodedTurn.length() < DecoderConst.TURN_LENGTH) {
			return turn;
		}
		int columnFirst = Integer.parseInt(encodedTurn.substring(DecoderConst.START_FIRST_FIELD, DecoderConst.END_FIRST_FIELD), DecoderConst.HEXADECIMAL);
		int columnSecond = Integer.parseInt(encodedTurn.substring(DecoderConst.START_SECOND_FIELD, DecoderConst.END_SECOND_FIELD), DecoderConst.HEXADECIMAL);
		if(columnFirst >= DecoderConst.FIELD_COLUMN.length || columnSecond >= DecoderConst.FIELD_COLUMN.length
				|| columnFirst < 0 || columnSecond < 0) {
			return turn;
		} 
		turn += DecoderConst.FIELD_COLUMN[columnFirst];
		turn += encodedTurn.substring(DecoderConst.END_FIRST_FIELD, DecoderConst.END_FIRST_FIELD + 1);
		turn += DecoderConst.FIELD_COLUMN[columnSecond];
		turn += encodedTurn.substring(DecoderConst.END_SECOND_FIELD, DecoderConst.END_SECOND_FIELD + 1);
		
		if(encodedTurn.length() == DecoderConst.PROMOTION_LENGTH) {
			int promotion = Integer.parseInt(encodedTurn.substring(DecoderConst.PROMOTION_POSITION, DecoderConst.PROMOTION_LENGTH), DecoderConst.HEXADECIMAL);
			turn += DecoderConst.PROMOTION[promotion];
		}
		
		return turn;
	}
	
	public static String encodeTurn(String turn) {
		String encodedTurn = "";
		encodedTurn += DecoderConst.COLUMNMAP.get(turn.substring(DecoderConst.FIRST_COLUMN, DecoderConst.FIRST_COLUMN + 1));
		encodedTurn += turn.substring(DecoderConst.FIRST_ROW, DecoderConst.FIRST_ROW + 1);
		encodedTurn += DecoderConst.COLUMNMAP.get(turn.substring(DecoderConst.SECOND_COLUMN, DecoderConst.SECOND_COLUMN + 1));
		encodedTurn += turn.substring(DecoderConst.SECOND_ROW, DecoderConst.SECOND_ROW + 1);
		return encodedTurn;
	}
}
