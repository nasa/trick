
//========================================
//    Package
//========================================
package trick.common.utils;

//========================================
//    Imports
//========================================
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.Scanner;

import trick.common.utils.LogVar.DisplayType;


/**
 * This data reader is for reading Trick log header data.
 * 
 *
 * @since Trick 10
 */
public class LogHeaderReader extends DataReader{

    //========================================
    //    Public data
    //========================================



    //========================================
    //    Protected data
    //========================================



    //========================================
    //    Private Data
    //========================================




    //========================================
    //    Constructors
    //========================================
    /**
     * Constructs a reader with specified file name.
     *
     * @param fileName    The name of the file.
     */
    public LogHeaderReader(String fileName) {
        super(fileName);
    }

    /**
     * Constructs a reader with specified file.
     *
     *  @param file        An instance of {@link File} for the reader to read.
     */
    public LogHeaderReader(File file) {
        super(file);
    }

    
    //========================================
    //    Set/Get methods
    //========================================



    //========================================
    //    Methods
    //========================================
    /**
     * Processes the recorded data. Required by {@link DataReader}.
     */
    @Override
	protected void processHeader() throws FileNotFoundException, IOException {
    	parseContents();
    }
    
    /**
     * Gets the contents from the file as a {@link List}.
     *
     * @return    A {@link List} of data objects representing all needed data in the recorded file.
     */
    private void parseContents() throws FileNotFoundException {
        Scanner scanner = new Scanner(dataFile);
        try {
            //first use a Scanner to get each line
            while ( scanner.hasNextLine() ){
                Object eachLine = processLine(scanner.nextLine());
                if (eachLine != null) {
                    recordedVarList.add(eachLine);
                }
            }
        }
        finally {
            //ensure the underlying stream is always closed
            scanner.close();
        }
    }
    
    /**
     * Processes a line of text. Required by {@link DataReader}.
     *
     * @return    An object of {@link LogVar} to represent a line of recorded header data,
     *             <code>null</code> if non-related data found in this line.
     */
	private Object processLine(String theLine) {
    	 if (theLine.endsWith("ASCII")) {
             return null;
         }
    	
        LogVar varObj = null;
        //use a second Scanner to parse the content of each line
        Scanner scanner = new Scanner(theLine);

        // by default, the scanner uses whitespace for the delimiter 
        // and this is what in recorded data
        if ( scanner.hasNext() ) {
            scanner.next(); // ignore the first item at each line
            String type = null;
            String units = null;
            String name = null;

            if (scanner.hasNext()) {
                type = scanner.next();
            }
            if (scanner.hasNext()) {
                units = scanner.next();
            }
            if (scanner.hasNext()) {
                name = scanner.next();
                if ( ! type.equals("byte_order")) {
                    varObj = new LogVar(name);
                    varObj.setUnits(units);
                    varObj.setRunDir(dataFile.getParent());
                    varObj.setDisplay(DisplayType.NORMAL);
                    varObj.setLevel(0);
                    varObj.setCount(1);
                    varObj.setIsFromLog(true);
                }
            }
        }
        //no need for finally here, since String is source
        scanner.close();

        return varObj;
    }

	@Override
	public double getVarValue() throws IOException {
		// there is no value in the header file
		return Double.NaN;
	}

	@Override
	public void endRead() {
		// do nothing
	}
}
