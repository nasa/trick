
//========================================
//	Package
//========================================
package trick.common.ui;

//========================================
//	Imports
//========================================
import java.io.File;
import java.io.FileFilter;


/**
 * A {@link FileFilter} that accepts specified file or directory name.
 *
 * @author Hong Chen
 * @since Trick 10
 */
public class TrickFileFilter implements java.io.FileFilter { 
	
	//========================================
	//	Public data
	//========================================
	public final static int SIM_RUN = 1;
	public final static int SIM_DP = 2;
	public final static int LOG = 3;
	public final static int XML = 4;
	public final static int HEADER = 5;
	public final static int TRK = 6;
	public final static int CSV = 7;
	public final static int H5  = 8;
	// this includes all trick data record related files, .header, .trk, .csv and .h5
	public final static int TRICK_DATA_RECORD = 9;
	// for postscript files
	public final static int PS = 10;
	
	//========================================
	//	Protected data
	//========================================
	
	
	//========================================
	//	Private Data
	//========================================
	private int type;
	private String filenameMatch = null;
	
	//========================================
	//	Constructors
	//========================================
	/**
	 *	Default constructor.
         * @param type not sure.
	 */
	public TrickFileFilter(int type) {
		this.type = type;
	}
	
	//========================================
	//	Set/Get methods
	//========================================
	/**
	 * Gets the type of this {@link TrickFileFilter}.
	 * 
	 * @return The type of the filter.
	 */
	public int getType() {
		return type;
	}
	
	/**
	 * Sets file name mathing criteria for the filter.
	 * 
	 * @param matching	This filter cannot be accepted if {@link File} is a file and 
	 *                  its name doesn't contain the matching string.	
	 */
	public void setFilenameMatch(String matching) {
		filenameMatch = matching;
	}
	
	//========================================
	//	Methods
	//========================================
	/**
	 * Required by {@link FileFilter}.
	 * 
	 * @see javax.swing.filechooser.FileFilter#accept(java.io.File)
	 */
	public boolean accept(File f) {
		boolean ret;
		switch (type) {
			case SIM_RUN:
				// only directories are considered
				if (f.isFile()) {
					ret = false;
				}
				ret = f.getName().startsWith("SIM") || f.getName().startsWith("RUN") || f.getName().startsWith("SET") || f.getName().startsWith("MONTE_RUN");
				// ignore all RUN dirs under another RUN dirs unless these RUN dirs are under SIM_, SET_, or MONTE_RUN
				if (f.getName().startsWith("RUN") && f.getParentFile().getName().startsWith("RUN")) {
					ret = false;
				}
				break;
			case SIM_DP:
				ret = (f.isDirectory() && f.getName().startsWith("SIM"))
						|| (f.isFile() && f.getName().startsWith("DP"))
						|| (f.isDirectory() && f.getName().startsWith("SET"))
						|| (f.isDirectory() && f.getName().equals("DP_Product"))
						|| (f.isDirectory() && f.getName().equals("DP_Products"));
				break;
			case LOG:
				ret = f.isFile() && f.getName().toLowerCase().startsWith("log");
				break;
			case XML:
				ret = f.isFile() && f.getName().toLowerCase().endsWith(".xml");
				break;
			case HEADER:
				ret = f.isFile() && f.getName().toLowerCase().endsWith(".header");
				break;
			case TRK:
				ret = f.isFile() && f.getName().toLowerCase().endsWith(".trk");
				break;
			case CSV:
				ret = f.isFile() && f.getName().toLowerCase().endsWith(".csv");
				break;
			case H5:
				ret = f.isFile() && f.getName().toLowerCase().endsWith(".h5");
				break;
			case TRICK_DATA_RECORD:
				ret = f.isFile() && 
				      (f.getName().toLowerCase().endsWith(".header") || 
				          f.getName().toLowerCase().endsWith(".trk") || 
				          f.getName().toLowerCase().endsWith(".csv") || 
				          f.getName().toLowerCase().endsWith(".mat") || 
				          f.getName().toLowerCase().endsWith(".h5")
				      );
				break;
			case PS:
				ret = f.isFile() && f.getName().toLowerCase().endsWith(".ps");
				break;
			default:
				ret = true;
		}
		if (filenameMatch != null && filenameMatch.length() > 0) {
			// only check the additional match when is a file
			if (f.isFile()) {
				ret = ret && f.getName().contains(filenameMatch);
			}
		}
		return ret;
	}
	
}
