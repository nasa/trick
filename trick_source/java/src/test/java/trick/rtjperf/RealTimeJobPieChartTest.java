package trick.rtjperf;

import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.Map;
import java.util.Set;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.SwingUtilities;
import javax.swing.Timer;
import org.junit.Before;
import org.junit.Test;
import static org.junit.Assert.*;

public class RealTimeJobPieChartTest {
    private RealTimeJobPieChart chart;

    @Before
    public void createChart() throws Exception {
        SwingUtilities.invokeAndWait(() -> {
            chart = new RealTimeJobPieChart();
            try {
                ((Timer) field("refreshTimer")).stop();
            } catch (Exception exception) {
                throw new AssertionError(exception);
            }
        });
    }

    private Object field(String name) throws Exception {
        Field field = RealTimeJobPieChart.class.getDeclaredField(name);
        field.setAccessible(true);
        return field.get(chart);
    }

    private void flush() throws Exception {
        SwingUtilities.invokeAndWait(() -> {});
    }

    private void render() throws Exception {
        SwingUtilities.invokeAndWait(() -> {
            try {
                Method method = RealTimeJobPieChart.class.getDeclaredMethod("renderLatestFrame");
                method.setAccessible(true);
                method.invoke(chart);
            } catch (Exception exception) {
                throw new AssertionError(exception);
            }
        });
    }

    private double total(String job) throws Exception {
        Object rolling = ((Map<?, ?>) field("jobTotals")).get(job);
        if (rolling == null) {
            return 0.0;
        }
        Method method = rolling.getClass().getDeclaredMethod("getTotal");
        method.setAccessible(true);
        return (Double) method.invoke(rolling);
    }

    private String label(String name) throws Exception {
        return ((JLabel) field(name)).getText();
    }

    private void renderHealth(long now) throws Exception {
        SwingUtilities.invokeAndWait(() -> {
            try {
                Method method = RealTimeJobPieChart.class.getDeclaredMethod("renderRealtimeHealth", long.class);
                method.setAccessible(true);
                method.invoke(chart, now);
            } catch (Exception exception) {
                throw new AssertionError(exception);
            }
        });
    }

    @Test
    public void recordedTotalIncludesJobsHiddenByThreshold() throws Exception {
        chart.updateFrameData(1.0, "Run", Arrays.asList(
                new RealTimeJobPieChart.JobDuration("big", 10000.0),
                new RealTimeJobPieChart.JobDuration("hidden", 1.0)));
        flush();
        render();
        assertEquals(10001.0, (Double) field("recordedJobTime"), 0.0);
        assertEquals(1, ((List<?>) field("currentFrameData")).size());
        chart.updateFrameData(2.0, "Run", Collections.emptyList());
        flush();
        render();
        assertEquals(0.0, (Double) field("recordedJobTime"), 0.0);
    }

    @Test
    public void healthDisplaysSignedHeadroomCountersAndPeak() throws Exception {
        assertTrue(label("telemetryLabel").contains("No samples"));
        chart.updateRealtimeHealth(true, 0.02, -0.003, 17, 2, 0.008, 42);
        flush();
        assertTrue(label("realtimeHealthLabel").contains("active"));
        assertTrue(label("realtimeHealthLabel").contains("0.020000 s"));
        assertTrue(label("realtimeHealthLabel").contains("17 total / 2 consecutive"));
        assertTrue(label("deadlineLabel").contains("-0.003000 s (headroom)"));
        assertTrue(label("deadlineLabel").contains("0.008000 s"));
        chart.updateRealtimeHealth(true, 0.02, 0.004, 18, 3, 0.008, 43);
        flush();
        assertTrue(label("deadlineLabel").contains("+0.004000 s (late)"));
    }

