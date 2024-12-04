package trick.jobperf;

import javax.swing.*;

/**
* Class TraceViewOutputToolBar displays information output from a
* TraceViewCanvas. Specifically, this displays the Job ID, frame number, and
* subFrame Time associated with a mouse click position on the TraceViewCanvas.
*/
class TraceViewOutputToolBar extends JToolBar {
    private JTextField IDField;
    private JTextField startField;
    private JTextField stopField;
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

        add( new JLabel(" Job Start: "));
        startField = new JTextField(15);
        startField.setEditable(false);
        startField.setText( "");
        add(startField);

        add( new JLabel(" Job Stop: "));
        stopField = new JTextField(15);
        stopField.setEditable(false);
        stopField.setText( "");
        add(stopField);

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
     * @param time to be displayed in the job start field.
     */
    public void setJobStartTime(Double time) {
        startField.setText( String.format("%8.4f", time) );
    }
    /**
     * @param time to be displayed in the job stop field.
     */
    public void setJobStopTime(Double time) {
        stopField.setText( String.format("%8.4f", time) );
    }
    /**
     * Clear the startField and stopField.
     */
    public void clearJobStartStopTime() {
        startField.setText("");
        stopField.setText("");
        IDField.setText("");
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
