package trick.jobperf;

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import javax.swing.*;

/**
* Class TraceViewMenuBar represents the menu bar of the JobPerf application.
* It aggregates the following GUI components:
* JMenuBar [this]
*     JMenu [fileMenu]
*         JMenuItem [fileMenuExit], Action: Call System.exit(0);
*     JMenu [optionsMenu]
*         JMenu [traceSizeMenu]
*             JMenuItem [traceSizeMenuIncrease], Action: Call traceView.incrementTraceWidth().
*             JMenuItem [traceSizeMenuDecrease], Action: Call traceView.decrementTraceWidth()
*/
public class TraceViewMenuBar extends JMenuBar implements ActionListener {

    private TraceViewCanvas traceView;

    /**
     * Constructor
     * @param tvc the TraceViewCanvas to be controlled by this menu bar.
     */
    public TraceViewMenuBar(TraceViewCanvas tvc) {
        traceView = tvc;

        JMenu fileMenu = new JMenu("File");
        JMenuItem fileMenuExit = new JMenuItem("Exit");
        fileMenuExit.setActionCommand("exit");
        fileMenuExit.addActionListener(this);
        fileMenu.add(fileMenuExit);
        add(fileMenu);

        JMenu optionsMenu = new JMenu("Options");
        JMenu traceSizeMenu = new JMenu("TraceSize");
        JMenuItem traceSizeMenuIncrease = new JMenuItem("Increase Trace Width");
        traceSizeMenuIncrease.setActionCommand("increase-trace_width");
        KeyStroke ctrlPlus  = KeyStroke.getKeyStroke('=', InputEvent.CTRL_MASK );
        traceSizeMenuIncrease.setAccelerator(ctrlPlus);
        traceSizeMenuIncrease.addActionListener(this);
        traceSizeMenu.add(traceSizeMenuIncrease);
        JMenuItem traceSizeMenuDecrease = new JMenuItem("Decrease Trace Width");
        traceSizeMenuDecrease.setActionCommand("decrease-trace_width");
        KeyStroke ctrlMinus = KeyStroke.getKeyStroke('-', InputEvent.CTRL_MASK);
        traceSizeMenuDecrease.setAccelerator(ctrlMinus);
        traceSizeMenuDecrease.addActionListener(this);
        traceSizeMenu.add(traceSizeMenuDecrease);
        optionsMenu.add(traceSizeMenu);
        add(optionsMenu);

    }
    @Override
    public void actionPerformed(ActionEvent e) {
        String s = e.getActionCommand();
        switch (s) {
            case "increase-trace_width":
                traceView.incrementTraceWidth();
            break;
            case "decrease-trace_width":
                traceView.decrementTraceWidth();
            break;
            case "exit":
                System.exit(0);
            default:
                System.out.println("Unknown Action Command:" + s);
            break;
        }
    }
}
