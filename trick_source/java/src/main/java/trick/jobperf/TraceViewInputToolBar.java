package trick.jobperf;

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import javax.swing.*;
import java.net.URL;

/**
* Class TraceViewInputToolBar initially displays an estimate of the frame size
* for the JobPerf input timeline data. A user may also enter the intended frame
* size into the JTextField, and pressing the "Set" button, which calls
* traceView.setFrameSize( newFrameSize );
*
* Class TraceViewInputToolBar aggregates the following GUI components:
* TraceViewInputToolBar (isa JToolBar)
*     JLabel ()
*     JTextField [frameSizeField]
*     JLabel
*     JLabel
*     JTextField [firstRenderFrameField]
*     JLabel
*     JTextField [lastRenderFrameField]
*/
public class TraceViewInputToolBar extends JToolBar {

    private TraceViewCanvas traceView;
    private JTextField frameSizeField;
    private JTextField firstRenderFrameField;
    private JTextField lastRenderFrameField;
    /**
     * Constructor
     * @param tvc TraceViewCanvas to be controlled.
     */
    public TraceViewInputToolBar (TraceViewCanvas tvc) {
        traceView = tvc;

        add( new JLabel(" Frame Size: "));
        frameSizeField = new JTextField(15);
        frameSizeField.setText( String.format("%8.4f", traceView.getFrameSize()) );
        add(frameSizeField);
        frameSizeField.addKeyListener( new KeyAdapter() {
            @Override
            public void keyPressed(KeyEvent e) {
                if (e.getKeyCode() == KeyEvent.VK_ENTER) {
                    setFrameSize();
                }
            }
        });

        add( new JLabel( String.format("    Total Frame Range: %d ... %d", 0, traceView.getFrameTotal()-1 )));

        add( new JLabel("    Selected Frame Range: "));

        firstRenderFrameField = new JTextField(15);
        firstRenderFrameField.setText( String.format("%d", traceView.getFirstRenderFrame()) );
        add(firstRenderFrameField);
        firstRenderFrameField.addKeyListener( new KeyAdapter() {
            @Override
            public void keyPressed(KeyEvent e) {
                if (e.getKeyCode() == KeyEvent.VK_ENTER) {
                     setFirstRenderFrame();
                }
            }
        });

        add( new JLabel("..."));
        lastRenderFrameField = new JTextField(15);
        lastRenderFrameField.setText( String.format("%d", traceView.getLastRenderFrame()) );
        add(lastRenderFrameField);
        lastRenderFrameField.addKeyListener( new KeyAdapter() {
            @Override
            public void keyPressed(KeyEvent e) {
                if (e.getKeyCode() == KeyEvent.VK_ENTER) {
                     setLastRenderFrame();
                }
            }
        });

        // Add Trick LOGO here.
    }

    private void setFirstRenderFrame() {
        int newStartFrame = 0;
        try {
            newStartFrame = Integer.parseInt( firstRenderFrameField.getText() );
            traceView.setFirstRenderFrame( newStartFrame );
        } catch ( NumberFormatException e) {
            firstRenderFrameField.setText( String.format("%d", traceView.getFirstRenderFrame()));
        } catch ( InvalidFrameBoundsExpection e) {
            firstRenderFrameField.setText( String.format("%d", traceView.getFirstRenderFrame()));
        }
    }

    private void setLastRenderFrame() {
        int newFinalFrame = 0;
        try {
            newFinalFrame = Integer.parseInt( lastRenderFrameField.getText() );
            traceView.setLastRenderFrame( newFinalFrame );
        } catch ( NumberFormatException e) {
            lastRenderFrameField.setText( String.format("%d", traceView.getLastRenderFrame()));
        } catch (InvalidFrameBoundsExpection e) {
            lastRenderFrameField.setText( String.format("%d", traceView.getLastRenderFrame()));
        }
    }

    private void setFrameSize() {
        double newFrameSize = 0.0;
        try {
            newFrameSize = Double.parseDouble( frameSizeField.getText() );
        } catch ( NumberFormatException e) {
            frameSizeField.setText( String.format("%8.4f", traceView.getFrameSize()) );
        }
        if ( newFrameSize > 0.0) {
            traceView.setFrameSize( newFrameSize );
        }
    }
}
