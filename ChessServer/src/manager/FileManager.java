package manager;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.io.OutputStream;
import java.sql.Timestamp;

public class FileManager {
	
	private final String FILE_END = ".txt";
	
	private String historyName;
	FileWriter fw;
	BufferedWriter bw;
	
	
	public void createNewHistory() {
		Timestamp timestamp = new Timestamp(System.currentTimeMillis());
		historyName = timestamp.toString() + FILE_END;
		
		try {
			fw = new FileWriter(historyName);
		} catch (IOException e) {
			e.printStackTrace();
		}
		
	}
	
	public void appendTurnToHistory(String turn) {
		try {
			fw = new FileWriter(historyName,true);
			bw = new BufferedWriter(fw);
			bw.append(turn);
			bw.newLine();
			bw.newLine();
			bw.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}
