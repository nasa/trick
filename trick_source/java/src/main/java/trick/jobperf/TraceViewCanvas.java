package trick.jobperf;

import java.awt.*;
import java.awt.event.*;
import java.awt.image.BufferedImage;
import java.io.*;
import java.util.*;
import java.util.List;
import javax.swing.*;
import javax.swing.event.*;

/**
* Class TraceViewCanvas renders the simulation timeline data stored in
* an ArrayList of JobExecutionEvent's [jobExecEvtList]. Information regarding
* mouse clicks are sent to the TraceViewOutputToolBar [outputToolBar.]
* @author John M. Penn
*/
public class TraceViewCanvas extends JPanel {

    public static final int MIN_TRACE_WIDTH = 4;
    public static final int DEFAULT_TRACE_WIDTH = 15;
    public static final int MAX_TRACE_WIDTH = 30;
    public static final int LEFT_MARGIN = 100;
    public static final int RIGHT_MARGIN = 100;
    public static final int TOP_MARGIN = 20;
    public static final int BOTTOM_MARGIN = 20;
    public static final int DEFAULT_FRAMES_TO_RENDER = 100;

    private int traceWidth;
    private double frameSize;
    private double totalDuration;
    private FrameRecord[] frameArray;
    private FrameRange frameRenderRange;
    private KeyedColorMap idToColorMap;
    private BufferedImage image;
    private TraceViewOutputToolBar sToolBar;
    private Cursor crossHairCursor;
    private Cursor defaultCursor;

    public class FrameRange {
        public int first;
        public int last;
        FrameRange (int first, int last) {
            this.first = first;
            this.last = last;
        }
        public boolean contains(int n) {
            return ((first <= n) && (n <= last));
        }
        public int size() {
            return last - first + 1;
        }
    }

    /**
     * Constructor
     * @param jobExecEvtList the job time line data.
     * @param outputToolBar the toolbar to which data is to be sent for display.
     */
    public TraceViewCanvas( ArrayList<JobExecutionEvent> jobExecEvtList,
                            TraceViewOutputToolBar outputToolBar ) {

        traceWidth = DEFAULT_TRACE_WIDTH;
        frameSize = 1.0;
        image = null;
        sToolBar = outputToolBar;
        crossHairCursor = new Cursor( Cursor.CROSSHAIR_CURSOR );
        defaultCursor = new Cursor( Cursor.DEFAULT_CURSOR );

        try {
           idToColorMap = new KeyedColorMap();
           File colorfile = new File("IdToColors.txt");
           if (colorfile.exists()) {
                idToColorMap.readFile("IdToColors.txt");
           }
           boolean wasTOF = false;

            List<FrameRecord> frameList = new ArrayList<FrameRecord>();
            FrameRecord frameRecord = new FrameRecord();
            for (JobExecutionEvent jobExec : jobExecEvtList ) {

                 if (!wasTOF && jobExec.isTOF) {
                     // Wrap up the previous frame record.
                     frameRecord.stop = jobExec.start;
                     frameList.add(frameRecord);

                     // Start a new frame record.
                     frameRecord = new FrameRecord();
                     frameRecord.start = jobExec.start;
                 }
                 frameRecord.jobEvents.add(jobExec);

                 wasTOF = jobExec.isTOF;
                 idToColorMap.addKey(jobExec.id);
             }
             frameArray = frameList.toArray( new FrameRecord[ frameList.size() ]);

            // Estimate the total duration and the average frame size. Notice
            // that we skip the first frame.
            totalDuration = 0.0;
            for (int n=1; n < frameArray.length; n++) {
                totalDuration += frameArray[n].getDuration();
            }
            frameSize = totalDuration/(frameArray.length-1);

            // Set the range of frames to be rendered.
            int last_frame_to_render = frameArray.length-1;
            if ( frameArray.length > DEFAULT_FRAMES_TO_RENDER) {
                last_frame_to_render = DEFAULT_FRAMES_TO_RENDER-1;
            }
            frameRenderRange = new FrameRange(0, last_frame_to_render);

            // Write the color file.
            idToColorMap.writeFile("IdToColors.txt");

           System.out.println("File loaded.\n");
        } catch ( java.io.FileNotFoundException e ) {
           System.out.println("File not found.\n");
           System.exit(0);
        } catch ( java.io.IOException e ) {
           System.out.println("IO Exception.\n");
           System.exit(0);
        }

        setPreferredSize(new Dimension(500, neededPanelHeight()));

        ViewListener viewListener = new ViewListener();
         addMouseListener(viewListener);
         addMouseMotionListener(viewListener);
    }

