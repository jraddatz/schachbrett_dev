package Constants;

public class ServerConst {
	
	public static final int PORT = 3000;
	public static final byte START = 0b00000001;
	public static final byte RESET = 0b00000010;
	public static final byte NEW_TURN = 0b00000100;
	public static final byte PROMOTION_ACK = 0b00001000;
	public static final byte[] Ok_RESPONSE = {0b0000000};
	public static final byte[] ERROR_RESPONSE = {0b1000000};
	public static final int MAX_INPUT_LENGTH = 20;
	
	public static final byte PLAYER = 0;
	public static final byte ENGINE = 1;
}
