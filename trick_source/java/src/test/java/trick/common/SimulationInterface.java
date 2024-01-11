package trick.common;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.FilenameFilter;
import java.io.IOException;
import java.util.Scanner;

public class SimulationInterface {
	public File working_directory, S_main;

	public SimulationInterface(String path) {
		working_directory = new File(path);
		find_S_main();
		if(S_main == null) {
			compile_sim();
			find_S_main();
		}
	}

	public void clean() {
		String infoPath = working_directory.getAbsolutePath() + "/socket_info";
		File info = new File(infoPath);
		if(info.exists())	info.delete();
	}

	public void compile_sim() {
		try {
			if (working_directory.exists()) {
				String cmd = "trick-CP";
				Process p = Runtime.getRuntime().exec(cmd, null, working_directory);
				while(p.isAlive()) {
					continue;
				}

			} else {
				System.err.println("Cannot find the Simulation Directory: " + working_directory.getAbsolutePath());
			}
		} catch(IOException e) {
			System.err.println(e.getMessage());
		}
	}

	public void find_S_main() {
		if (!working_directory.exists()) {
			System.err.println("Cannot find the Simulation Directory: " + working_directory.getAbsolutePath());
			S_main = null;
			return;
		}

		File[] exe = working_directory.listFiles(new FilenameFilter() {
			public boolean accept(File d, String name) {
				return name.startsWith("S_main") && name.endsWith("exe");
			}
		});

		if (exe.length > 0) {
			if (exe.length > 1) {
				System.out.printf("WARNING: Detected %d simulation executables. Selecting '%s'...\n", exe.length,  exe[0].getName());
			}

			S_main = exe[0];
		} else {
			S_main = null;
		}
	}

	public File get_S_main() {
		return S_main;
	}

	public Process run_S_main(String ... args) throws IOException {
		if (!validate_S_main())	return null;

		String cmd = "./" + S_main.getName();
		for (String arg : args)	cmd += " " + arg;

		working_directory = S_main.getParentFile();
		return Runtime.getRuntime().exec(cmd, null, working_directory);
	}

	public String get_var_server_connection() {
		return get_var_server_connection("");
	}

	public String get_var_server_connection(String delim) {
		// Variable declaration
		String path = working_directory.getAbsolutePath(), info = "";
		File socketInfo;
		Scanner siReader;

		// File Scanner Set up
		socketInfo = new File(path + "/socket_info");
		try {
			siReader = new Scanner(socketInfo);

			// Data Extraction and Clean-Up
			info = siReader.nextLine();
			siReader.close();
		} catch (FileNotFoundException e) {
			System.err.println(e.getMessage());
		}

		String tokens[] = info.split(",");
		if(!delim.isEmpty() && tokens.length == 2) {
			info = tokens[0] + delim + tokens[1];
		}

		return info;
	}
	
	private boolean validate_S_main() {
		return S_main.exists() && S_main.getName().startsWith("S_main") && S_main.getName().endsWith(".exe");
	}
}