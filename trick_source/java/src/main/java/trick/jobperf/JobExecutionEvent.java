package trick.jobperf;

import java.awt.*;
import java.awt.event.*;
import java.awt.image.BufferedImage;
import java.io.*;
import java.lang.Math;
import java.util.*;
import java.util.List;
import javax.swing.*;
import javax.swing.event.*;
import java.net.URL;


/**
* Class JobExecutionEvent represents one execution/run of a Trick job.
* <id> identifies the job. <start> and <stop> specify the
* clock times at which the job started and finished.
* <isEOF> and <isTOF> indicate whether the job was run as
* an "end-of-frame", or a "top-of-frame" job.
*/
class JobExecutionEvent {
    public String id;
    public boolean isEOF;
    public boolean isTOF;
    public double start;
    public double stop;

    /**
     * @param identifier identifies the relavant Trick job.
     * @param isTopOfFrame true if the job is a "top-of-frame" job, otherwise false.
     * @param isEndOfFrame true if the job is a "end-of-frame" job, otherwise false.
     * @param start_time the start time (seconds) of the identified job.
     * @param stop_time the stop time (seconds) of the identified job.
     */
    public JobExecutionEvent(String identifier, boolean isTopOfFrame, boolean isEndOfFrame, double start_time, double stop_time) {
        id = identifier;
        isEOF = isEndOfFrame;
        isTOF = isTopOfFrame;
        start = start_time;
        stop = stop_time;
    }
    /**
     * Create a String representation of an object of this class.
     */
     @Override
    public String toString() {
        return ( "JobExecutionEvent: " + id + "," + start + "," + stop );
    }
}
