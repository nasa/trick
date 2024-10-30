package trick.jobperf;

/**
* Class StatisticsRecord represents the statistics, i.e., mean, std deviation,
* max value, and min value of the run-duration of an identified Trick job.
*/
public class StatisticsRecord {
    public String id;
    public double meanValue;
    public double stddev;
    public double maxValue;
    public double minValue;
    /**
     * Constructor
     * @param s - the job identifier.
     * @param mean - the mean value of job duration.
     * @param sd - the standard deviation of job duration.
     * @param min - the minimum value of job duration.
     * @param max - the maximum value of job duration.
     */
    public StatisticsRecord( String s, double mean, double sd, double min, double max) {
        id = s;
        meanValue = mean;
        stddev = sd;
        minValue = min;
        maxValue = max;
    }
}
