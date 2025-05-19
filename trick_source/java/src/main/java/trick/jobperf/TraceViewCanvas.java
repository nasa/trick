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
* frameArray. Information regarding mouse clicks are sent to the
* TraceViewOutputToolBar [outputToolBar.]
* @author John M. Penn
*/
public class TraceViewCanvas extends JPanel {

    public static final int MIN_TRACE_WIDTH = 12;
    public static final int DEFAULT_TRACE_WIDTH = 18;
    public static final int MAX_TRACE_WIDTH = 24;
    public static final int LEFT_MARGIN = 150;
    public static final int RIGHT_MARGIN = 100;
    public static final int TOP_MARGIN = 50;
    public static final int BOTTOM_MARGIN = 20;
    public static final int DEFAULT_FRAMES_TO_RENDER = 100;

    public KeyedColorMap idToColorMap;
    public JobSpecificationMap jobSpecificationMap;
    public JobStats jobStats;

    private int traceWidth;
    private double frameSize;
    private double totalDuration;
    private FrameRecord[] frameArray;
    private int selectedFrameNumber;
    private FrameRange frameRenderRange;
    private BufferedImage image;
    private TraceViewOutputToolBar outputToolBar;
    private Cursor crossHairCursor;
    private Cursor defaultCursor;
    private Font frameFont12;
    private Font frameFont18;

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
    public TraceViewCanvas( String filesDir,
                            FrameRecord[] frameArray,
                            JobStats jobStats,
                            TraceViewOutputToolBar outputToolBar,
                            JobSpecificationMap jobSpecificationMap ) {

        traceWidth = DEFAULT_TRACE_WIDTH;
        frameSize = 1.0;
        image = null;
        selectedFrameNumber = 0;

        this.frameArray = frameArray;
        this.jobStats = jobStats;
        this.outputToolBar = outputToolBar;
        this.jobSpecificationMap = jobSpecificationMap;

        // -----------------------
        // Create the IDtoColorMap

        idToColorMap = null;
        File colorMapFile = null;
        try {
            colorMapFile = new File(filesDir + "/IdToColors.txt");
            idToColorMap = new KeyedColorMap( colorMapFile.toString());
            if ( colorMapFile.exists()) {
                 idToColorMap.readFile();
            }
        } catch ( java.io.IOException e ) {
            System.out.println("IO Exception while attempting to read " + colorMapFile.toString() + ".\n");
            System.exit(0);
        }

        // Ensure that all of the Job IDs are in the IDtoColorMap.
        for (int i=0 ; i <frameArray.length ; i++ ) {
            FrameRecord frame = frameArray[i];
            for (JobExecutionEvent jobExec : frame.jobEvents ) {
                idToColorMap.addKey(jobExec.id);
            }
        }

        // Write the IDtoColorMap to a file.
        try {
            idToColorMap.writeFile();
        } catch ( java.io.IOException e ) {
            System.out.println("IO Exception.\n");
            System.exit(0);
        }

        // ------------------
        // Create the Cursors
        crossHairCursor = new Cursor( Cursor.CROSSHAIR_CURSOR );
        defaultCursor = new Cursor( Cursor.DEFAULT_CURSOR );

        // ----------------
        // Create the Fonts
        frameFont12 = new Font("Arial", Font.PLAIN, 12);
        frameFont18 = new Font("Arial", Font.PLAIN, 18);

        // CALC AVERAGE FRAME SIZE
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

    public int getLastRenderFrame() {
        return frameRenderRange.last;
    }

    public void moveRenderFrameRangeBy(int advance) {
        if ( frameArray.length > 50 ) {

            int maxIndex = frameArray.length - 1;
            int tFirst = frameRenderRange.first + advance;
            int tLast = frameRenderRange.last + advance;

            if (tLast > maxIndex) {
                tLast = maxIndex;
                tFirst = maxIndex - 49;
            } else if (tFirst < 0) {
                tFirst = 0;
                tLast = 49;
            }
            frameRenderRange = new FrameRange(tFirst, tLast);
            setPreferredSize(new Dimension(500, neededPanelHeight()));
            repaint();
        }
    }

    public void setFirstRenderFrame(int first) throws InvalidFrameBoundsExpection {
        if ( (first >= 0) && (first <= frameRenderRange.last)) {
            frameRenderRange = new FrameRange(first, frameRenderRange.last);
            setPreferredSize(new Dimension(500, neededPanelHeight()));
            repaint();
        } else {
            throw new InvalidFrameBoundsExpection("");
        }
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
     *
     */
    public void displaySelectedFrame() {
        FrameViewWindow window = new FrameViewWindow( this, frameArray[selectedFrameNumber], selectedFrameNumber);
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
            outputToolBar.setJobID(id);

            // Get and display the job name associated with the ID.
            JobSpecification jobSpec = jobSpecificationMap.getJobSpecification(id);
            if (jobSpec != null) {
                outputToolBar.setJobName(jobSpec.name);
                outputToolBar.setJobClass(jobSpec.jobClass);
            }

            // Determine the frame number that we clicked on from the y-
            // coordinate of the click position.
            if ( y > TOP_MARGIN) {
                selectedFrameNumber = (y - TOP_MARGIN) / traceWidth + frameRenderRange.first;
            }

            // Determine the subframe-time where we clicked from the x-coordinate
            // of the click position.
            double subFrameClickTime = 0.0;
            if ( traceRectContains(x, y)) {
                double pixelsPerSecond = (double)traceRectWidth() / frameSize;
                subFrameClickTime = (x - LEFT_MARGIN) / pixelsPerSecond;
            }

            /**
             * If we clicked on a job trace (above), show the start and stop
             * times of the job, otherwise clear the start and stop fields.
             */
            if (id != null) {
                FrameRecord frame = frameArray[selectedFrameNumber];
                Double clickTime = frame.start + subFrameClickTime;
                for (JobExecutionEvent jobExec : frame.jobEvents) {
                    if (id.equals( jobExec.id) &&
                        clickTime >= jobExec.start &&
                        clickTime <= jobExec.stop ) {
                        outputToolBar.setJobTimes(jobExec.start, jobExec.stop);
                    }
                }
                repaint();
            } else {
                outputToolBar.clearJobFields();
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
        g2d.fillRect(LEFT_MARGIN, TOP_MARGIN, traceRectWidth, traceRectHeight);

        if (traceWidth >= DEFAULT_TRACE_WIDTH) {
            g2d.setFont(frameFont18);
        } else {
            g2d.setFont(frameFont12);
        }

        FontMetrics fm = g2d.getFontMetrics();
        int TX = 0;
        int TY = TOP_MARGIN - 10;
        String FN_text = "Frame# (JobCnt)";
        TX = 10;
        g2d.drawString (FN_text, TX, TY);

        g2d.drawString ("Top of Frame", LEFT_MARGIN, TY);

        String EOF_text = "End of Frame";
        TX = LEFT_MARGIN + traceRectWidth - fm.stringWidth(EOF_text);
        g2d.drawString (EOF_text, TX, TY);

        g2d.setColor(Color.RED);

        String ORZ_L1_text = "OVER-RUN";
        TX = LEFT_MARGIN + traceRectWidth + (RIGHT_MARGIN - fm.stringWidth(ORZ_L1_text))/2;
        TY = TOP_MARGIN + fm.getHeight();
        g2d.drawString(ORZ_L1_text, TX, TY);

        String ORZ_L2_text = "ZONE";
        TX = LEFT_MARGIN + traceRectWidth + (RIGHT_MARGIN - fm.stringWidth(ORZ_L2_text))/2;
        TY = TY + fm.getHeight();
        g2d.drawString(ORZ_L2_text, TX, TY);

        // Draw each frame in the selected range of frames to be rendered.
        for (int n = frameRenderRange.first;
                 n <= frameRenderRange.last;
                 n++) {

            FrameRecord frame = frameArray[n];
            int jobY = TOP_MARGIN + (n - frameRenderRange.first) * traceWidth;

            // Draw frame number.
            if (n == selectedFrameNumber) {
                g2d.setPaint(Color.RED);
                g2d.drawString ( "\u25c0", 80, jobY + traceWidth - 2);
            } else {
                g2d.setPaint(Color.BLACK);
            }

            int jobCount = frame.jobEvents.size();
            g2d.drawString ( String.format("%d  (%d)", n, jobCount), 20, jobY + traceWidth - 2);

            // Draw the frame
            for (int containment = 0; containment <= frame.maxContainment ; containment++) {
                for (JobExecutionEvent jobExec : frame.jobEvents) {
                    int jobStartX = (int)((jobExec.start - frame.start) * pixelsPerSecond) + LEFT_MARGIN;
                    int jobWidth  = (int)((jobExec.stop - jobExec.start) * pixelsPerSecond);

                    g2d.setPaint( idToColorMap.getColor( jobExec.id ) );

                    int jobHeight = traceWidth - 2;
                    if (jobExec.contained == containment) {
                        jobHeight = (traceWidth - 2) / (containment + 1);
                        g2d.fillRect(jobStartX, jobY, jobWidth, jobHeight);
                    }
                }
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
