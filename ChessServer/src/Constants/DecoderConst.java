package Constants;

import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

public class DecoderConst {
	
	public static final int MIN_LENGTH = 2;
	public static final int START_COMMAND = 0;
	public static final int END_COMMAND = 1;
	public static final int START_FIRST_FIELD = 2;
	public static final int END_FIRST_FIELD = 3;
	public static final int START_SECOND_FIELD = 4;
	public static final int END_SECOND_FIELD = 5;
	public static final int TURN_LENGTH = 6;
	public static final int PROMOTION_LENGTH = 7;
	public static final int PROMOTION_POSITION = 6;
	
	public static final int NEW_COMMAND = 0x01;
	public static final int TURN_COMMAND = 0x02;
	
	public static final int HEXADECIMAL = 0x16;
	
	public static final int FIRST_COLUMN = 0;
	public static final int FIRST_ROW = 1;
	public static final int SECOND_COLUMN = 2;
	public static final int SECOND_ROW = 3;
	
	public static final String [] FIELD_COLUMN = {"error", "a", "b", "c", "d", "e", "f", "g", "h"};
	public static final String [] PROMOTION = {"q", "n", "r", "b"};
	
	public static final Map <String,String>COLUMNMAP;
	static {
		HashMap<String,String> tmp = new HashMap<String,String>();
		tmp.put("a", "1");
		tmp.put("b", "2");
		tmp.put("c", "3");
		tmp.put("d", "4");
		tmp.put("e", "5");
		tmp.put("f", "6");
		tmp.put("g", "7");
		tmp.put("h", "8");
		COLUMNMAP = Collections.unmodifiableMap(tmp);
	}
}
