/*
 * Trick
 * 2018 (c) National Aeronautics and Space Administration (NASA)
 * Programmers: Scott P. Fennell
 */



import javax.swing.*;
import java.awt.*;

class MsdView extends JPanel {

    private int scale;
    private Color massColor;
    private Color backgroundColor;
    private Color wallColor;
    private Color springColor;
    private Double position;
    private Double velocity;
    private Double massRadius;
    boolean hideWall = false;

    // Origin of world coordinates in jpanel coordinates.
    private int worldOriginX;
    private int worldOriginY;


    /**
     * Class constructor.
     */
    public MsdView(int mapScale) {

        setScale(mapScale);

        backgroundColor = new Color(200, 200, 255);
        springColor = new Color(150, 150, 100);
        massColor = new Color(10, 10, 10);
        wallColor = new Color(120, 118, 118);
        springColor = Color.BLUE;
        massRadius = 0.75;
        position = 0.0;
        velocity = 0.0;
    }

    /**
     * @param value angle in degrees
     */
    public void setPosition(double value) {
        position = value;
    }

    public void setVelocity(double value) {
        velocity = value;
    }

    public void setScale(int mapScale) {
        if (mapScale < 4) {
            scale = 4;
        } else if (mapScale > 128) {
            scale = 128;
        } else {
            scale = mapScale;
        }
        repaint();
    }

    public int getScale() {
        return scale;
    }

    public void drawCenteredCircle(Graphics2D g, int x, int y, int r) {
        x = x - (r / 2);
        y = y - (r / 2);
        g.fillOval(x, y, r, r);
    }

    private void doDrawing(Graphics g) {
        Graphics2D g2d = (Graphics2D) g;

        RenderingHints rh = new RenderingHints(
                RenderingHints.KEY_ANTIALIASING,
                RenderingHints.VALUE_ANTIALIAS_ON);

        rh.put(RenderingHints.KEY_RENDERING,
                RenderingHints.VALUE_RENDER_QUALITY);

        int ii, jj;
        int width = getWidth();
        int height = getHeight();


        worldOriginX = (width / 3);
        worldOriginY = (height / 2);

        while (worldOriginX + scale * position > width) {
            scale = scale / 2;
        }

        // Draw Sky
        g2d.setPaint(backgroundColor);
        g2d.fillRect(0, 0, width, worldOriginY);

        //  Draw mass
        g2d.setPaint(massColor);
        int bx = (int) (worldOriginX + scale * position);
        int by = (int) (worldOriginY);
        drawCenteredCircle(g2d, bx, by, (int) (scale * massRadius));

        // Draw spring
        double springPosition;
        // reflect spring visual over origin correctly
        if(position < massRadius/2 && position > -massRadius/2){
           springPosition = 0.0;
        } else if(position < 0){
            springPosition = position + massRadius/2;
        } else{
            springPosition = position - massRadius/2;
        }
        int SCALEFACTOR = 200;
        int cycles = 5;
        int points = SCALEFACTOR * cycles * 2;
        double[] sines = new double[points];
        for (int i = 0; i < points; i++) {
            double radians = (Math.PI / SCALEFACTOR) * i;
            sines[i] = Math.sin(radians);
        }
        int maxWidth = (int) (scale * (springPosition /*- massRadius / 2*/));
        double hstep = (double) maxWidth / (double) points;
        int maxHeight = (int) (scale);
        int[] pts = new int[points];
        for (int i = 0; i < points; i++) {
            pts[i] = (int) (sines[i] * maxHeight / 2 * .95 + maxHeight / 2);
        }
        g2d.setColor(springColor);
        for (int i = 1; i < points; i++) {
            int x1 = (int) ((i - 1) * hstep);
            int x2 = (int) (i * hstep);
            int y1 = pts[i - 1];
            int y2 = pts[i];
            g2d.drawLine(x1 + worldOriginX, y1 + height / 2 - scale / 2, x2 + worldOriginX, y2 + height / 2 - scale / 2);

        }

        //Draw wall
        if (!hideWall && position >= 0) {
            g2d.setPaint(wallColor);
            g2d.fillRect(0, 0, worldOriginX, height);
        } else {
            hideWall = true;
        }


        // Draw range markers.
        int tickRange = 50;
        if (scale >= 8) tickRange = 20;
        if (scale >= 16) tickRange = 10;
        if (scale >= 32) tickRange = 5;
        if (scale >= 64) tickRange = 1;

        int lower = ((int) ((-worldOriginX) / (scale * tickRange)) + 1) * tickRange;
        int upper = ((int) ((width - worldOriginX) / (scale * tickRange)) + 1) * tickRange;

        g2d.setPaint(Color.BLACK);

        for (ii = lower; ii < upper; ii += tickRange) {
            int mx = (int) (worldOriginX + scale * ii);
            g2d.drawLine(mx, worldOriginY, mx, worldOriginY + 20);
            g2d.drawString(String.format("%d", ii), mx, worldOriginY + 15);
        }

        g2d.drawString(String.format("SCALE: %d pixels/meter", scale), 20, 20);
        g2d.drawString(String.format("  Position (m): %.2f", position), 20, 80);
        g2d.drawString(String.format("Velocity (m/s): %.2f", velocity), 20, 120);

    }

    @Override
    public void paintComponent(Graphics g) {
        super.paintComponent(g);
        doDrawing(g);
    }
}