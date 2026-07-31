package trick.jobperf;
import java.awt.*;
import java.awt.event.*;
import java.awt.image.BufferedImage;
import java.io.*;
import javax.imageio.ImageIO;
import javax.swing.*;

/**
 * Class TraceViewInputToolBar controls the TraceViewCanvas.
 *
 * Zoom controls:
 *   Zoom is now driven by rubber-band mouse drag on the canvas itself.
 *   The "Zoom Reset" button here resets all zoom levels at once.
 *   Right-click on the canvas steps back one zoom level.
 */
public class TraceViewInputToolBar extends JToolBar implements ActionListener {
    private TraceViewCanvas traceView;
    private JTextField frameSizeField;
    private JButton frameDetailsButton;
    private JButton zoomResetButton;
    private JButton advanceRangeButton;
    private JButton retreatRangeButton;
    private JTextField firstRenderFrameField;
    private JTextField lastRenderFrameField;

    public TraceViewInputToolBar(TraceViewCanvas tvc) {
        traceView = tvc;

        add(new JLabel("           Frame Size (Avg): "));
        frameSizeField = new JTextField(10);
        frameSizeField.setText(String.format("%8.4f", traceView.getFrameSize()));
        add(frameSizeField);
        frameSizeField.addKeyListener(new KeyAdapter() {
            @Override
            public void keyPressed(KeyEvent e) {
                if (e.getKeyCode() == KeyEvent.VK_ENTER) setFrameSize();
            }
        });

        frameDetailsButton = new JButton("Frame Details");
        frameDetailsButton.addActionListener(this);
        frameDetailsButton.setActionCommand("display-frame-details");
        frameDetailsButton.setToolTipText("Display the job details of the selected frame.");
        add(frameDetailsButton);

        zoomResetButton = new JButton("\u2b73 Zoom Reset");
        zoomResetButton.addActionListener(this);
        zoomResetButton.setActionCommand("zoom-reset");
        zoomResetButton.setToolTipText("Reset all zoom levels back to the original view.");
        zoomResetButton.setEnabled(false);
        add(zoomResetButton);

        add(new JLabel(String.format("          Frames : [%d ... %d]", 0, traceView.getFrameTotal() - 1)));
        add(new JLabel("    Selected Range: "));

        firstRenderFrameField = new JTextField(10);
        firstRenderFrameField.setText(String.format("%d", traceView.getFirstRenderFrame()));
        add(firstRenderFrameField);
        firstRenderFrameField.addKeyListener(new KeyAdapter() {
            @Override
            public void keyPressed(KeyEvent e) {
                if (e.getKeyCode() == KeyEvent.VK_ENTER) setFirstRenderFrame();
            }
        });

        add(new JLabel("..."));

        lastRenderFrameField = new JTextField(10);
        lastRenderFrameField.setText(String.format("%d", traceView.getLastRenderFrame()));
        add(lastRenderFrameField);
        lastRenderFrameField.addKeyListener(new KeyAdapter() {
            @Override
            public void keyPressed(KeyEvent e) {
                if (e.getKeyCode() == KeyEvent.VK_ENTER) setLastRenderFrame();
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

        add(new JLabel("               "));

        try {
            BufferedImage image = ImageIO.read(getClass().getResource("/trick/common/resources/trick_small.gif"));
            add(new JLabel(new ImageIcon(image)));
        } catch (IOException e) {
            System.out.println("Failed to load image.");
        }
    }

    /** Call after any zoom operation to keep the Zoom Reset button in sync. */
    public void updateZoomButtons() {
        zoomResetButton.setEnabled(traceView.isZoomed());
        firstRenderFrameField.setText(String.format("%d", traceView.getFirstRenderFrame()));
        lastRenderFrameField.setText(String.format("%d", traceView.getLastRenderFrame()));
    }

    @Override
    public void actionPerformed(ActionEvent e) {
        switch (e.getActionCommand()) {
            case "display-frame-details":
                traceView.displaySelectedFrame();
                break;
            case "zoom-reset":
                traceView.zoomReset();
                updateZoomButtons();
                break;
            case "advance-frame-range":
                moveRenderFrameRangeBy(50);
                break;
            case "retreat-frame-range":
                moveRenderFrameRangeBy(-50);
                break;
            default:
                System.out.println("Unknown Action Command: " + e.getActionCommand());
        }
    }

    private void moveRenderFrameRangeBy(int advance) {
        traceView.moveRenderFrameRangeBy(advance);
        firstRenderFrameField.setText(String.format("%d", traceView.getFirstRenderFrame()));
        lastRenderFrameField.setText(String.format("%d", traceView.getLastRenderFrame()));
    }

    private void setFirstRenderFrame() {
        try {
            traceView.setFirstRenderFrame(Integer.parseInt(firstRenderFrameField.getText()));
        } catch (NumberFormatException | InvalidFrameBoundsExpection ex) {
            firstRenderFrameField.setText(String.format("%d", traceView.getFirstRenderFrame()));
        }
    }

    private void setLastRenderFrame() {
        try {
            traceView.setLastRenderFrame(Integer.parseInt(lastRenderFrameField.getText()));
        } catch (NumberFormatException | InvalidFrameBoundsExpection ex) {
            lastRenderFrameField.setText(String.format("%d", traceView.getLastRenderFrame()));
        }
    }

    private void setFrameSize() {
        try {
            double v = Double.parseDouble(frameSizeField.getText());
            if (v > 0.0) traceView.setFrameSize(v);
        } catch (NumberFormatException ex) {
            frameSizeField.setText(String.format("%8.4f", traceView.getFrameSize()));
        }
    }
}
