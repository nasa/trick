package trick.jobperf;

import java.awt.*;
import java.util.*;
import javax.swing.*;

/**
* Class TraceViewWindow represents the main window of the JobPerf application.
* It aggregates the following GUI components:
*
* - TraceViewMenuBar [menuBar]
* - TraceViewInputToolBar [toolbar]
* - JPanel [mainPanel]
*     - JPanel [tracePanel]
*         - JScrollPane [scrollPane]
*             - TraceViewCanvas [traceView]
* - TraceViewOutputToolBar [outputToolBar]
*/
public class TraceViewWindow extends JFrame {

    /**
     * Constructor
     * @param jobExecList an ArrayList of JobExecutionEvent, i.e., the job timeline data.
     */
    public TraceViewWindow( ArrayList<JobExecutionEvent> jobExecList ) {
        TraceViewOutputToolBar outputToolBar = new TraceViewOutputToolBar();
        TraceViewCanvas traceView = new TraceViewCanvas( jobExecList, outputToolBar);

        TraceViewMenuBar menuBar = new TraceViewMenuBar(traceView);
        setJMenuBar(menuBar);

        TraceViewInputToolBar nToolBar = new TraceViewInputToolBar( traceView );
        add(nToolBar, BorderLayout.NORTH);

        JScrollPane scrollPane = new JScrollPane( traceView );
        scrollPane.setPreferredSize(new Dimension(800, 400));

        JPanel tracePanel = new JPanel();
        tracePanel.setPreferredSize(new Dimension(800, 400));
        tracePanel.add(scrollPane);
        tracePanel.setLayout(new BoxLayout(tracePanel, BoxLayout.X_AXIS));

        JPanel mainPanel  = new JPanel();
        mainPanel.setLayout(new BoxLayout(mainPanel, BoxLayout.Y_AXIS));
        mainPanel.add(tracePanel);

        add(outputToolBar, BorderLayout.SOUTH);

        setTitle("JobPerf");
        setSize(800, 500);
        add(mainPanel);
        pack();
        setVisible(true);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setFocusable(true);
        setVisible(true);

        traceView.repaint();
    }
}