    public int getFrameTotal() {
        return frameArray.length;
    }

    public int getFirstRenderFrame() {
        return frameRenderRange.first;
    }

    public void setFirstRenderFrame(int first) throws InvalidFrameBoundsExpection {
        if ((first >= 0) && (first <= frameRenderRange.last)) {
            frameRenderRange = new FrameRange(first, frameRenderRange.last);
            setPreferredSize(new Dimension(500, neededPanelHeight()));
            repaint();
        } else {
            throw new InvalidFrameBoundsExpection("");
        }
    }

    public int getLastRenderFrame() {
        return frameRenderRange.last;
    }

    public void setLastRenderFrame(int last) throws InvalidFrameBoundsExpection {
        if ((last >= frameRenderRange.first) && (last < frameArray.length)) {
            frameRenderRange = new FrameRange(frameRenderRange.first, last);
            // Re-render this TraceViewCanvas.
            setPreferredSize(new Dimension(500, neededPanelHeight()));
            repaint();
        } else {
            throw new InvalidFrameBoundsExpection("");
        }
    }

    /**
     * @return the current working frame size (seconds) used for rendering.
     * Initially this is estimated from the timeline data, but it can be set to
     * the actual realtime frame size of the user's sim.
     */
    public double getFrameSize() {
        return frameSize;
    }
    /**
     * Set the frame size (seconds) to be used for rendering the timeline data.
     * @param duration the frame size.
     */
    public void setFrameSize(double time) {
        frameSize = time;
        repaint();
    }

    /**
     * Increment the width to be used to render the job traces if the current
     * trace width is less than MAX_TRACE_WIDTH.
     */
    public void incrementTraceWidth() {
        if (traceWidth < MAX_TRACE_WIDTH) {
            traceWidth ++;
            repaint();
        }
    }

    /**
     * Decrement the width to be used to render the job traces if the current
     * trace width is greater than MIN_TRACE_WIDTH.
     */
    public void decrementTraceWidth() {
        if (traceWidth > MIN_TRACE_WIDTH) {
            traceWidth --;
            repaint();
        }
    }

    /**
     * @return true if the trace rectangle contains the point <x,y>, otherwise
     * false.
     */
    private boolean traceRectContains(int x, int y) {
        int traceRectXMax = getWidth() - RIGHT_MARGIN;
        if ( x < (LEFT_MARGIN)) return false;
        if ( x > (traceRectXMax)) return false;
        if (( y < TOP_MARGIN) || (y > (TOP_MARGIN + traceRectHeight()))) return false;
        return true;
    }

    /**
     * Class ViewListener monitors mouse activity within the TraceViewCanvas.
     */
    private class ViewListener extends MouseInputAdapter {

        @Override
        public void mouseReleased(MouseEvent e) {
            int x = e.getX();
            int y = e.getY();
            Color color = new Color ( image.getRGB(x,y) );

            // Get and display the ID of the job associated with the color.
            String id = idToColorMap.getKeyOfColor( color );
            sToolBar.setJobID(id);

            // Determine the frame number that we clicked on from the y-
            // coordinate of the click position.
            int frameNumber = 0;
            if ( y > TOP_MARGIN) {
                frameNumber = (y - TOP_MARGIN) / traceWidth + frameRenderRange.first;
                sToolBar.setFrameNumber(frameNumber);
            }

            // Determine the subframe-time where we clicked from the x-coordinate
            // of the click position.
            if ( traceRectContains(x, y)) {
                double pixelsPerSecond = (double)traceRectWidth() / frameSize;
                double subFrameTime = (x - LEFT_MARGIN) / pixelsPerSecond;
                sToolBar.setSubFrameTime(subFrameTime);
            }

            /**
             * If we clicked on a job trace (above), show the start and stop
             * times of the job, otherwise clear the start and stop fields.
             */
            if (id != null) {
                FrameRecord frame = frameArray[frameNumber];
                for (JobExecutionEvent jobExec : frame.jobEvents) {
                    if (id.equals( jobExec.id)) {
                        sToolBar.setJobStartTime(jobExec.start);
                        sToolBar.setJobStopTime(jobExec.stop);
                    }
                }
            } else {
                sToolBar.clearJobStartStopTime();
            }
        }

