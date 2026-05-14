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
 * TraceViewOutputToolBar [outputToolBar].
 *
 * Zoom behaviour:
 *   - Click and drag draws a rubber-band selection box.
 *   - Releasing the mouse zooms the view into the selected box
 *     (vertical = frame rows, horizontal = time window).
 *   - Each zoom pushes the previous view onto a stack.
 *   - Right-click pops one zoom level off the stack.
 *   - Repeated rubber-band drags zoom in further on the already-zoomed view.
 *
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

    // Minimum drag distance (pixels) before treating a drag as a zoom box.
    private static final int MIN_DRAG_PIXELS = 5;

    public KeyedColorMap idToColorMap;
    public JobSpecificationMap jobSpecificationMap;
    public JobStats jobStats;

    private int traceWidth;
    private FrameRecord[] frameArray;
    private int selectedFrameNumber;
    private BufferedImage image;
    private TraceViewOutputToolBar outputToolBar;
    private Cursor crossHairCursor;
    private Cursor defaultCursor;
    private Font frameFont12;
    private Font frameFont18;

    // -----------------------------------------------------------------------
    // View state
    //   frameSize  = seconds visible across the full trace-rect width.
    //   viewStart  = seconds offset from frame.start shown at the left edge.
    // -----------------------------------------------------------------------
    private double frameSize;
    private double viewStart;
    private double totalDuration;
    private FrameRange frameRenderRange;

    // -----------------------------------------------------------------------
    // Zoom stack — each entry is the complete view state before a zoom-in.
    // -----------------------------------------------------------------------
    private static class ViewState {
        final double frameSize;
        final double viewStart;
        final int rangeFirst;
        final int rangeLast;
        ViewState(double fs, double vs, int rf, int rl) {
            frameSize  = fs;  viewStart  = vs;
            rangeFirst = rf;  rangeLast  = rl;
        }
    }
    private final Deque<ViewState> zoomStack = new ArrayDeque<>();

    // -----------------------------------------------------------------------
    // Rubber-band drag state
    // -----------------------------------------------------------------------
    private boolean dragging  = false;
    private int dragStartX, dragStartY;
    private int dragCurrentX, dragCurrentY;

    // -----------------------------------------------------------------------
    // Inner class: FrameRange
    // -----------------------------------------------------------------------
    public class FrameRange {
        public int first, last;
        FrameRange(int first, int last) { this.first = first; this.last = last; }
        public boolean contains(int n)  { return (first <= n) && (n <= last); }
        public int size()               { return last - first + 1; }
    }

    // -----------------------------------------------------------------------
    // Constructor
    // -----------------------------------------------------------------------
    public TraceViewCanvas(String filesDir,
                           FrameRecord[] frameArray,
                           JobStats jobStats,
                           TraceViewOutputToolBar outputToolBar,
                           JobSpecificationMap jobSpecificationMap) {
        traceWidth = DEFAULT_TRACE_WIDTH;
        frameSize  = 1.0;
        viewStart  = 0.0;
        image      = null;
        selectedFrameNumber = 0;
        this.frameArray          = frameArray;
        this.jobStats            = jobStats;
        this.outputToolBar       = outputToolBar;
        this.jobSpecificationMap = jobSpecificationMap;

        // Create the IDtoColorMap
        File colorMapFile = null;
        try {
            colorMapFile = new File(filesDir + "/IdToColors.txt");
            idToColorMap = new KeyedColorMap(colorMapFile.toString());
            if (colorMapFile.exists()) {
                idToColorMap.readFile();
            }
        } catch (java.io.IOException e) {
            System.out.println("IO Exception while attempting to read " + colorMapFile + ".\n");
            System.exit(0);
        }

        for (FrameRecord frame : frameArray) {
            for (JobExecutionEvent jobExec : frame.jobEvents) {
                idToColorMap.addKey(jobExec.id);
            }
        }

        try {
            idToColorMap.writeFile();
        } catch (java.io.IOException e) {
            System.out.println("IO Exception.\n");
            System.exit(0);
        }

        crossHairCursor = new Cursor(Cursor.CROSSHAIR_CURSOR);
        defaultCursor   = new Cursor(Cursor.DEFAULT_CURSOR);
        frameFont12     = new Font("Arial", Font.PLAIN, 12);
        frameFont18     = new Font("Arial", Font.PLAIN, 18);

        totalDuration = 0.0;
        for (int n = 1; n < frameArray.length; n++) {
            totalDuration += frameArray[n].getDuration();
        }
        frameSize = totalDuration / (frameArray.length - 1);

        int last_frame_to_render = Math.min(frameArray.length - 1, DEFAULT_FRAMES_TO_RENDER - 1);
        frameRenderRange = new FrameRange(0, last_frame_to_render);

        setPreferredSize(new Dimension(500, neededPanelHeight()));
        ViewListener vl = new ViewListener();
        addMouseListener(vl);
        addMouseMotionListener(vl);
    }

    // -----------------------------------------------------------------------
    // Public accessors
    // -----------------------------------------------------------------------
    public int     getFrameTotal()       { return frameArray.length; }
    public int     getFirstRenderFrame() { return frameRenderRange.first; }
    public int     getLastRenderFrame()  { return frameRenderRange.last; }
    public double  getFrameSize()        { return frameSize; }
    public boolean isZoomed()            { return !zoomStack.isEmpty(); }

    // -----------------------------------------------------------------------
    // Zoom: push
    // -----------------------------------------------------------------------
    /**
     * Zoom into the rectangle defined by two canvas pixel coordinates.
     * Coordinates are automatically normalised and clamped to the trace rect.
     */
    private void pushZoom(int x1, int y1, int x2, int y2) {
        int xMin = Math.max(Math.min(x1, x2), LEFT_MARGIN);
        int xMax = Math.min(Math.max(x1, x2), getWidth() - RIGHT_MARGIN);
        int yMin = Math.max(Math.min(y1, y2), TOP_MARGIN);
        int yMax = Math.min(Math.max(y1, y2), TOP_MARGIN + traceRectHeight());

        if (xMax <= xMin || yMax <= yMin) return;

        double pixelsPerSecond = (double) traceRectWidth() / frameSize;

        // New time window (offset from frame.start)
        double newViewStart = viewStart + (xMin - LEFT_MARGIN) / pixelsPerSecond;
        double newFrameSize = (xMax - xMin) / pixelsPerSecond;
        if (newFrameSize <= 0) return;

        // New frame row range
        int newFirst = (yMin - TOP_MARGIN) / traceWidth + frameRenderRange.first;
        int newLast  = (yMax - TOP_MARGIN - 1) / traceWidth + frameRenderRange.first;
        newFirst = Math.max(newFirst, frameRenderRange.first);
        newLast  = Math.min(newLast,  frameRenderRange.last);
        if (newFirst > newLast) newLast = newFirst;

        // Push current state then apply new state
        zoomStack.push(new ViewState(frameSize, viewStart,
                                     frameRenderRange.first, frameRenderRange.last));
        frameSize        = newFrameSize;
        viewStart        = newViewStart;
        frameRenderRange = new FrameRange(newFirst, newLast);

        setPreferredSize(new Dimension(500, neededPanelHeight()));
        repaint();
    }

    // -----------------------------------------------------------------------
    // Zoom: pop (right-click)
    // -----------------------------------------------------------------------
    public void zoomOut() {
        if (zoomStack.isEmpty()) return;
        ViewState vs = zoomStack.pop();
        frameSize        = vs.frameSize;
        viewStart        = vs.viewStart;
        frameRenderRange = new FrameRange(vs.rangeFirst, vs.rangeLast);
        setPreferredSize(new Dimension(500, neededPanelHeight()));
        repaint();
    }

    // -----------------------------------------------------------------------
    // Zoom: reset all the way out
    // -----------------------------------------------------------------------
    public void zoomReset() {
        if (zoomStack.isEmpty()) return;
        ViewState vs = null;
        while (!zoomStack.isEmpty()) vs = zoomStack.pop();
        frameSize        = vs.frameSize;
        viewStart        = vs.viewStart;
        frameRenderRange = new FrameRange(vs.rangeFirst, vs.rangeLast);
        setPreferredSize(new Dimension(500, neededPanelHeight()));
        repaint();
    }

    // -----------------------------------------------------------------------
    // Existing controls (unchanged API)
    // -----------------------------------------------------------------------
    public void moveRenderFrameRangeBy(int advance) {
        if (frameArray.length > 50) {
            int maxIndex = frameArray.length - 1;
            int tFirst = frameRenderRange.first + advance;
            int tLast  = frameRenderRange.last  + advance;
            if (tLast > maxIndex)  { tLast = maxIndex; tFirst = maxIndex - 49; }
            else if (tFirst < 0)   { tFirst = 0;       tLast  = 49; }
            frameRenderRange = new FrameRange(tFirst, tLast);
            setPreferredSize(new Dimension(500, neededPanelHeight()));
            repaint();
        }
    }

    public void setFirstRenderFrame(int first) throws InvalidFrameBoundsExpection {
        if ((first >= 0) && (first <= frameRenderRange.last)) {
            frameRenderRange = new FrameRange(first, frameRenderRange.last);
            setPreferredSize(new Dimension(500, neededPanelHeight()));
            repaint();
        } else { throw new InvalidFrameBoundsExpection(""); }
    }

    public void setLastRenderFrame(int last) throws InvalidFrameBoundsExpection {
        if ((last >= frameRenderRange.first) && (last < frameArray.length)) {
            frameRenderRange = new FrameRange(frameRenderRange.first, last);
            setPreferredSize(new Dimension(500, neededPanelHeight()));
            repaint();
        } else { throw new InvalidFrameBoundsExpection(""); }
    }

    public void setFrameSize(double time)    { frameSize = time; repaint(); }
    public void incrementTraceWidth() { if (traceWidth < MAX_TRACE_WIDTH) { traceWidth++; repaint(); } }
    public void decrementTraceWidth() { if (traceWidth > MIN_TRACE_WIDTH) { traceWidth--; repaint(); } }

    public void displaySelectedFrame() {
        new FrameViewWindow(this, frameArray[selectedFrameNumber], selectedFrameNumber);
    }

    // -----------------------------------------------------------------------
    // Geometry helpers
    // -----------------------------------------------------------------------
    private boolean traceRectContains(int x, int y) {
        return x >= LEFT_MARGIN
            && x <= getWidth() - RIGHT_MARGIN
            && y >= TOP_MARGIN
            && y <= TOP_MARGIN + traceRectHeight();
    }
    private int traceRectHeight() { return traceWidth * frameRenderRange.size(); }
    private int traceRectWidth()  { return getWidth() - LEFT_MARGIN - RIGHT_MARGIN; }
    private int neededPanelHeight(){ return traceWidth * frameRenderRange.size() + TOP_MARGIN + BOTTOM_MARGIN; }

    // -----------------------------------------------------------------------
    // Mouse listener
    // -----------------------------------------------------------------------
    private class ViewListener extends MouseInputAdapter {

        @Override
        public void mousePressed(MouseEvent e) {
            if (SwingUtilities.isLeftMouseButton(e)) {
                dragStartX   = e.getX();
                dragStartY   = e.getY();
                dragCurrentX = dragStartX;
                dragCurrentY = dragStartY;
                dragging     = true;
            }
        }

        @Override
        public void mouseDragged(MouseEvent e) {
            if (dragging) {
                dragCurrentX = e.getX();
                dragCurrentY = e.getY();
                repaint();
            }
        }

        @Override
        public void mouseReleased(MouseEvent e) {

            // Right-click: zoom out one level
            if (SwingUtilities.isRightMouseButton(e)) {
                zoomOut();
                return;
            }

            if (!dragging) return;
            dragging = false;

            int x  = e.getX();
            int y  = e.getY();
            int dx = Math.abs(x - dragStartX);
            int dy = Math.abs(y - dragStartY);

            if (dx >= MIN_DRAG_PIXELS || dy >= MIN_DRAG_PIXELS) {
                // ---- Rubber-band zoom ----
                pushZoom(dragStartX, dragStartY, x, y);

            } else {
                // ---- Single click: identify job under cursor ----
                if (image == null) return;
                Color color = new Color(image.getRGB(x, y));
                String id   = idToColorMap.getKeyOfColor(color);
                outputToolBar.setJobID(id);
                JobSpecification jobSpec = jobSpecificationMap.getJobSpecification(id);
                if (jobSpec != null) {
                    outputToolBar.setJobName(jobSpec.name);
                    outputToolBar.setJobClass(jobSpec.jobClass);
                }
                if (y > TOP_MARGIN) {
                    selectedFrameNumber = (y - TOP_MARGIN) / traceWidth + frameRenderRange.first;
                }
                if (id != null && traceRectContains(x, y)) {
                    double pixelsPerSecond = (double) traceRectWidth() / frameSize;
                    double clickTime = frameArray[selectedFrameNumber].start
                                     + viewStart
                                     + (x - LEFT_MARGIN) / pixelsPerSecond;
                    FrameRecord frame = frameArray[selectedFrameNumber];
                    for (JobExecutionEvent jobExec : frame.jobEvents) {
                        if (id.equals(jobExec.id)
                                && clickTime >= jobExec.start
                                && clickTime <= jobExec.stop) {
                            outputToolBar.setJobTimes(jobExec.start, jobExec.stop);
                        }
                    }
                } else if (id == null) {
                    outputToolBar.clearJobFields();
                }
                repaint();
            }
        }

        @Override
        public void mouseMoved(MouseEvent e) {
            setCursor(traceRectContains(e.getX(), e.getY()) ? crossHairCursor : defaultCursor);
        }
    }

    // -----------------------------------------------------------------------
    // Drawing
    // -----------------------------------------------------------------------
    private void doDrawing(Graphics g) {
        Graphics2D g2d = (Graphics2D) g;
        g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
        g2d.setRenderingHint(RenderingHints.KEY_RENDERING,    RenderingHints.VALUE_RENDER_QUALITY);

        int    traceRectWidth   = traceRectWidth();
        int    traceRectHeight  = traceRectHeight();
        double pixelsPerSecond  = (double) traceRectWidth / frameSize;

        // Background
        g2d.setPaint(Color.WHITE);
        g2d.fillRect(0, 0, getWidth(), getHeight());

        // Black trace rectangle
        g2d.setPaint(Color.BLACK);
        g2d.fillRect(LEFT_MARGIN, TOP_MARGIN, traceRectWidth, traceRectHeight);

        g2d.setFont(traceWidth >= DEFAULT_TRACE_WIDTH ? frameFont18 : frameFont12);
        FontMetrics fm = g2d.getFontMetrics();

        // Column headings
        int TY = TOP_MARGIN - 10;
        g2d.setPaint(Color.BLACK);
        g2d.drawString("Frame# (JobCnt)", 10, TY);
        g2d.drawString("Top of Frame", LEFT_MARGIN, TY);
        String EOF_text = "End of Frame";
        g2d.drawString(EOF_text, LEFT_MARGIN + traceRectWidth - fm.stringWidth(EOF_text), TY);

        g2d.setColor(Color.RED);
        String ORZ1 = "OVER-RUN", ORZ2 = "ZONE";
        int orzX = LEFT_MARGIN + traceRectWidth + (RIGHT_MARGIN - fm.stringWidth(ORZ1)) / 2;
        g2d.drawString(ORZ1, orzX, TOP_MARGIN + fm.getHeight());
        g2d.drawString(ORZ2, LEFT_MARGIN + traceRectWidth + (RIGHT_MARGIN - fm.stringWidth(ORZ2)) / 2,
                       TOP_MARGIN + 2 * fm.getHeight());

        // Zoom depth indicator
        int depth = zoomStack.size();
        if (depth > 0) {
            g2d.setFont(frameFont18);
            g2d.setColor(new Color(0, 130, 0));
            g2d.drawString(String.format(
                "ZOOM x%d  [frames %d\u2013%d | t+%.6f \u2013 %.6f s]   right-click to zoom out",
                depth,
                frameRenderRange.first, frameRenderRange.last,
                viewStart, viewStart + frameSize),
                LEFT_MARGIN, TOP_MARGIN - 28);
        }

        // Draw each frame row
        g2d.setFont(traceWidth >= DEFAULT_TRACE_WIDTH ? frameFont18 : frameFont12);
        for (int n = frameRenderRange.first; n <= frameRenderRange.last; n++) {
            FrameRecord frame = frameArray[n];
            int rowY = TOP_MARGIN + (n - frameRenderRange.first) * traceWidth;

            // Frame label
            g2d.setPaint(n == selectedFrameNumber ? Color.RED : Color.BLACK);
            if (n == selectedFrameNumber)
                g2d.drawString("\u25c0", 80, rowY + traceWidth - 2);
            g2d.drawString(String.format("%d  (%d)", n, frame.jobEvents.size()),
                           20, rowY + traceWidth - 2);

            // Job bars
            for (int containment = 0; containment <= frame.maxContainment; containment++) {
                for (JobExecutionEvent jobExec : frame.jobEvents) {
                    if (jobExec.contained != containment) continue;

                    // Pixel position accounting for viewStart offset
                    double offsetStart = (jobExec.start - frame.start) - viewStart;
                    int jobStartX = (int)(offsetStart * pixelsPerSecond) + LEFT_MARGIN;
                    int jobWidth  = Math.max(1, (int)((jobExec.stop - jobExec.start) * pixelsPerSecond));

                    // Cull bars entirely outside the visible window
                    if (jobStartX + jobWidth < LEFT_MARGIN) continue;
                    if (jobStartX > LEFT_MARGIN + traceRectWidth) continue;

                    int jobHeight = (traceWidth - 2) / (containment + 1);
                    g2d.setPaint(idToColorMap.getColor(jobExec.id));
                    g2d.fillRect(jobStartX, rowY, jobWidth, jobHeight);
                }
            }
        }
    }

    /**
     * Draw the rubber-band selection rectangle directly onto the component
     * graphics so it does not pollute the pixel-pick image used for job ID lookup.
     */
    private void drawRubberBand(Graphics g) {
        if (!dragging) return;
        int x = Math.min(dragStartX, dragCurrentX);
        int y = Math.min(dragStartY, dragCurrentY);
        int w = Math.abs(dragCurrentX - dragStartX);
        int h = Math.abs(dragCurrentY - dragStartY);
        if (w < 1 || h < 1) return;

        Graphics2D g2d = (Graphics2D) g;
        g2d.setColor(new Color(100, 180, 255, 60));
        g2d.fillRect(x, y, w, h);
        g2d.setColor(new Color(0, 100, 220));
        Stroke old = g2d.getStroke();
        g2d.setStroke(new BasicStroke(1.5f));
        g2d.drawRect(x, y, w, h);
        g2d.setStroke(old);
    }

    @Override
    public void paintComponent(Graphics g) {
        super.paintComponent(g);
        // Render frame data into the off-screen image (used for colour-pick hit testing)
        image = new BufferedImage(getWidth(), getHeight(), BufferedImage.TYPE_INT_RGB);
        Graphics2D g2 = image.createGraphics();
        doDrawing(g2);
        g2.dispose();
        g.drawImage(image, 0, 0, this);
        // Rubber-band overlay goes on top, outside the image, so it never
        // affects job-ID colour lookups.
        drawRubberBand(g);
    }
}