    @Test
    public void duplicateSequenceRefreshesTelemetryButNotCompletedFrameProgress() throws Exception {
        chart.updateFrameData(1.0, "Run", Collections.emptyList());
        chart.updateRealtimeHealth(true, 0.02, 0, 0, 0, 0, 42);
        flush();
        long progress = (Long) field("lastFrameProgressNanos");
        chart.updateRealtimeHealth(true, 0.02, 0, 0, 0, 0, 42);
        flush();
        assertEquals(progress, field("lastFrameProgressNanos"));
        assertTrue((Long) field("lastTelemetryNanos") >= progress);
        renderHealth(progress + 2_000_000_000L);
        assertTrue(label("telemetryLabel").contains("STALLED"));
        assertTrue(label("telemetryLabel").contains("stale"));
        SwingUtilities.invokeAndWait(() -> {
            try {
                Field timestamp = RealTimeJobPieChart.class.getDeclaredField("lastFrameProgressNanos");
                timestamp.setAccessible(true);
                timestamp.setLong(chart, progress - 2_000_000_000L);
            } catch (Exception exception) {
                throw new AssertionError(exception);
            }
        });
        renderHealth((Long) field("lastTelemetryNanos"));
        assertTrue(label("telemetryLabel").contains("STALLED"));
        assertFalse(label("telemetryLabel").contains("stale"));
        chart.updateFrameData(1.0, "Freeze", Collections.emptyList());
        flush();
        renderHealth(progress + 2_000_000_000L);
        assertFalse(label("telemetryLabel").contains("STALLED"));
        chart.updateFrameData(1.0, "Run", Collections.emptyList());
        chart.updateRealtimeHealth(false, 0.02, 0, 0, 0, 0, 42);
        flush();
        renderHealth(progress + 2_000_000_000L);
        assertFalse(label("telemetryLabel").contains("STALLED"));
        assertTrue(label("deadlineLabel").contains("not applicable"));
        assertFalse(label("deadlineLabel").contains("(headroom)"));
        chart.updateRealtimeHealth(true, 0.02, 0, 0, 0, 0, 43);
        flush();
        assertTrue((Long) field("lastFrameProgressNanos") > progress);
    }

    @Test
    public void missingMetricsAreUnavailableAndResetDiscardsQueuedHealth() throws Exception {
        chart.updateRealtimeHealthUnavailable("Missing completed_frame_count");
        flush();
        assertTrue(label("realtimeHealthLabel").contains("Unavailable"));
        assertTrue(label("realtimeHealthLabel").contains("Missing completed_frame_count"));
        assertFalse(label("deadlineLabel").contains("0.000000"));
        SwingUtilities.invokeAndWait(() -> {
            chart.updateRealtimeHealth(true, 0.02, 1, 3, 2, 1, 8);
            chart.updateRealtimeHealthUnavailable("queued");
            chart.reset();
        });
        flush();
        assertEquals(false, field("hasHealthSample"));
        assertEquals(0L, field("totalOverruns"));
        assertEquals(0L, field("lastTelemetryNanos"));
        assertTrue(label("telemetryLabel").contains("Disconnected"));
        assertNull(field("healthUnavailableReason"));
        chart.updateRealtimeHealth(true, 0.02, 0, 0, 0, 0, 1);
        flush();
        assertTrue(label("telemetryLabel").contains("Completed frame: 1"));
    }

    @Test
    public void samplesCountOnceEvenWithoutRedraws() throws Exception {
        for (int i = 1; i <= 101; i++) {
            chart.updateFrameData(i, "Run", Collections.singletonList(
                    new RealTimeJobPieChart.JobDuration("job", i)));
        }
        flush();
        assertEquals(5150.0, total("job"), 0.0);
        render();
        render();
        assertEquals(5150.0, total("job"), 0.0);
        for (int i = 0; i < 100; i++) {
            chart.updateFrameData(i, "Run", Collections.emptyList());
        }
        flush();
        assertEquals(0.0, total("job"), 0.0);
    }

    @Test
    public void threadSwitchDiscardsAlreadyQueuedSamples() throws Exception {
        SwingUtilities.invokeAndWait(() -> {
            chart.updateFrameData(1.0, "Run", Collections.singletonList(
                    new RealTimeJobPieChart.JobDuration("old", 1.0)));
            try {
                Method method = RealTimeJobPieChart.class.getDeclaredMethod("clearRollingTotals");
                method.setAccessible(true);
                method.invoke(chart);
            } catch (Exception exception) {
                throw new AssertionError(exception);
            }
        });
        flush();
        assertTrue(((Map<?, ?>) field("jobTotals")).isEmpty());
        assertNull(field("pendingFrameData"));
    }

    @Test
    @SuppressWarnings("unchecked")
    public void pinningBelowThresholdDoesNotDuplicateAndResetClearsDisplayedModels() throws Exception {
        SwingUtilities.invokeAndWait(() -> {
            try {
                ((Set<String>) field("pinnedJobs")).add("small");
            } catch (Exception exception) {
                throw new AssertionError(exception);
            }
        });
        chart.updateFrameData(1.0, "Run", Arrays.asList(
                new RealTimeJobPieChart.JobDuration("big", 10000.0),
                new RealTimeJobPieChart.JobDuration("small", 1.0)));
        flush();
        render();
        List<RealTimeJobPieChart.JobDuration> frame =
                (List<RealTimeJobPieChart.JobDuration>) field("currentFrameData");
        assertEquals(2, frame.size());
        assertEquals(1.0, total("small"), 0.0);
        chart.reset();
        flush();
        for (String name : Arrays.asList(
                "currentFrameList", "pinnedJobsList", "unpinnedTotalList", "pinnedTotalList")) {
            assertEquals(name, 0, ((JList<?>) field(name)).getModel().getSize());
        }
    }
}
