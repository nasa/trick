package trick.simcontrol;


import java.beans.Transient;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.PrintStream;
import java.util.Arrays;
import java.util.Scanner;

import org.jdesktop.application.Application;
import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;
import static org.junit.Assert.assertTrue;
import static org.junit.Assume.assumeTrue;

import trick.common.CheckApplicationProperties;
import trick.common.SimulationInterface;
import trick.common.utils.VariableServerConnection;

/**
 * 
 * Test SimControlApplication life cycle.
 * 
 * @author hchen
 * @intern mrockwell2
 *
 */
public class SimControlApplicationTest {
	private final static String SIM_DIR = "/home/cacistudent/trick/test/SIM_gui_testing";
	private final ByteArrayOutputStream stdContent = new ByteArrayOutputStream();
	private final PrintStream originalStd = System.out;
	private final ByteArrayOutputStream errContent = new ByteArrayOutputStream();
	private final PrintStream originalErr = System.err;

	private final int MODE_FREEZE = 1, 
					     MODE_RUN = 5;

	private static HeadlessSimControlApplication simControl;
	private static VariableServerConnection varserv;
	private static Process simProc;
	private static String host;
	private static int port;
		
	@BeforeClass
	public static void setUpBeforeClass() {
		SimulationInterface.cleanSim(SIM_DIR);
		SimulationInterface.compileSim(SIM_DIR);
		simProc = SimulationInterface.startSim(SIM_DIR, "RUN_test/input.py");

		sleep(1000);
		
		try {
			Scanner infoReader = new Scanner(new File(SIM_DIR + "/socket_info"));
			String line = infoReader.nextLine();

			host = line.split(":")[0];
			port = Integer.parseInt(line.split(":")[1]);

			varserv = new VariableServerConnection(host, port);
		} catch (Exception e) {
			System.err.println(e.getMessage());
			host = "";
			port = -1;
			varserv = null;
		}
		
		assumeTrue("Did not connect to the variable server", varserv != null);
		assumeTrue("Did not find the host name.", !host.isEmpty());
		assumeTrue("Did not find the port numbber", port >= 0);
	}

	@AfterClass
	public static void tearDownAfterClass() {
		// if(simControl != null) {
		// 	simControl.freezeSim();
		// 	simControl.shutdown();
		// }
		if(simProc != null && simProc.isAlive())
			simProc.destroy();
	}

	@Before
	public void setUp() {		
		// System.setOut(new PrintStream(stdContent));
		System.setErr(new PrintStream(errContent));
	}

	@After
	public void tearDown() {
		// System.setOut(originalStd);
		System.setErr(originalErr);

		// stdContent.reset();
		errContent.reset();
	}

	@Test
	public void testStartSim() throws IOException {
		// ARRANGE
		System.out.println("START TEST");
		simControl = new HeadlessSimControlApplication(host, port);
		sleep(1000);
		System.out.println("CONNECT");
		simControl.connect();
		String out; 
		int mode = 1, count = 0;

		// ACT
		varserv.put("trick.var_send_once(\"trick_sys.sched.mode\")");
		mode = Integer.parseInt(varserv.get().split("\t")[1]);
		assumeTrue("Sim Mode was not MODE_FREEZE at test start", mode == MODE_FREEZE);
		simControl.startSim();
		do {
			count ++;
			varserv.put("trick.var_send_once(\"trick_sys.sched.mode\")");
			mode = Integer.parseInt(varserv.get().split("\t")[1]);
		} while (mode != 5 && count < 100000);
		
		// ASSERT
		System.out.println(errContent.toString());
		assertTrue("Sim Mode is not MODE_RUN(5)\nMODE_ID=" + mode, mode == MODE_RUN);

	}
	
	@Test
	public void testConnectionFail() {		
		// ARRANGE
		String[] expOutput = {  "Sim Control Error at Initialization: ",
								"Error: SimControlApplication:getInitializationPacket()",
								" Invalid TCP/IP port number \"0\"",
								" Please check the server and enter a proper port number!",
								" IOException ...java.net.ConnectException: Connection refused (Connection refused)",
								" If there is no connection, please make sure SIM is up running properly!"  },
				 output;

		String badH = "localhost";
		int badP = 0;
		simControl = new HeadlessSimControlApplication();

		// ACT
		simControl.setHostPort(badH, badP);
		simControl.connect();

		// ASSERT
		output = errContent.toString().split("\n");
		assertTrue("Did not recieve the expected error message: \n"
					 	+ Arrays.toString(output),
				   compStringArray(expOutput, output));
	}

	public boolean compStringArray(String[] a, String[] b) {
		boolean same = true;
		for(int i = 0; i < a.length && i < b.length; i++) {
			same = same && sameLetters(a[i], b[i]);
			if(!same) return same;
		}
		return same;
	}

	public boolean sameLetters(String str1, String str2) {
		String a = str1.replaceAll("\\s+", "").toLowerCase(),
			   b = str2.replaceAll("\\s+", "").toLowerCase();
		return a.equals(b);
	}

	public static void sleep(long ms) {
		try {Thread.sleep(ms);} catch(Exception ignored) {}
	}
}