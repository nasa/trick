
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
import java.util.ArrayList;
import java.util.List;

import trick.common.TrickApplication;
import trick.dataproducts.trickqp.utils.ProductVar;


/**
 * This data reader is for reading recorded data from different type of files.
 *
 *
 * @since Trick 10
 */
public abstract class DataReader {

    //========================================
    //    Public data
    //========================================



    //========================================
    //    Protected data
    //========================================
    protected File dataFile;
    protected List<Object> recordedVarList;
    
    protected double theTimeValue = Double.NaN;
    protected int theVarIndex = -1;
    protected ProductVar theVar;
    
    protected Double tstart;
    protected Double tstop;
    protected Double frequency;
    // the index number of time name in the recordedVarList, by default, it's the 1st variable on the list
    protected int timeIndex = 0;
    
    protected String timename = TrickApplication.DEFAULT_TIME_NAME;
    
    //========================================
    //    Private Data
    //========================================
    private static double EPSILON = 1e-9;



    //========================================
    //    Constructors
    //========================================
    /**
     * Constructs the reader with specified file name.
     * Once the reader is created, its header is always 
     * processed so that the recorded variable list is 
     * ready to use.
     *
     * @param fileName    The name of the file for the reader to read.
     */
    public DataReader(String fileName) {
        this(new File(fileName));
    }

    /**
     * Constructs the reader with specified {@link File}.
     * Once the reader is created, its header is always 
     * processed so that the recorded variable list is 
     * ready to use.
     * 
     * @param file    An instance of {@link File} for the reader to read.
     */
    public DataReader(File file) {
        dataFile = file;
        init();
        try {
            processHeader();
        } catch (FileNotFoundException e) {
            System.out.println("Error: " + file.getPath() + " can not be found!");
        } catch (IOException e) {
            System.out.println("I/O Exception: " + file.getPath());
        }
        timeIndex = locateVarIndex(timename);
    }

    //========================================
    //    Set/Get methods
    //========================================
    /**
     * Gets the variable list that is recorded either from the header file or the data file.
     * 
     * @return 	A {@link List} of variables that are recorded.
     */
    public List<Object> getRecordedVarList() {
    	return recordedVarList;
    }
    
    /**
     * Sets the variable which the reader needs reading data for.
     * 
     * @param var	An instance of {@link ProductVar} for the variable.
     */
    public void setVar(ProductVar var) {
    	theVar = var;
    }
    
    /**
     * Sets the start time object for the XY data series.
     * 
     * @param start The start time object.
     */
    public void setTStart(Double start) {
        tstart = start;
    }
    
    /**
     * Sets the stop time object for the XY data series.
     * 
     * @param stop The stop time object;
     */
    public void setTStop(Double stop) {
        tstop = stop;
    }
    
    /**
     * Sets the time name for this data reader.
     * 
     * @param name  The time name for the data reader.
     */
    public void setTimename(String name) {
        // if name is null, name keeps its default that is DEFAULT_TIME_NAME(sys.exec.out.time)
        if (name != null && !name.equals(timename)) {
            timename = name;
            timeIndex = locateVarIndex(timename);
            // if can't find the specified timename, go back to the default one
            if (timeIndex == -1) {
            	timename = TrickApplication.DEFAULT_TIME_NAME;
            	timeIndex = locateVarIndex(timename);
            }
        }
    }
        
    /**
     * Gets the log variable for the specified variable.
     * @param varName  string of variable name
     * @return value of variable.
     */
    public LogVar getLogVar(String varName) {
        LogVar logVar = null;
        if (recordedVarList != null) {
            for (Object thisVar : recordedVarList) {
                if (thisVar instanceof LogVar) {
                    if (((LogVar)thisVar).getName().equals(varName)) {
                        logVar = (LogVar)thisVar;
                        break;
                    }
                }
            }
        }
        
        return logVar;
    }
    
    
    //========================================
    //    Methods
    //========================================
    /**
     * Initialize.
     */
    private void init() {
    	recordedVarList = new ArrayList<Object>();
    }
    
    
    /**
     * Starts the process for getting value.
     * 
     * @param var the variable name which need retrieving value for.
     * @throws IOException IOException
     */
    public void beginRead(ProductVar var) throws IOException {
    	theVarIndex = locateVarIndex(var.getName());
    	theVar = var;
    }
    
    /**
     * Gets the variable value.
     * 
     * @return The variable value.
     * 
     * @throws IOException IOException
     */
    public abstract double getVarValue() throws IOException;
        
