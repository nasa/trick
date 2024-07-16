package trick.common.utils;

import java.awt.event.ActionEvent;
import java.io.File;
import java.lang.reflect.Method;
import java.util.Properties;

import javax.swing.AbstractAction;
import javax.swing.Action;
import javax.swing.ImageIcon;

import trick.common.TrickApplication;

public class TrickAction extends AbstractAction {
	private TrickResources props;
	private TrickApplication invoker;
	private Method man;
	
	
	public TrickAction(TrickResources p, TrickApplication target, Method m) {
		props = new TrickResources(p);
		invoker = target;
		man = m;
		applyProperties();
	}

	@Override
    public void actionPerformed(ActionEvent actionEvent) {
        // System.out.println(props.getProperty("text"));
        try {
	        man.invoke(invoker);
        } catch (Exception e) {
        	System.err.println(e.getMessage());
        }
    }
    
    @Override
	public boolean isEnabled() {
		return super.isEnabled();
    }
    
    @Override
    public void setEnabled(boolean enabled) {
        super.setEnabled(enabled);
    }
    
    @Override
    public void putValue(String key, Object value) {
    	if(value != null)  super.putValue(key, value);
    }
    
    @Override
    public String toString() {
    	return "";
    }
    
    private void applyProperties() {
    	String propValue;
    	ImageIcon iconValue;
    	
    	putValue(Action.NAME, props.getString(TEXT_PROPERTY));
    	putValue(Action.SHORT_DESCRIPTION, 
    			 props.getString(SHORT_DESCRIPTION_PROPERTY));
    	putValue(Action.LONG_DESCRIPTION,
    			 props.getString(LONG_DESCRIPTION_PROPERTY));
    	
    	if((iconValue = props.getIcon(ICON_PROPERTY)) != null) {
    		putValue(Action.SMALL_ICON    , iconValue);
    		putValue(Action.LARGE_ICON_KEY, iconValue);
    	} else {
    		if((iconValue = props.getIcon(SMALL_ICON_PROPERTY)) != null) {
				putValue(Action.SMALL_ICON, iconValue);
			}
			
			if((iconValue = props.getIcon(LARGE_ICON_PROPERTY)) != null) {
				putValue(Action.LARGE_ICON_KEY, iconValue);
			}
    	}
    }
    
    private ImageIcon getIcon(String fileName) {
	    	String iconPath;
	    
    		if(fileName.indexOf("/") >= 0) {
    			iconPath = fileName;
			} else {
				iconPath = invoker.getResourcePath(invoker.getClass());
				int finalSlash = iconPath.lastIndexOf("/") + 1;
				iconPath = iconPath.substring(0, finalSlash) + fileName;
    		}
    		return new ImageIcon(iconPath);
    }
    
    private static String resolveFilePath(String PATH, String fileName) {
    	String paths[] = PATH.split(";");
    	String resolvedName;
    	File f;
    	
		for(String path : paths) {
			resolvedName = path + fileName;
			f = new File(resolvedName);
			if(f.exists() && !f.isDirectory())
				return resolvedName;
		}
		
		return fileName;
    }
    
    public static TrickResources extractProperties(TrickResources props, String name) {
		String head = name + ".Action.";
		String  textKey = head + TEXT_PROPERTY,
			    iconKey = head + ICON_PROPERTY,
			   sIconKey = head + SMALL_ICON_PROPERTY,
			   lIconKey = head + LARGE_ICON_PROPERTY, 
			   sDescKey = head + SHORT_DESCRIPTION_PROPERTY, 
			   lDescKey = head + LONG_DESCRIPTION_PROPERTY;
			   
		String textVal, sDescVal, lDescVal,
			   iconVal, sIconVal, lIconVal;
		
		TrickResources actionProp = new TrickResources();
		
		if((textVal = props.getProperty(textKey)) != null)
			actionProp.setProperty(TEXT_PROPERTY, textVal);
		
		if((sDescVal = props.getProperty(sDescKey)) != null) 
			actionProp.setProperty(SHORT_DESCRIPTION_PROPERTY, sDescVal);
		
		if((lDescVal = props.getProperty(lDescKey)) != null) 
			actionProp.setProperty(LONG_DESCRIPTION_PROPERTY, lDescVal);
		
		if((iconVal = props.getProperty(iconKey)) != null) {
			iconVal = resolveFilePath(props.getProperty("PATH"), iconVal);
			
			actionProp.setProperty(ICON_PROPERTY, iconVal);
		} else {
			if((sIconVal = props.getProperty(sIconKey)) != null) 
				actionProp.setProperty(SMALL_ICON_PROPERTY, sIconVal);
			
			if((lIconVal = props.getProperty(lIconKey)) != null) 
				actionProp.setProperty(LARGE_ICON_PROPERTY, lIconVal);
		}
		
		return actionProp;
    }
    
    
    private static final String TEXT_PROPERTY = "text";
    private static final String ICON_PROPERTY = "icon";
    private static final String SMALL_ICON_PROPERTY = "smallIcon";
    private static final String LARGE_ICON_PROPERTY = "largeIcon";
    private static final String SHORT_DESCRIPTION_PROPERTY = "shortDescription";
    private static final String LONG_DESCRIPTION_PROPERTY = "longDescription";
}