package trick.jobperf;
import java.util.*;

/**
* Class CompareByDuration compares two JobExecutionEvent's by their duration.
*/
class CompareByDuration implements Comparator<JobExecutionEvent> {
    public int compare(JobExecutionEvent a, JobExecutionEvent b) {
        Double dur_a = a.stop - a.start;
        Double dur_b = b.stop - b.start;
        if ( dur_a > dur_b) return -1;
        if ( dur_a < dur_b) return  1;
        return 0;
    }
}

/**
* Class FrameRecord represents the set of jobs that have been executed during a
* frame.
*/
public class FrameRecord {
    public ArrayList<JobExecutionEvent> jobEvents;
    public double start;
    public double stop;
    public int maxContainment;
    /**
     * Constructor
     */
    public FrameRecord() {
        start = 0.0;
        stop  = 0.0;
        maxContainment = 0;
        jobEvents = new ArrayList<JobExecutionEvent>();
    }

    /**
     * @return the stop time minus the start time.
     */
    public double getDuration() {
        return stop - start;
    }

    /**
     * For each jobEvent in the frame, record the number of times
     * its start time is contained within
     * another jobs stop/stop range.
     */
    public void CalculateJobContainment() {
        int N = jobEvents.size();
        ArrayList<JobExecutionEvent> tjobs = new ArrayList<JobExecutionEvent>(jobEvents);
        Collections.sort( tjobs, new CompareByDuration());
        for (int i = 0 ; i < (N-1); i++) {
            for (int j = 0 ; j < (N-1); j++) {
                if (i!=j) {
                    JobExecutionEvent ithjob = tjobs.get(i);
                    JobExecutionEvent jthjob = tjobs.get(j);
                    if ( ithjob.contains( jthjob )) {
                        jthjob.contained = ithjob.contained + 1;
                        if (jthjob.contained > maxContainment) {
                            maxContainment = jthjob.contained;
                        }
                    }
                }
            }
        }
    }
}
