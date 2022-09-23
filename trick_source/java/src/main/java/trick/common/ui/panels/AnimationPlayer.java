
//========================================
//  Package
//========================================
package trick.common.ui.panels;

//========================================
//  Imports
//========================================
import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.Image;
import java.awt.image.BufferedImage;
import java.io.IOException;
import java.io.InputStream;
import java.util.Iterator;

import javax.imageio.ImageIO;
import javax.imageio.ImageReader;
import javax.imageio.stream.ImageInputStream;
import javax.swing.ImageIcon;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.SwingWorker;

import trick.common.ui.UIUtils;

/**
 * An extension of JPanel that displays an animation image with the control capability.
 *
 * @since Trick 10
 */
public class AnimationPlayer extends JPanel {
	
	//========================================
	//	Public data
	//========================================
	
	
	//========================================
	//	Protected data
	//========================================
	
	//========================================
	//	Private Data
	//========================================
	 private boolean paused;
	 private boolean finished; 
	 private String animationFile;
	 private JLabel animationLabel;   
	 private PlayAnimationTask animationTask;
	 
	 
	 private static final long serialVersionUID = 3705588596523798631L;
	 
	//========================================
	//	Constructors
	//========================================
	/**
	 *	Default constructor.
         * @param fileName name of file
	 */
	public AnimationPlayer(String fileName) {
		animationFile = fileName;
		buildGUI();
		if (animationTask == null) {
			animationTask = new PlayAnimationTask();
		}
	}
	
	//========================================
	//	Set/Get methods
	//========================================
	
	
	//========================================
	//	Methods
	//========================================	
	/**
	 * Starts the animation task. This method has to be called
	 * in order for the animation to be played.
	 */
	public void start() {
		if (animationTask == null) {
			animationTask = new PlayAnimationTask();
		}	
		animationTask.execute();
	}
	
	/**
	 * Pauses the animation play.
	 */
	public void pause() {
		paused = true;
	}
	
	/**
	 * Resumes the animation play.
	 */
	public void resume() {
		paused = false;
	}
	
	/**
	 * Stops the animation play.
	 */
	public void stop() {
		finished = true;
	}
	
	/**
	 * Builds the player GUI.
	 */
	private void buildGUI() {
        setLayout(new BorderLayout());
        animationLabel = new JLabel();
        ImageIcon icon = UIUtils.createImageIcon(animationFile);
        // set proper initial size for the label
        if (icon != null) {
        	animationLabel.setPreferredSize(new Dimension(icon.getIconWidth(), icon.getIconHeight()));
        }
        add(animationLabel, BorderLayout.CENTER);    
    }
	
	//========================================
	//	Inner Class
	//========================================	
	/**
     * Inner class for playing an animation image.
     */
    private class PlayAnimationTask extends SwingWorker<Void, Void> {
    	ImageInputStream stream;
    	
    	@Override
    	public Void doInBackground() {   	
    		
    		if (animationFile == null) {
    			return null;
    		}
    		
            try {	         
	            InputStream input = UIUtils.getInputStreamForFile(animationFile);
	            stream = ImageIO.createImageInputStream(input);
	            Iterator readers = ImageIO.getImageReaders(stream);
	            if (!readers.hasNext()) {
					throw new RuntimeException("no image reader found");
				}
	            ImageReader reader = (ImageReader) readers.next();
	            reader.setInput(stream); // don't omit this line!
	            int n = reader.getNumImages(true); // don't use false!
	            
	            for (int i = 0; i < n; i++) {
	                BufferedImage image = reader.read(i);
	                Image img = image;            
	                animationLabel.setIcon(new ImageIcon(img));
	                do {
	                    try {            
	                        Thread.sleep(150);
	                    } catch (InterruptedException ie) {
	                    }        
	                }  while (paused);
	                     
	                if (finished) {
	                	break;
	                } else {
	                	// rewind
	                	if (i == n-1) {
	                		i = 0;                
	                	}                         
	                }
	            }
	            
            } catch (IOException ioe) {   
            	         
            }
            
    		return null;
    	}
            
    	@Override
		public void done() {
    		if (stream != null) {
                try {
                    stream.close();
                } catch (IOException ioe) { }
            }
    	}
    }
}