        /**
         * Set the cursor to a crossHairCursor if it's over the frame traces,
         * otherwise, set it to the defaultCursor.
         */
        @Override
        public void mouseMoved(MouseEvent e) {
            int x = e.getX();
            int y = e.getY();
            if ( traceRectContains(x, y)) {
                setCursor(crossHairCursor);
            } else {
                setCursor(defaultCursor);
            }
        }
    }

    /**
     * @return the height of the trace rectangle.
     */
    private int traceRectHeight() {
        return traceWidth * frameRenderRange.size();
    }

    /**
     * @return the width of the trace rectangle.
     */
    private int traceRectWidth() {
        return ( getWidth() - LEFT_MARGIN - RIGHT_MARGIN);
    }

    /**
     * Calculate the height of the TraceViewCanvas (JPanel) needed to render the
     * selected range of frames.
     */
    private int neededPanelHeight() {
        return traceWidth * frameRenderRange.size() + TOP_MARGIN + BOTTOM_MARGIN;
    }

    /**
     * Render the job execution traces in the jobExecEvtList.
     */
    private void doDrawing(Graphics g) {
        Graphics2D g2d = (Graphics2D) g;

        RenderingHints rh = new RenderingHints(
                RenderingHints.KEY_ANTIALIASING,
                RenderingHints.VALUE_ANTIALIAS_ON);

        rh.put(RenderingHints.KEY_RENDERING,
               RenderingHints.VALUE_RENDER_QUALITY);

        int traceRectHeight = traceRectHeight();
        int traceRectWidth = traceRectWidth();
        double pixelsPerSecond = (double)traceRectWidth / frameSize;

        // Panel Background Color Fill
        g2d.setPaint(Color.WHITE);
        g2d.fillRect(0, 0, getWidth(), getHeight());

        // Frame Trace Rectangle Fill
        g2d.setPaint(Color.BLACK);
        g2d.fillRect(LEFT_MARGIN, TOP_MARGIN, traceRectWidth, traceRectHeight());

        // Draw each frame in the selected range of frames to be rendered.
        for (int n = frameRenderRange.first;
                 n <= frameRenderRange.last;
                 n++) {

            FrameRecord frame = frameArray[n];

            // Draw the frame
            for (JobExecutionEvent jobExec : frame.jobEvents) {
                int jobY = TOP_MARGIN + (n - frameRenderRange.first) * traceWidth;
                int jobStartX = LEFT_MARGIN + (int)((jobExec.start - frame.start) * pixelsPerSecond);
                int jobWidth  = (int)( (jobExec.stop - jobExec.start) * pixelsPerSecond);

                g2d.setPaint(Color.BLACK);
                g2d.drawString ( String.format("%d", n), 50, jobY + traceWidth/2);
                g2d.setPaint( idToColorMap.getColor( jobExec.id ) );
                g2d.fillRect(jobStartX, jobY, jobWidth, traceWidth-2);
            }
        }
    }

    /**
     * This function paints the TraceViewCanvas (i.e, JPanel) when required.
     */
    @Override
    public void paintComponent(Graphics g) {
        super.paintComponent(g);
        image = new BufferedImage(getWidth(), getHeight(), BufferedImage.TYPE_INT_RGB);
        Graphics2D g2 = image.createGraphics();
        doDrawing(g2);
        g.drawImage(image, 0, 0, this);
        g2.dispose();
    }
}
