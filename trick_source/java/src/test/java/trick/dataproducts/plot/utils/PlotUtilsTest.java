package trick.dataproducts.plot.utils;

import static org.junit.Assert.assertEquals;

import org.junit.Test;

/**
 * Tests {@link PlotUtils}.
 */
public class PlotUtilsTest {

    private static final double DELTA = 1e-12;

    /**
     * Builds a series from {time, value} pairs.
     */
    private static TrickXYSeries series(String key, double[][] points) {
        TrickXYSeries theSeries = new TrickXYSeries(key, false, true);
        for (double[] eachPoint : points) {
            theSeries.add(eachPoint[0], eachPoint[1]);
        }
        return theSeries;
    }

    /**
     * Asserts that a series holds the expected {time, value} pairs.
     */
    private static void assertSeries(double[][] expected, TrickXYSeries actual) {
        assertEquals(expected.length, actual.getItemCount());
        for (int i = 0; i < expected.length; i++) {
            assertEquals(expected[i][0], actual.getX(i).doubleValue(), DELTA);
            assertEquals(expected[i][1], actual.getY(i).doubleValue(), DELTA);
        }
    }

    @Test
    public void deltaOfRunsRecordedAtTheSameRate() {
        TrickXYSeries run1 = series("RUN_1", new double[][] {{0.0, 1.0}, {1.0, 3.0}, {2.0, 6.0}});
        TrickXYSeries run2 = series("RUN_2", new double[][] {{0.0, 1.0}, {1.0, 2.0}, {2.0, 4.0}});

        assertSeries(
                new double[][] {{0.0, 0.0}, {1.0, 1.0}, {2.0, 2.0}}, PlotUtils.getDeltaSeries("DELTA", run1, run2));
    }

    /**
     * The same signal recorded at different rates has no differences at the times both runs share.
     */
    @Test
    public void deltaOfIdenticalSignalsRecordedAtDifferentRates() {
        TrickXYSeries run1 =
                series("RUN_1", new double[][] {{0.0, 0.0}, {1.0, 1.0}, {2.0, 2.0}, {3.0, 3.0}, {4.0, 4.0}});
        TrickXYSeries run2 = series("RUN_2", new double[][] {{0.0, 0.0}, {2.0, 2.0}, {4.0, 4.0}});

        assertSeries(
                new double[][] {{0.0, 0.0}, {2.0, 0.0}, {4.0, 0.0}}, PlotUtils.getDeltaSeries("DELTA", run1, run2));
    }

    /**
     * The same signal recorded from different start times has no differences where the runs overlap.
     */
    @Test
    public void deltaOfIdenticalSignalsRecordedFromDifferentStartTimes() {
        TrickXYSeries run1 = series("RUN_1", new double[][] {{0.0, 0.0}, {1.0, 1.0}, {2.0, 2.0}, {3.0, 3.0}});
        TrickXYSeries run2 = series("RUN_2", new double[][] {{2.0, 2.0}, {3.0, 3.0}, {4.0, 4.0}, {5.0, 5.0}});

        assertSeries(new double[][] {{2.0, 0.0}, {3.0, 0.0}}, PlotUtils.getDeltaSeries("DELTA", run1, run2));
    }

    /**
     * A run that dropped samples is differenced against the samples that remain, not against its neighbors.
     */
    @Test
    public void deltaSkipsTimesThatAreMissingFromOneRun() {
        TrickXYSeries run1 = series("RUN_1", new double[][] {{0.0, 10.0}, {1.0, 20.0}, {2.0, 30.0}, {3.0, 40.0}});
        TrickXYSeries run2 = series("RUN_2", new double[][] {{0.0, 1.0}, {2.0, 3.0}, {3.0, 4.0}});

        assertSeries(
                new double[][] {{0.0, 9.0}, {2.0, 27.0}, {3.0, 36.0}}, PlotUtils.getDeltaSeries("DELTA", run1, run2));
    }

    @Test
    public void deltaOfRunsThatShareNoTimesIsEmpty() {
        TrickXYSeries run1 = series("RUN_1", new double[][] {{0.0, 1.0}, {2.0, 2.0}});
        TrickXYSeries run2 = series("RUN_2", new double[][] {{1.0, 1.0}, {3.0, 2.0}});

        assertEquals(0, PlotUtils.getDeltaSeries("DELTA", run1, run2).getItemCount());
    }

    @Test
    public void deltaOfAnEmptyRunIsEmpty() {
        TrickXYSeries run1 = series("RUN_1", new double[][] {{0.0, 1.0}, {1.0, 2.0}});
        TrickXYSeries run2 = series("RUN_2", new double[][] {});

        assertEquals(0, PlotUtils.getDeltaSeries("DELTA", run1, run2).getItemCount());
    }
}
