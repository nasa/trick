package trick.jobperf;

import java.awt.*;
import java.awt.event.*;
import java.awt.image.BufferedImage;
import java.io.*;

import javax.imageio.ImageIO;
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
public class TraceViewInputToolBar extends JToolBar implements ActionListener {

    private TraceViewCanvas traceView;
    private JTextField frameSizeField;
    private JButton frameDetailsButton;
    private JButton advanceRangeButton;
    private JButton retreatRangeButton;
    private JTextField firstRenderFrameField;
    private JTextField lastRenderFrameField;
    /**
     * Constructor
     * @param tvc TraceViewCanvas to be controlled.
     */
    public TraceViewInputToolBar (TraceViewCanvas tvc) {
        traceView = tvc;

        add( new JLabel("           Frame Size (Avg): "));
        frameSizeField = new JTextField(10);
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

        frameDetailsButton = new JButton("Frame Details");
        frameDetailsButton.addActionListener(this);
        frameDetailsButton.setActionCommand("display-frame-details");
        frameDetailsButton.setToolTipText("Display the job details of the selected frame.");
        add(frameDetailsButton);

        add( new JLabel( String.format("          Frames : [%d ... %d]", 0, traceView.getFrameTotal()-1 )));

        add( new JLabel("    Selected Range: "));

        firstRenderFrameField = new JTextField(10);
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
        lastRenderFrameField = new JTextField(10);
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

        advanceRangeButton = new JButton("\u25bc");
        advanceRangeButton.addActionListener(this);
        advanceRangeButton.setActionCommand("advance-frame-range");
        advanceRangeButton.setToolTipText("Advance the selected range of frames to be displayed.");
        add(advanceRangeButton);

        retreatRangeButton = new JButton("\u25b2");
        retreatRangeButton.addActionListener(this);
        retreatRangeButton.setActionCommand("retreat-frame-range");
        retreatRangeButton.setToolTipText("Retreat the selected range of frames to be displayed.");
        add(retreatRangeButton);

        add( new JLabel("               "));

        // Add Trick LOGO.
        try { 
            BufferedImage image = ImageIO.read(getClass().getResource("/trick/common/resources/trick_small.gif"));
            add( new JLabel( new ImageIcon(image)));
        } catch (IOException e) {
            System.out.println("Failed to load image.");
        }
        
    }

    @Override
    public void actionPerformed(ActionEvent e) {
        String s = e.getActionCommand();
        switch (s) {
            case "display-frame-details":
                traceView.displaySelectedFrame();
            break;
            case "advance-frame-range":
                moveRenderFrameRangeBy(50);
            break;
            case "retreat-frame-range":
                moveRenderFrameRangeBy(-50);
            break;
            default:
                System.out.println("Unknown Action Command:" + s);
            break;
        }
    }

    private void moveRenderFrameRangeBy(int advance) {
        traceView.moveRenderFrameRangeBy(advance);
        firstRenderFrameField.setText( String.format("%d", traceView.getFirstRenderFrame()));
        lastRenderFrameField.setText( String.format("%d", traceView.getLastRenderFrame()));
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
