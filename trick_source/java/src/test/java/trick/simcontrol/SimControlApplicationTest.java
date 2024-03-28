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

import trick.common.ApplicationTest;
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
public class SimControlApplicationTest extends ApplicationTest {
	private final static String SIM_DIR = "/test/SIM_gui_testing";
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
		String fullDir = getTrickHome() + SIM_DIR;

		SimulationInterface.cleanSim(fullDir);
		SimulationInterface.compileSim(fullDir);
		simProc = SimulationInterface.startSim(fullDir, "RUN_test/input.py");

		sleep(1000);
		
		try {
			Scanner infoReader = new Scanner(new File(fullDir + "/socket_info"));
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
		assumeTrue("Did not find the port number", port >= 0);
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

	@Test
	public void testStartSim() throws IOException {
		// ARRANGE
		String out; 
		int mode = 1, count = 0;
		
		simControl = new HeadlessSimControlApplication(host, port);
		sleep(1000);
		
		simControl.connect();
		sleep(1000);

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
		assertTrue("Sim Mode is not MODE_RUN(5)\nMODE_ID=" + mode, mode == MODE_RUN);

		// CLEAN UP
		varserv.put("trick.exec_freeze()\n");
	}

	@Test
	public void testFreezeSim() throws IOException {
		// ARRANGE
		String out; 
		int mode = 1, count = 0;
		
		simControl = new HeadlessSimControlApplication(host, port);
		sleep(1000);
		
		simControl.connect();
		sleep(1000);
		
		simControl.startSim();
		sleep(1000);

		varserv.put("trick.var_send_once(\"trick_sys.sched.mode\")");
		mode = Integer.parseInt(varserv.get().split("\t")[1]);
		assumeTrue("Sim Mode was not MODE_RUN at test start", mode == MODE_RUN);

		// ACT
		simControl.freezeSim();
		do {
			count ++;
			varserv.put("trick.var_send_once(\"trick_sys.sched.mode\")");
			mode = Integer.parseInt(varserv.get().split("\t")[1]);
		} while (mode != 1 && count < 100000);
		
		// ASSERT
		assertTrue("Sim Mode is not MODE_FREEZE (1)\nMODE_ID=" + mode, mode == MODE_FREEZE);
	}
	
	@Test
	public void testConnectionFail() {		
		// ARRANGE
		String expOutput = "Invalid TCP/IP port number \"0\"", output;

		String badH = "localhost";
		int badP = 0;
		simControl = new HeadlessSimControlApplication();
		System.setErr(new PrintStream(errContent));

		// ACT
		simControl.setHostPort(badH, badP);
		simControl.connect();
		sleep(500);

		// ASSERT
		output = errContent.toString();
		assertTrue("Did not recieve the expected error message: \n"
					+ output, output.indexOf(expOutput) >= 0);

		// CLEAN UP
		System.setErr(originalErr);
	}
}