    /**
     * Ends the process for getting value.
     */
    public abstract void endRead();
    
    /**
     * Checks to see if this reader reaches its end.
     * 
     * @return True if the reader reaches its end, false otherwise.
     */
    public boolean isEnd() {
    	return false;
    }
    
    /**
     * Returns the index number of a variable in the list based on its name.
     * @param varName string of variable name
     * @return index value in list
     */
    public int locateVarIndex(String varName) {
        int idx = -1;
        for (int i = 0; i < recordedVarList.size(); i++) {
            LogVar thisVar = (LogVar)recordedVarList.get(i);
            
            if (thisVar.getName().equals(varName)) {
                idx = i;
            } 
            if (idx != -1) {
                break;
            }
        }
        
        return idx;
    }
    
    /**
     * Checks to see if the time is in the range of specified tstart and tstop if applicable.
     * If the range is not specified, true is always returned.
     * 
     * @see #tstart
     * @see #tstop
     * @param timeData the time to check
     * @return true if time in range, else false
     */
    public boolean isTimeInRange(double timeData) {
        boolean ret = true;
        if (tstart != null || tstop != null) {
            ret = false;
            if (tstart != null) {
                if (timeData >= tstart.doubleValue()) {
                    ret = true;
                } else {
                    ret = false;
                }
            }
            if (tstop != null) {
                if (timeData <= tstop.doubleValue()) {
                    ret = ret && true;
                } else {
                    ret = false;
                }
            }
        }
        return ret;
    }
    
    /**
     * Gets the time stamp for the specified variable value retrieved.
     * @return the time
     */
    public double getTimeValue() {
    	return theTimeValue;
    }
    
    /**
     * Abstract method for processing the data recording file.
     * This method needs to be called after a {@link DataReader} instance
     * is created in order for recordedVarList to be filled in.
     * @throws FileNotFoundException FileNotFoundException
     * @throws IOException IOException
     */
    protected abstract void processHeader() throws FileNotFoundException, IOException;
    
    /**
     * Checks to see if 2 doubles are almost equal with specified Epsilon.
     * @param a first value
     * @param b second value
     * @param eps epsilon (tolerance)
     * @return true if a and b are within epsilon, else false
     */
    public static boolean nearlyEqual(double a, double b, double eps) {
    	if (a == b) {
    		return true;
    	}
    	return Math.abs(a - b) < eps * Math.max(Math.abs(a), Math.abs(b));
    }
    
    /**
     * Checks to see if 2 doubles are almost equal with the default Epsilon.
     * @param a first value
     * @param b second value
     * @return true if a and b are within defalut epsilon, else false
     */
    public static boolean nearlyEqual(double a, double b) {
    	return nearlyEqual(a, b, EPSILON);
	}
    
    /**
     * Updates the value of the variable properties such as scale, bias, and units.
     * @param varData value to set
     * @return the value of varData
     * @throws UnitType.IllegalUnitConversionException stuff
     */
	protected double applyVarProperties(double varData) throws UnitType.IllegalUnitConversionException {
		if (theVar.getScale() != null) {
			varData = theVar.getScale().doubleValue() * varData;
		}
		if (theVar.getBias() != null) {
			varData = varData + theVar.getBias().doubleValue();
		}
		
		if (theVar.getUnits() != null) {
			varData = convertUnits(varData, getLogVar(theVar.getName()), theVar);
		}
    	
		return varData;
	}
    	
	/**
	 * Converts log variable value to product variable value based on their units.
	 * 
	 * @throws UnitType.IllegalUnitConversionException if units conversion error.
	 */
	private double convertUnits(double fromValue, LogVar fromVar, ProductVar toVar) throws UnitType.IllegalUnitConversionException {
		double toValue = fromValue;
		String fromUnits = null;
		if (fromVar.getUnits() == null && toVar.getFromUnits() != null && !toVar.getFromUnits().isEmpty()) {
			fromUnits = toVar.getFromUnits();
		} else {
			fromUnits = fromVar.getUnits();
		}
		if (fromUnits != null && !fromUnits.equals(toVar.getUnits())) {			
			try {
				toValue = UnitType.convertUnits(fromValue, fromVar.getUnits(), toVar.getUnits());
			} catch (UnitType.IllegalUnitConversionException e) {
				throw new UnitType.IllegalUnitConversionException("===" + fromVar.getName() + ": [" + fromVar.getUnits() + "]", toVar.getName() + ": [" + toVar.getUnits() + "]");
			}
		}
		return toValue;
	}
}
