package trick.jobperf;

import javax.swing.*;

/**
* Class TraceViewOutputToolBar displays information output from a
* TraceViewCanvas. Specifically, this displays the Job ID, frame number, and
* subFrame Time associated with a mouse click position on the TraceViewCanvas.
*/
class TraceViewOutputToolBar extends JToolBar {
    private JTextField IDField;
    private JTextField frameNumberField;
    private JTextField subFrameTimeField;

    /**
     * Constructor
     */
    public TraceViewOutputToolBar () {

        add( new JLabel(" Job ID: "));
        IDField = new JTextField(15);
        IDField.setEditable(false);
        IDField.setText( "");
        add(IDField);

        add( new JLabel(" Frame Number: "));
        frameNumberField = new JTextField(15);
        frameNumberField.setEditable(false);
        frameNumberField.setText( "0");
        add(frameNumberField);

        add( new JLabel(" Subframe Time: "));
        subFrameTimeField = new JTextField(15);
        subFrameTimeField.setEditable(false);
        subFrameTimeField.setText( "0.00");
        add(subFrameTimeField);
    }
    /**
     * @param id job identifier to display.
     */
    public void setJobID(String id) {
        IDField.setText( id );
    }
    /**
     * @param fn frame number to display.
     */
    public void setFrameNumber(int fn) {
        frameNumberField.setText( String.format("%d", fn));
    }
    /**
     * @param time subframe time to display.
     */
    public void setSubFrameTime(double time) {
        subFrameTimeField.setText( String.format("%8.4f", time));
    }
}
