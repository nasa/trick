
//========================================
//    Package
//========================================
package trick.common.utils;

//========================================
//    Imports
//========================================
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.Scanner;

import trick.common.utils.LogVar.DisplayType;
import trick.common.utils.UnitType.IllegalUnitConversionException;
import trick.dataproducts.trickqp.utils.ProductVar;


/**
 * This data reader is for retrieving the recorded variable list out of Trick recorded csv data.
 * 
 *
 * @since Trick 10
 */
public class CSVDataReader extends DataReader {

    //========================================
    //    Public data
    //========================================



    //========================================
    //    Protected data
    //========================================



    //========================================
    //    Private Data
    //========================================
    private BufferedReader bufferedDataReader = null;



    //========================================
    //    Constructors
    //========================================
    /**
     * Constructs a reader with specified file name.
     *
     * @param fileName    The name of the file.
     */
    public CSVDataReader(String fileName) {
        super(fileName);      
    }

    /**
     * Constructs a reader with specified file.
     *
     *  @param file        An instance of {@link File} for the reader to read.
     */
    public CSVDataReader(File file) {
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
    	Scanner scanner = new Scanner(dataFile);
        
    	try {
    		String headerLine = scanner.nextLine();
    		headerLine = headerLine.replaceAll("[{}]", "");
    		String[] tokens = headerLine.split(",");
    		
    		for (String eachToken : tokens) {
    			String[] varPair = eachToken.split("\\s+");
    			LogVar varObj = new LogVar(varPair[0]);
    			if (varPair.length > 1) {
    				varObj.setUnits(varPair[1]);
    			} 
    			// TODO: type?
    			varObj.setDisplay(DisplayType.NORMAL);
                varObj.setLevel(0);
                varObj.setCount(1);
                varObj.setIsFromLog(true);
                varObj.setRunDir(dataFile.getParent());
                recordedVarList.add(varObj);
    		}
    	}
    	
    	finally {
    		if (scanner != null) {
    			scanner.close();
    		}
    	}
    }
    
    /**
     * Starts the process for getting variable value.
     * 
     * @param var the variable name which need retrieving value for.
     * @throws IOException io exception
     */
    @Override
	public void beginRead(ProductVar var) throws IOException {
    	super.beginRead(var);
    	bufferedDataReader = new BufferedReader(new InputStreamReader(new FileInputStream(dataFile)));
        bufferedDataReader.readLine();  // ignore header
    }
    
    /**
     * Ends the process for getting value.
     */
    @Override
	public void endRead() {
    	if (bufferedDataReader != null) {
    		try {
				bufferedDataReader.close();
			} catch (IOException e) {
				e.printStackTrace();
			}
    	}
    }
    
    @Override
	public double getVarValue() throws IOException {
    	
        String oneLine = bufferedDataReader.readLine();
        
        while (oneLine != null && !oneLine.isEmpty()) {
            String[] dataStrList = oneLine.split(",");
            // if time stamp is not recorded, no need to verify it
            if (timeIndex > -1) {
	            theTimeValue = Double.parseDouble(dataStrList[timeIndex]);
	            if (!isTimeInRange(theTimeValue)){
	            	oneLine = bufferedDataReader.readLine();
	                continue;
	            }
            }
            double varData = Double.parseDouble(dataStrList[theVarIndex]);
            
            try {
				varData = applyVarProperties(varData);
			} catch (IllegalUnitConversionException e) {
				break;
			}
            return varData;
        }
    	return Double.NaN;
    }  
    
    @Override
	public boolean isEnd() {
    	boolean ret = false;
    	if (bufferedDataReader == null) {
    		ret = true;
    	}
    	try {
			if (bufferedDataReader.readLine() == null) {
				ret = true;
			}
		} catch (IOException e) {
			e.printStackTrace();
		}
    	return ret;
    }
}
