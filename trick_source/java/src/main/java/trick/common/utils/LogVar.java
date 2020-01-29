
//========================================
//    Package
//========================================
package trick.common.utils;

//========================================
//    Imports
//========================================
import java.io.IOException;
import java.io.ObjectOutputStream;
import java.io.Serializable;
import java.util.regex.Matcher;
import java.util.regex.Pattern;


/**
 * Defines each variable object for recorded data.
 * 
 *
 * @since Trick 10
 */
public class LogVar implements Comparable<LogVar>, Serializable {

   	
	
	//========================================
    //    Public data
    //========================================
    public enum DisplayType { NORMAL, EXPANDED, CONTRACTED, HIDDEN }

    public static enum TrickType {
		TRICK_VOID,
		TRICK_CHARACTER,
		TRICK_UNSIGNED_CHARACTER,			
		TRICK_STRING,
		TRICK_SHORT,
		TRICK_UNSIGNED_SHORT,
		TRICK_INTEGER,
		TRICK_UNSIGNED_INTEGER,
		TRICK_LONG,
		TRICK_UNSIGNED_LONG,
		TRICK_FLOAT,
		TRICK_DOUBLE,
		TRICK_BITFIELD,
		TRICK_UNSIGNED_BITFIELD,
		TRICK_LONG_LONG,
		TRICK_UNSIGNED_LONG_LONG,
		TRICK_FILE_PTR,
		TRICK_BOOLEAN,
		TRICK_WCHAR,
		TRICK_WSTRING,
		TRICK_VOID_PTR,
		TRICK_ENUMERATED,
		TRICK_STRUCTURED,
		TRICK_USER_DEFINED_TYPE,
		TRICK_NUMBER_OF_TYPES		
	}
    
    //========================================
    //    Protected data
    //========================================



    //========================================
    //    Private Data
    //========================================
    private static final long serialVersionUID = 1136627204616267499L;
    
    
    private String name;
    private String units;
    private String runDir;     // for when run dir of this var is needed such as delta plot
    private TrickType type;	   // only needed when reading data out of a binary data file
    private int size;          // needed when reading data out of a binary data file
    private DisplayType display;  // tell how to display variable in  list
    private int level;         // number indicating what level of expansion this is for an arrayed variable
    private String sort_name;  // name used for sorting in displayed varlist
    private int count;         // count how many RUN dirs this variable is in

    private boolean isFromLog; // flag to indicate if this var is really from the log file, not by gui creation
    
    private DisplayType prevDisplay; // the place holder for previous display type of this variable, used if searching variable is performed

    //========================================
    //    Constructors
    //========================================
    /**
     * Constructor with specified variable name.
     *
     * @param name        The name of the sim variable.
     */
    public LogVar(String name) {
        this.name = name;
        this.setSortName(name);
    }
    /**
     * Copy constructor.
     *
     * @param obj         The LogVar object to copy.
     */
    public LogVar(Object obj) {
        if (! (obj instanceof LogVar)) {
            this.name = "";
            return;
        }
        LogVar source = (LogVar)obj;
        this.name = source.name;
        this.setSortName(source.name);
        this.units = source.units;
        this.runDir = source.runDir;
        this.type = source.type;
        this.display = source.display;
        this.level = source.level;
        this.count = source.count;
    }


    //========================================
    //    Set/Get methods
    //========================================

    /**
     * Sets the name for the variable.
     *
     * @param name    The variable name.
     */
    public void setName(String name) {
        this.name = name;
        this.setSortName(name);
    }

    /**
     * Sets the unit for the variable.
     *
     * @param units    The variable unit.
     */
    public void setUnits(String units) {
        this.units = units;
    }

    /**
     * Sets the run directory which the recorded data is in.
     *
     * @param dir    The run directory.
     */
    public void setRunDir(String dir) {
        runDir = dir;
    }
    
    /**
     * Sets the isFromLog flag.
     * @param b    value to set flag to.
     */
    public void setIsFromLog(boolean b) {
    	isFromLog = b;
    }
    
    /**
     * Sets the variable data type.
     *
     * @param type    The type of the variable.
     */
    public void setType(TrickType type) {
        this.type = type;
    }

    /**
     * Sets how to display this variable in list.
     *
     * @param display   true or false.
     */
    public void setDisplay(DisplayType display) {
         this.display = display;
    }

    /**
     * Sets expand level of this variable.
     *
     * @param level The expand level of the variable.
     */
    public void setLevel(int level) {
         this.level = level;
    }

