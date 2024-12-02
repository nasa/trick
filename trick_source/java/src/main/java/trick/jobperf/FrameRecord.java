package trick.jobperf;
import java.util.*;
/**
* Class FrameRecord represents the set of jobs that have been executed during a
* frame.
*/
public class FrameRecord {
    public ArrayList<JobExecutionEvent> jobEvents;
    public double start;
    public double stop;
    /**
     * Constructor
     */
    public FrameRecord() {
        start = 0.0;
        stop  = 0.0;
        jobEvents = new ArrayList<JobExecutionEvent>()

        ;
    }
    /**
     * @return the stop time minus the start time.
     */
    public double getDuration() {
        return stop - start;
    }
}
