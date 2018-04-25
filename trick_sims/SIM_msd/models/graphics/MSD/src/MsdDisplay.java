/*
 * Trick
 * 2018 (c) National Aeronautics and Space Administration (NASA)
 * Programmers: Scott P. Fennell
 */


import javax.swing.*;


public class MsdDisplay extends JFrame {
    MsdView msdView;



    public void setPosition(double value) {
        msdView.setPosition(value);
    }

    public void setVelocity(double value) {
        msdView.setVelocity(value);
    }

    public void setScale(int value) {
        msdView.setScale(value);
    }

    public int getScale() {
        return msdView.getScale();
    }

    public void drawMsdView() {
        msdView.repaint();
    }



    public MsdDisplay(MsdView arena) {
        setTitle("Mass-Spring-Damper");
        msdView = arena;
        add(msdView);


        setSize(800, 500);
        setLocationByPlatform(true);

        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    }

}