    /**
     * Sets the variable's sort name.
     * (if it's an expanded variable, name contains parent array name; any indices get leading zeros)
     *
     * @param name    The sort name variable.
     */
    public void setSortName(String name) {
        this.sort_name = name;
        // add leading zeros to all array indices (e.g. so x[10] does not come before x[2])
        // is this slow? seems to work ok
        String regex;
        Pattern p;
        Matcher m;
        regex = "\\[(\\d\\d\\d)\\]"; // [nnn] -> [0nnn]
        p = Pattern.compile(regex);
        m = p.matcher(this.sort_name);
        this.sort_name = m.replaceAll("\\[00$1\\]");
        regex = "\\[(\\d\\d)\\]";    // [nn] -> [00nn]
        p = Pattern.compile(regex);
        m = p.matcher(this.sort_name);
        this.sort_name = m.replaceAll("\\[00$1\\]");
        regex = "\\[(\\d)\\]";       // [n] -> [000n]
        p = Pattern.compile(regex);
        m = p.matcher(this.sort_name);
        this.sort_name = m.replaceAll("\\[000$1\\]");
    }

    /**
     * Sets how many RUN dirs this variable belongs to.
     *
     * @param count The RUN dir count of the variable.
     */
    public void setCount(int count) {
         this.count = count;
    }
    
    /**
     * Sets the size of for this variable.
     * 
     * @param size	The size of this variable in bytes.
     */
	public void setSize(int size) {
		this.size = size;
	}
	
	/**
	 * Remembers the previous display type.
	 * 
	 * @param prevDisplay The previous {@link LogVar.DisplayType} of this variable.
	 */
	public void setPrevDisplay(DisplayType prevDisplay) {
		this.prevDisplay = prevDisplay;
	}
	
	/**
	 * Gets the previous display type.
	 * 
	 * @return The previous {@link LogVar.DisplayType} of this variable.
	 */
	public DisplayType getPrevDisplay() {
		return prevDisplay;
	}
	
	/**
	 * Gets the size of this variable.
	 * 
	 * @return The size of this variable in bytes.
	 */
	public int getSize() {
		return size;
	}
	
    /**
     * Gets the name of the variable.
     *
     * @return The name of the variable.
     */
    public String getName() {
        return name;
    }

    /**
     * Gets the units of the variable.
     *
     * @return The units of the variable.
     */
    public String getUnits() {
        return units;
    }

    /**
     * Gets the type of the variable
     *
     * @return The type of the variable.
     */
    public TrickType getType() {
        return type;
    }

    /**
     * Gets the run directory which the recorded data is in.
     *
     * @return The run directory.
     */
    public String getRunDir() {
        return runDir;
    }

    /**
     * Gets the flag of isFromLog.
     * @return true or false.
     */
    public boolean getIsFromLog() {
    	return isFromLog;
    }
    
    /**
     * Return if how th variable is to be displayed in list.
     *
     * @return true or false.
     */
    public DisplayType getDisplay() {
         return display;
    }

    /**
     * Gets expand level of this variable.
     *
     * @return The expand level of the variable.
     */
    public int getLevel() {
        return level;
    }

    /**
     * Gets the variable's sort name.
     *
     * @return The sort name variable.
     */
    public String getSortName() {
        return sort_name;
    }

    /**
     * Gets how many RUN dirs this variable belongs to.
     *
     * @return The count of the variable.
     */
    public int getCount() {
        return count;
    }

    //========================================
    //    Methods
    //========================================
    /**
     * Overrides toString() method in Object class so that it returns the value
     * as desired instead of the value of
     * <blockquote>
     * <pre>
     * getClass().getName() + '@' + Integer.toHexString(hashCode())
     * </pre></blockquote>
     *
     * @return a string representation of the object.
     */
    @Override
	public String toString() {
        return name + " (" + units + ")";
    }

    /**
     * Required by Comparable interface. When two LogVar objects compare, only
     * name gets compared.
     *
     * @param   o the object to be compared.
     * @return  a negative integer, zero, or a positive integer as this object
     *            is less than, equal to, or greater than the specified object.
     */
    // this is what is used when sorting items in varlist
    public int compareTo(LogVar o) {
        if (name != null && o.name != null) {
            // sort name makes expanded variables be shown directly below their parent
            // also indices have leading zeros so components shown in numerical order
            String compare1 = sort_name;
            String compare2 = o.sort_name;
            return compare1.compareTo(compare2);
        } else {
            return 0;
        }
    }

    // must override equals so that calling contains for list of LogVars will work
    @Override
    public boolean equals(Object o) {
        if (!(o instanceof LogVar)) {
            return false;
        }
        LogVar varObj = (LogVar) o;
        if (name != null && varObj.name != null) {
            return name.equals(varObj.name);
        } else {
            return false;
        }
    }

    // Serializable writeObject method - without this you get a NotSerializableException during drag/drop
    private void writeObject(
      ObjectOutputStream aOutputStream
    ) throws IOException {
      //perform the default serialization for all non-transient, non-static fields
      aOutputStream.defaultWriteObject();
    }
    
}
