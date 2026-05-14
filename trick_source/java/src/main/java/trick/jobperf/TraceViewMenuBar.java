package trick.jobperf;
import java.awt.*;
import java.awt.event.*;
import java.io.*;
import javax.swing.*;

/**
 * Class TraceViewMenuBar represents the menu bar of the JobPerf application.
 *
 * View menu zoom items:
 *   Ctrl+Z       — Zoom Out (one level, same as right-click on canvas)
 *   Ctrl+Shift+Z — Zoom Reset (back to original view)
 */
public class TraceViewMenuBar extends JMenuBar implements ActionListener {
    private TraceViewCanvas traceView;
    private TraceViewInputToolBar inputToolBar;

    public TraceViewMenuBar(TraceViewCanvas tvc, TraceViewInputToolBar tb) {
        traceView    = tvc;
        inputToolBar = tb;

        // File menu
        JMenu fileMenu = new JMenu("File");
        JMenuItem fileMenuExit = new JMenuItem("Exit");
        fileMenuExit.setActionCommand("exit");
        fileMenuExit.setAccelerator(KeyStroke.getKeyStroke('Q', InputEvent.CTRL_MASK));
        fileMenuExit.addActionListener(this);
        fileMenu.add(fileMenuExit);
        add(fileMenu);

        // View menu
        JMenu viewMenu = new JMenu("View");

        JMenuItem traceSizeIncrease = new JMenuItem("Increase Trace Width");
        traceSizeIncrease.setActionCommand("increase-trace_width");
        traceSizeIncrease.setAccelerator(KeyStroke.getKeyStroke('=', InputEvent.CTRL_MASK));
        traceSizeIncrease.addActionListener(this);
        viewMenu.add(traceSizeIncrease);

        JMenuItem traceSizeDecrease = new JMenuItem("Decrease Trace Width");
        traceSizeDecrease.setActionCommand("decrease-trace_width");
        traceSizeDecrease.setAccelerator(KeyStroke.getKeyStroke('-', InputEvent.CTRL_MASK));
        traceSizeDecrease.addActionListener(this);
        viewMenu.add(traceSizeDecrease);

        viewMenu.addSeparator();

        JMenuItem showFrame = new JMenuItem("Frame Details ...");
        showFrame.setActionCommand("expand-selected-frame");
        showFrame.setAccelerator(KeyStroke.getKeyStroke('F', InputEvent.CTRL_MASK));
        showFrame.addActionListener(this);
        viewMenu.add(showFrame);

        JMenuItem showStats = new JMenuItem("Job Statistics ...");
        showStats.setActionCommand("show-job-stats");
        showStats.setAccelerator(KeyStroke.getKeyStroke('V', InputEvent.CTRL_MASK));
        showStats.addActionListener(this);
        viewMenu.add(showStats);

        viewMenu.addSeparator();

        JMenuItem zoomOut = new JMenuItem("Zoom Out  (one level)");
        zoomOut.setActionCommand("zoom-out");
        zoomOut.setAccelerator(KeyStroke.getKeyStroke('Z', InputEvent.CTRL_MASK));
        zoomOut.addActionListener(this);
        viewMenu.add(zoomOut);

        JMenuItem zoomReset = new JMenuItem("Zoom Reset  (all levels)");
        zoomReset.setActionCommand("zoom-reset");
        zoomReset.setAccelerator(KeyStroke.getKeyStroke('Z', InputEvent.CTRL_MASK | InputEvent.SHIFT_MASK));
        zoomReset.addActionListener(this);
        viewMenu.add(zoomReset);

        add(viewMenu);
    }

    @Override
    public void actionPerformed(ActionEvent e) {
        switch (e.getActionCommand()) {
            case "increase-trace_width":  traceView.incrementTraceWidth(); break;
            case "decrease-trace_width":  traceView.decrementTraceWidth(); break;
            case "expand-selected-frame": traceView.displaySelectedFrame(); break;
            case "show-job-stats":
                traceView.jobStats.SortByID();
                traceView.jobStats.displayJobStatsWindow();
                break;
            case "zoom-out":
                traceView.zoomOut();
                inputToolBar.updateZoomButtons();
                break;
            case "zoom-reset":
                traceView.zoomReset();
                inputToolBar.updateZoomButtons();
                break;
            case "exit": System.exit(0); break;
            default: System.out.println("Unknown Action Command: " + e.getActionCommand());
        }
    }
}
