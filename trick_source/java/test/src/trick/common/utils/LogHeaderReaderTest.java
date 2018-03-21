package trick.common.utils;


import static org.junit.Assert.assertArrayEquals;

import java.io.FileNotFoundException;
import java.io.IOException;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;


public class LogHeaderReaderTest {
	
	@BeforeClass
	public static void setUpBeforeClass() throws Exception {		
	}

	@AfterClass
	public static void tearDownAfterClass() throws Exception {
	}

	@Before
	public void setUp() throws Exception {		
	}

	@After
	public void tearDown() throws Exception {		
	}

	@Test(expected = FileNotFoundException.class)
	public void testGetContentsWithNotFoundFile() throws FileNotFoundException, IOException {
		DataReader reader = new LogHeaderReader("resources" + java.io.File.separator + "unknowFile.header");
		reader.processHeader();
	}
	
	@Test
	public void testGetContents() throws FileNotFoundException {
		LogVar[] expectedVarList = new LogVar[9];
		
		expectedVarList[0] = new LogVar("sys.exec.out.time");
		expectedVarList[0].setUnits("s");
		expectedVarList[1] = new LogVar("ball.obj.state.output.position[0]");
		expectedVarList[1].setUnits("m");
		expectedVarList[2] = new LogVar("ball.obj.state.output.position[1]");
		expectedVarList[2].setUnits("m");
		expectedVarList[3] = new LogVar("ball.obj.state.output.velocity[0]");
		expectedVarList[3].setUnits("m/s");
		expectedVarList[4] = new LogVar("ball.obj.state.output.velocity[1]");
		expectedVarList[4].setUnits("m/s");
		expectedVarList[5] = new LogVar("ball.obj.state.output.acceleration[0]");
		expectedVarList[5].setUnits("m/s2");
		expectedVarList[6] = new LogVar("ball.obj.state.output.acceleration[1]");
		expectedVarList[6].setUnits("m/s2");
		expectedVarList[7] = new LogVar("ball.obj.state.output.external_force[0]");
		expectedVarList[7].setUnits("N");
		expectedVarList[8] = new LogVar("ball.obj.state.output.external_force[1]");
		expectedVarList[8].setUnits("N");
		
		// ASCII
		DataReader reader = new LogHeaderReader("resources" + java.io.File.separator + "log_Ball.header");		
		assertArrayEquals("Error in getContents for ASCII in DataReader.java", expectedVarList, reader.getRecordedVarList().toArray());
		
		// HDF5
		reader = new LogHeaderReader("resources" + java.io.File.separator + "log_Ball2.header");
		assertArrayEquals("Error in getContents for HDF5 in DataReader.java", expectedVarList, reader.getRecordedVarList().toArray());
		
		// little_endian
		reader = new LogHeaderReader("resources" + java.io.File.separator + "log_Ball3.header");
		assertArrayEquals("Error in getContents for little_endian in DataReader.java", expectedVarList, reader.getRecordedVarList().toArray());
	}
	
}
