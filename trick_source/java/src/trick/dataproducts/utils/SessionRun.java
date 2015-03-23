
//========================================
//  Package
//========================================
package trick.dataproducts.utils;

import java.io.File;
import java.io.Serializable;
//========================================
//  Imports
//========================================

/**
 * Defines run data for a {@link Session}.
 * 
 * @author Hong Chen
 * @since Trick 10
 */
public class SessionRun implements Comparable<SessionRun>, Serializable {
    //========================================
    //  Public data
    //========================================
    
    
    //========================================
    //  Protected data
    //========================================
	
	
    
    //========================================
    //  Private Data
    //========================================
    private static final long serialVersionUID = -6290142270526879885L;
    
	private String dir;
	private String timename;
	
	
    //========================================
    //  Constructors
    //========================================
	/**
	 * Constructs a SessionRun with specified run directory.
	 * 
	 * @param dir	The run directory.
	 */
    public SessionRun(String dir) {
    	this.dir = dir;
    }
    
    //========================================
    //  Set/Get methods
    //========================================
    /**
     * Sets the run directory.
     * 
     * @param dir	The run directory.
     */
    public void setDir(String dir) {
    	this.dir = dir; 
    }
  
    /**
     * Sets the time name for the {@link SessionRun}.
     * 
     * @param timename	The time name for the run.
     */
    public void setTimename(String timename) {
    	this.timename = timename;
    }
    
    /**
     * Gets the run directory.
     * 
     * @return The run directory.
     */
    public String getDir() {
    	return dir;
    }
    
    /**
     * Gets name of the run directory.
     * 
     * @return The name of run directory.
     */
    public String getDirName() {
        String dirName = null;
        if (dir != null) {
            dirName = dir.substring(dir.lastIndexOf(File.separator)+1);
        }
        return dirName;
    }
    
    /**
     * Gets the time name for the run.
     * 
     * @return The time name for the run.
     */
    public String getTimename() {
    	return timename;
    }
    
    //========================================
    //  Methods
    //========================================
    /**
     * Required by {@link Comparable} interface. When two SessionRun objects compare, only
     * dir gets compared.
     *
     * @param   o the object to be compared.
     * @return  a negative integer, zero, or a positive integer as this object
     *            is less than, equal to, or greater than the specified object.
     */
    public int compareTo(SessionRun o) {
        if (dir != null && o.getDir() != null) {
            return dir.compareTo(o.getDir());
        } else {
            return 0;
        }
    }

    @Override
    public boolean equals(Object o) {
        if (!(o instanceof SessionRun)) {
            return false;
        }
        SessionRun runObj = (SessionRun)o;
        if (dir != null && runObj.getDir() != null) {
            return dir.equals(runObj.getDir());
        } else {
            return false;
        }
    }
    
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
        return dir;
    }
    
    /**
     * Prints the run information.
     */
    public void printData() {
    	System.out.println("\tdir:\t" + dir);
    	System.out.println("\ttimename:\t" + timename);
    }
}
