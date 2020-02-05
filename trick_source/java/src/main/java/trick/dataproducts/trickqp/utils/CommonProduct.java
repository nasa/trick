
//========================================
//  Package
//========================================
package trick.dataproducts.trickqp.utils;

//========================================
//  Imports
//========================================

import java.awt.Color;

import trick.common.ui.UIUtils;

/**
 * Defines common product data for Trick QP GUI. 
 * This data can be shared among certain product XML elements.
 * 
 * @since Trick 10
 */
public abstract class CommonProduct {
	
    //========================================
    //  Public data
    //========================================
	public static final String DEFAULT_COLOR_TEXT = "Default";
    
    //========================================
    //  Protected data
    //========================================
	
	
    
	//========================================
    //  Private Data
    //========================================
	private String title;
	private Double tstart;
	private Double tstop;
	private Double frequency;
	private String foregroundColorCode = null;
	private String backgroundColorCode = null;
	
    //========================================
    //  Constructors
    //========================================
	
    
    //========================================
    //  Set/Get methods
    //========================================
	/**
	 * Sets the title.
	 * 
	 * @param title		The tile of the related data.
	 */
	public void setTitle(String title) {
		this.title = title;
	}
	
    /** 
     * Sets the start time.
     * 
     * @param tstart	The start time of the related data.
     */
    public void setTStart(double tstart) {
    	this.tstart = tstart;
    }
    
    /**
     * Sets the start time.
     * 
     * @param tstartStr	The start time in text.
     */
    public void setTStart(String tstartStr) {
    	try {
			tstart = Double.valueOf(tstartStr);
		} catch (NumberFormatException e) {
			tstart = null;
		} catch (NullPointerException e) {
			tstart = null;
		}
    }
    
    /**
     * Sets the start time.
     * 
     * @param tstartObj	An instance of {@link Double} that represents the start time.
     */
    public void setTStart(Double tstartObj) {
    	tstart = tstartObj;
    }
    
    /**
     * Sets the stop time.
     * 
     * @param tstop		The stop time of the related data.
     */
    public void setTStop(double tstop) {
    	this.tstop = tstop;
    }
    
    /**
     * Sets the stop time.
     * 
     * @param tstopStr	The stop time in text.
     */
    public void setTStop(String tstopStr) {
    	try {
			tstop = Double.valueOf(tstopStr);
		} catch (NumberFormatException e) {
			tstop = null;
		} catch (NullPointerException e) {
			tstop = null;
		}
    }
    
    /**
     * Sets the stop time.
     * 
     * @param tstopObj	An instance of {@link Double} that represents the stop time.
     */
    public void setTStop(Double tstopObj) {
    	tstop = tstopObj;
    }
    
    /**
     * Sets the frequency.
     * 
     * @param freq	The frequency of the related data.
     */
    public void setFrequency(double freq) {
    	frequency = freq;
    }
    
    /**
     * Sets the frequency.
     * 
     * @param freqStr	The frequency in text.
     */
    public void setFrequency(String freqStr) {
    	try {
			frequency = Double.valueOf(freqStr);
		} catch (NumberFormatException e) {
			frequency = null;
		} catch (NullPointerException e) {
			frequency = null;
		}
    }
    
    /**
     * Sets the frequency.
     * 
     * @param freqObj	An instance of {@link Double} that represents the frequency.
     */
    public void setFrequency(Double freqObj) {
    	frequency = freqObj;
    }
    
    /**
     * Sets the foreground color.
     * 
     * @param fColor	The foreground color Hex code.
     */
    public void setForegroundColorCode(String fColor) {
    	foregroundColorCode = fColor;
    }
    
    /**
     * Sets the background color.
     * 
     * @param bColor	The background color Hex code.
     */
    public void setBackgroundColorCode(String bColor) {
    	backgroundColorCode = bColor;
    }
    
    /**
     * Gets the title.
     * 
     * @return The title of the related data.
     */
    public String getTitle() {
    	return title;
    }
    
    /**
     * Gets the start time.
     * 
     * @return The start time.
     */
    public Double getTStart() {
    	return tstart;
    }
    
    /**
     * Gets the stop time.
     * 
     * @return The stop time.
     */
    public Double getTStop() {
    	return tstop;
    }
    
    /**
     * Gets the frequency.
     * 
     * @return The frequency.
     */
    public Double getFrequency() {
    	return frequency;
    }
    
    /**
     * Gets the foreground color Hex code.
     * 
     * @return 	The foreground color Hex code.
     */
    public String getForegroundColorCode() {
    	// no need to return the "Default" color text
    	if (foregroundColorCode != null && foregroundColorCode.equals(DEFAULT_COLOR_TEXT)) {
    		return null;
    	}
    	return foregroundColorCode;
    }
    
    /**
     * Gets the background color Hex code.
     * 
     * @return	The background color Hex code.
     */
    public String getBackgroundColorCode() {
    	// no need to return the "Default" color text
    	if (backgroundColorCode != null && backgroundColorCode.equals(DEFAULT_COLOR_TEXT)) {
    		return null;
    	}
    	return backgroundColorCode;
    }
    
    /**
     * Gets the foreground {@link Color}.
     * 
     * @return	The background {@link Color} instance.
     */
    public Color getForegroundColor() {
    	return UIUtils.getColorFromHTMLHex(getForegroundColorCode());
	}
    
    /**
     * Gets the background {@link Color}.
     * 
     * @return	The background {@link Color} instance.
     */
    public Color getBackgroundColor() {
		return UIUtils.getColorFromHTMLHex(getBackgroundColorCode());
	}
    
    
    //========================================
    //  Methods
    //========================================
    
    /**
     * Removes a specified child from its child list.
     * 
     * @param child	The child object that needs removing.
     */
    public abstract void removeChild(Object child);
    
    /**
     * Removes a child at specified index from its child list.
     * 
     * @param idx	The index at which the child needs removing.
     */
    public abstract void removeChild(int idx);
    
}
