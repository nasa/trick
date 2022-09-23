
//========================================
//  Package
//========================================
package trick.simcontrol.utils;

//========================================
//  Imports
//========================================
import java.util.Calendar;
import java.util.TimeZone;

import trick.common.ui.UIUtils;

/**
 * The class that holds the data for Sim state.
 *
 * @since Trick 09
 */
public class SimState {
    //========================================
    //  Public data
    //========================================
    
    
    //========================================
    //  Protected data
    //========================================
	//public static final String[] SIM_MODE_DESCRIPTION = new String[] {
	//	"Init stepping", "Freeze", "Loading Checkpoint", "Dumping Checkpoint", "Debug Stepping", "Run", "Exit", "Sim Complete"
	//};
	
	public static final String[] SIM_MODE_DESCRIPTION = new String[] {"Init stepping", "Freeze", "Not Used" , "Not Used" , "Debug Stepping", "Run", "Exit", "Sim Complete"};
	
	public static final int INITIALIZATION_MODE = 0;
	public static final int FREEZE_MODE = 1;
	//public static final int LDCHCK_MODE = 2;
	//public static final int DMPCHCK_MODE = 3;
	public static final int DEBUG_STEPPING_MODE = 4;
	public static final int RUN_MODE = 5;
	public static final int EXIT_MODE = 6;
	public static final int COMPLETE_MODE = 7;
	
    //========================================
    //  Private Data
    //========================================
	
	private double execOutTime;
	private long metTime;
    private double realtimeTime;
    // whether realtime is active
    private int realtimeActive;
    private long gmtTime;
    private int mode = -1;
    //private Vector<Integer> overruns = new Vector<Integer>();
    private int overruns;
    private String runPath;
    private int currentPhase;
    private int maxPhase;
    private double simRealtimeRatio;
       
    
    //========================================
    //  Constructors
    //========================================
    
    
    //========================================
    //  Set/Get methods
    //========================================
    public void setExecOutTime(double time) {
    	execOutTime = time;
    }
    
    public void setMetTime(long date) {
    	metTime = date;
    }
    
    public void setRealtimeTime(double time) {
    	realtimeTime = time;
    }
    
    public void setRealtimeActive(int active) {
    	realtimeActive = active;
    }
    
    public void setGmtTime(long date) {
    	gmtTime = date;
    }
    
    public void setMode(int mode) {
    	this.mode = mode;
    }
       
    public void setRunPath(String path) {
    	runPath = path;
    }
    
/*
    public void setOverruns(int indx, int runs) {
    	overruns.add(new Integer(runs));
    }
*/
    public void setOverruns(int in_overruns) {
    	overruns = in_overruns ;
    }
    
    public void setCurrentPhase(int phase) {
    	currentPhase = phase;
    }
    
    public void setMaxPhase(int phase) {
    	maxPhase = phase;
    }
    
    public void setSimRealtimeRatio(double ratio) {
    	simRealtimeRatio = ratio;
    }
    
    public double getExecOutTime() {
    	return execOutTime;
    }
    
    public long getMetTime() {
    	return metTime;
    }
    
    public double getRealtimeTime() {
    	return realtimeTime;
    }
    
    public int getRealtimeActive() {
    	return realtimeActive;
    }
    
    public long getGmtTime() {
    	return gmtTime;
    }
     
    public int getMode() {
    	return mode;
    }
    
    public String getRunPath() {
    	return runPath;
    }
      
/*
    public int getOverruns(int indx) {
    	if (overruns !=null && overruns.size()>0) {
    		return ((Integer)overruns.elementAt(indx)).intValue();
    	} else {
    		return 0;
    	}
    }
*/
    public int getOverruns() {
       return (overruns);
    }
    
    public int getCurrentPhase() {
    	return currentPhase;
    }
    
    public int getMaxPhase() {
    	return maxPhase;
    }
    
    public double getSimRealtimeRatio() {
    	return simRealtimeRatio;
    }
    
    public String getTwoFractionFormatted(double n) {
    	return UIUtils.TWO_FRACTION_FORMATTER.format(n);
    }
    
    public String getCalendarFormatted(long t, String type) {  	
        Calendar now = Calendar.getInstance();
        int year = 0;
        int day = 0;
        // Always use GMT timezone instead of local one.
        now.setTimeZone(TimeZone.getTimeZone("GMT"));      
        now.setTimeInMillis(t);       
        // TODO: make sure that sys.exec.out.time retrieved from variable server is in seconds.
        now.add(Calendar.SECOND, (int)execOutTime);   
        year = now.get(Calendar.YEAR);
        year = year - 1970;
        day = now.get(Calendar.DAY_OF_YEAR);
        
        if ("MET".equals(type)) {
        	day--;
        }
        // In either ddd:hh:mm:ss or yyyy:ddd:hh:mm:sss format.  
        if (year == 0) {       	
        	return String.format("%1$03d:%2$tT", day, now);
        } else {      
        	return String.format("%1$04d:%2$03d:%3$tT", year, day, now);
        }
    }
    
    //========================================
    //  Methods
    //========================================
    
}
