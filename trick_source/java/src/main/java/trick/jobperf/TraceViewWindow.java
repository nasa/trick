package trick.jobperf;
import java.awt.*;
import java.util.*;
import javax.swing.*;

/**
 * Class TraceViewWindow represents the main window of the JobPerf application.
 *
 * Component hierarchy:
 *   TraceViewWindow (JFrame)
 *     TraceViewMenuBar          [north – menu bar]
 *     TraceViewInputToolBar     [north – tool bar]
 *     JScrollPane
 *       TraceViewCanvas         [centre – main drawing surface]
 *     TraceViewOutputToolBar    [south – status bar]
 */
public class TraceViewWindow extends JFrame {

    public TraceViewWindow(String filesDir,
                           FrameRecord[] frameArray,
                           JobStats jobStats,
                           JobSpecificationMap jobSpecificationMap) {

        TraceViewOutputToolBar outputToolBar = new TraceViewOutputToolBar();
        TraceViewCanvas traceViewCanvas = new TraceViewCanvas(
                filesDir, frameArray, jobStats, outputToolBar, jobSpecificationMap);

        // InputToolBar must exist before MenuBar so the menu bar can reference
        // it for zoom-button synchronisation.
        TraceViewInputToolBar nToolBar = new TraceViewInputToolBar(traceViewCanvas);
        TraceViewMenuBar menuBar = new TraceViewMenuBar(traceViewCanvas, nToolBar);

        setJMenuBar(menuBar);
        add(nToolBar, BorderLayout.NORTH);

        JScrollPane scrollPane = new JScrollPane(traceViewCanvas);
        scrollPane.setPreferredSize(new Dimension(800, 400));
        scrollPane.getVerticalScrollBar().setUnitIncrement(20);

        JPanel tracePanel = new JPanel();
        tracePanel.setPreferredSize(new Dimension(800, 400));
        tracePanel.add(scrollPane);
        tracePanel.setLayout(new BoxLayout(tracePanel, BoxLayout.X_AXIS));

        JPanel mainPanel = new JPanel();
        mainPanel.setLayout(new BoxLayout(mainPanel, BoxLayout.Y_AXIS));
        mainPanel.add(tracePanel);

        add(outputToolBar, BorderLayout.SOUTH);
        add(mainPanel);

        setTitle("JobPerf");
        setSize(800, 500);
        pack();
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        setFocusable(true);
        setVisible(true);
        traceViewCanvas.repaint();
    }
}
