package trick.rtjperf;

import static org.junit.Assert.*;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicInteger;
import org.junit.Test;
import trick.common.utils.VariableServerConnection;

/** Worker lifecycle and subscription ordering, without a simulation or display. */
public class RTJPerfVarServerClientTest {
    private static final long TIMEOUT = 3000;

    @Test(timeout = 10000)
    public void pollingEofClosesConnectionAndNotifiesOnce() throws Exception {
        try (Fixture f = new Fixture(false)) {
            f.start();
            f.server.awaitSend();
            f.server.reply("100\t100\t5\t0.125");
            f.server.awaitSend();
            f.server.endResponses();
            f.awaitTermination();
            assertEquals(1, f.gui.frames.size());
            assertEquals("mainJob", f.gui.frames.get(0).get(0).jobId);
            assertEquals(2, f.connection.sampleReads.size());
            assertTrue(f.gui.healthSamples.isEmpty());
            assertTrue(f.gui.unavailableReason.contains("rebuild the simulation"));
            f.assertCleanup();
        }
    }

    @Test(timeout = 10000)
    public void initializationEofClosesConnectionAndNotifiesOnce() throws Exception {
        try (Fixture f = new Fixture(true)) {
            f.start();
            f.awaitTermination();
            assertTrue(f.gui.frames.isEmpty());
            assertTrue(f.connection.sampleReads.isEmpty());
            f.assertCleanup();
        }
    }

    @Test(timeout = 10000)
    public void threadChangeWaitsForOldResponseAndDiscardsItsSample() throws Exception {
        try (Fixture f = new Fixture(false)) {
            f.start();
            f.server.awaitSend();
            int commandsBeforeSelection = f.connection.commands.size();
            f.client.subscribeToThread(1);
            // Selection only queues intent: it must not touch the socket or decoding map.
            assertEquals(commandsBeforeSelection, f.connection.commands.size());
            assertNull("Only one var_send may be outstanding",
                    f.server.sends.poll(100, TimeUnit.MILLISECONDS));
            assertEquals("Subscription must not change before the outstanding read completes",
                    commandsBeforeSelection, f.connection.commands.size());
            f.server.reply("100\t100\t5\t0.99");
            f.server.awaitSend();
            assertTrue("The old in-flight sample must be discarded", f.gui.frames.isEmpty());
            f.server.reply("200\t100\t5\t0.25\t0.5");
            f.server.awaitSend();
            f.server.endResponses();
            f.awaitTermination();

            assertEquals(1, f.gui.frames.size());
            List<RealTimeJobPieChart.JobDuration> frame = f.gui.frames.get(0);
            assertEquals(2, frame.size());
            assertDuration(frame, "childA", 0.25);
            assertDuration(frame, "childB", 0.5);
            assertEquals(Integer.valueOf(4), f.connection.sampleReads.get(0));
            assertEquals(Integer.valueOf(5), f.connection.sampleReads.get(1));
            assertEquals(Integer.valueOf(5), f.connection.sampleReads.get(2));
            assertTrue(f.connection.commands.contains(
                    "trick.var_add(\"trick_sys.sched.all_jobs_vector[1].prev_frame_time_seconds\")"));
            assertTrue(f.connection.commands.contains(
                    "trick.var_add(\"trick_sys.sched.all_jobs_vector[2].prev_frame_time_seconds\")"));
            for (Thread writer : f.connection.writers) {
                assertSame("Only the network worker may configure the subscription", f.worker, writer);
            }
            f.assertCleanup();
        }
    }

    @Test(timeout = 10000)
    public void rateChangeIsAppliedByWorkerBeforeNextRequest() throws Exception {
        try (Fixture f = new Fixture(false)) {
            f.start();
            f.server.awaitSend();
            int commandsBeforeChange = f.connection.commands.size();
            f.client.setCycleRate(0.02);
            assertEquals(commandsBeforeChange, f.connection.commands.size());
            f.server.reply("100\t100\t5\t0.125");
            f.server.awaitSend();
            assertTrue(f.connection.commands.contains("trick.var_cycle(0.02)"));
            for (Thread writer : f.connection.writers) {
                assertSame(f.worker, writer);
            }
            f.server.endResponses();
            f.awaitTermination();
            f.assertCleanup();
        }
    }

    private static void assertDuration(List<RealTimeJobPieChart.JobDuration> frame, String name, double duration) {
        for (RealTimeJobPieChart.JobDuration job : frame) {
            if (name.equals(job.jobId)) {
                assertEquals(duration, job.duration, 0.0);
                return;
            }
        }
        fail("Missing job " + name);
    }

    @Test(timeout = 10000)
    public void preservesMeasuredDurationsWithoutFlooringOrDroppingLongJobs() throws Exception {
        try (Fixture f = new Fixture(false)) {
            f.start();
            f.server.awaitSend();
            f.server.reply("100\t100\t5\t0.00000025");
            f.server.awaitSend();
            f.server.reply("200\t100\t5\t120.0");
            f.server.awaitSend();
            f.server.endResponses();
            f.awaitTermination();
            assertDuration(f.gui.frames.get(0), "mainJob", 0.00000025);
            assertDuration(f.gui.frames.get(1), "mainJob", 120.0);
            f.assertCleanup();
        }
    }

    @Test(timeout = 10000)
    public void healthIncludesRetainedOverrunsAndDuplicateFrameObservations() throws Exception {
        try (Fixture f = new Fixture(false, true, true)) {
            f.start();
            f.server.awaitSend();
            f.server.replyHealth("100\t100\t5\t1\t0.02\t0.015\t1\t1\t0.015\t10\t0.125");
            f.server.awaitSend();
            // Five frames later the sim has recovered, but the peak and cumulative
            // counter still reveal an overrun that occurred between polls.
            f.server.replyHealth("200\t100\t5\t1\t0.02\t-0.005\t3\t0\t0.03\t15\t0.002");
            f.server.awaitSend();
            f.server.replyHealth("200\t100\t5\t1\t0.02\t-0.005\t3\t0\t0.03\t15\t0.002");
            f.server.awaitSend();
            f.server.endResponses();
            f.awaitTermination();
            assertEquals(3, f.gui.healthSamples.size());
            HealthSample recovered = f.gui.healthSamples.get(1);
            assertTrue(recovered.active);
            assertEquals(0.02, recovered.budget, 0.0);
            assertEquals(-0.005, recovered.lateness, 0.0);
            assertEquals(3, recovered.overruns);
            assertEquals(0, recovered.consecutive);
            assertEquals(0.03, recovered.peak, 0.0);
            assertEquals(15, recovered.sequence);
            assertEquals(15, f.gui.healthSamples.get(2).sequence);
            assertDuration(f.gui.frames.get(0), "mainJob", 0.125);
            assertDuration(f.gui.frames.get(1), "mainJob", 0.002);
            assertEquals(Integer.valueOf(11), f.connection.sampleReads.get(0));
            assertNull(f.gui.unavailableReason);
            f.assertCleanup();
        }
    }

    private static class HealthSample {
        final boolean active;
        final double budget, lateness, peak;
        final long overruns, consecutive, sequence;

        HealthSample(boolean active, double budget, double lateness, long overruns,
                long consecutive, double peak, long sequence) {
            this.active = active;
            this.budget = budget;
            this.lateness = lateness;
            this.overruns = overruns;
            this.consecutive = consecutive;
            this.peak = peak;
            this.sequence = sequence;
        }
    }

    private static class RecordingGui extends RealTimeJobPieChart {
        final List<List<JobDuration>> frames = Collections.synchronizedList(new ArrayList<>());
        final List<HealthSample> healthSamples = Collections.synchronizedList(new ArrayList<>());
        volatile String unavailableReason;

        @Override
        public void updateRealtimeHealth(boolean active, double budget, double lateness,
                long overruns, long consecutive, double peak, long sequence) {
            healthSamples.add(new HealthSample(active, budget, lateness, overruns, consecutive, peak, sequence));
        }

        @Override
        public void updateRealtimeHealthUnavailable(String reason) {
            unavailableReason = reason;
        }

        @Override
        public void setSoftwareFrameRate(double rate) {}

        @Override
        public void registerAllJobs(List<String> names) {}

        @Override
        public void initializeThreads(int count, RTJPerfVarServerClient client) {}

        @Override
        public void updateFrameData(double time, String mode, List<JobDuration> jobs) {
            frames.add(new ArrayList<>(jobs));
        }
    }

    /**
     * Retains real socket construction, handshake, ASCII initialization and close.
     * Only binary telemetry decoding is replaced by a line-based test protocol.
     */
    private static class RecordingConnection extends VariableServerConnection {
        final List<String> commands = Collections.synchronizedList(new ArrayList<>());
        final List<Thread> writers = Collections.synchronizedList(new ArrayList<>());
        final List<Integer> sampleReads = new ArrayList<>();
        final AtomicInteger closes = new AtomicInteger();
        final boolean binaryTelemetry;

        RecordingConnection(int port, boolean binaryTelemetry) throws IOException {
            super(InetAddress.getLoopbackAddress().getHostAddress(), port);
            this.binaryTelemetry = binaryTelemetry;
        }

        @Override
        public void put(String command) throws IOException {
            // The superclass constructor invokes put during its handshake.
            if (commands != null) {
                for (String line : command.split("\n")) {
                    if (!line.isEmpty()) {
                        commands.add(line);
                        writers.add(Thread.currentThread());
                    }
                }
            }
            super.put(command);
        }

        @Override
        public String get(int count) throws IOException {
            if (dataMode == DataMode.BINARY_NO_NAMES) {
                sampleReads.add(count);
                if (binaryTelemetry) {
                    return super.get(count);
                }
            }
            return inputStream.readLine();
        }

        @Override
        public void close() throws IOException {
            closes.incrementAndGet();
            super.close();
        }

        boolean socketClosed() {
            return socket.isClosed();
        }
    }

    private static class FakeServer implements AutoCloseable {
        final ServerSocket listener = new ServerSocket(0, 1, InetAddress.getLoopbackAddress());
        final BlockingQueue<String> sends = new LinkedBlockingQueue<>();
        final Thread thread;
        volatile Socket socket;
        volatile PrintWriter output;
        volatile Throwable failure;
        volatile boolean closing;

        FakeServer(boolean initializationEof, boolean healthSupported) throws IOException {
            thread = new Thread(() -> {
                try (Socket accepted = listener.accept()) {
                    socket = accepted;
                    output = new PrintWriter(accepted.getOutputStream(), true);
                    BufferedReader input = new BufferedReader(new InputStreamReader(accepted.getInputStream()));
                    String command;
                    while ((command = input.readLine()) != null) {
                        if (command.contains("var_exists")) {
                            reply(healthSupported ? "1\t1" : "1\t0");
                        } else if (command.contains("var_send_once") && command.contains("software_frame")) {
                            if (initializationEof) {
                                endResponses();
                            } else {
                                reply("5\t0.001");
                            }
                        } else if (command.contains("num_threads")) {
                            reply("5\t2");
                        } else if (command.contains("var_get_stl_size")) {
                            reply("5\t3");
                        } else if (command.contains("var_send_once") && command.contains(".name")) {
                            reply("5\tmainJob\t0\tchildA\t1\tchildB\t1");
                        } else if (command.equals("trick.var_send()")) {
                            sends.add(command);
                        }
                    }
                } catch (Throwable e) {
                    if (!closing) {
                        failure = e;
                    }
                }
            }, "fake-variable-server");
            thread.setDaemon(true);
            thread.start();
        }

        void awaitSend() throws InterruptedException {
            assertNotNull("Worker did not request telemetry", sends.poll(TIMEOUT, TimeUnit.MILLISECONDS));
            assertNull("Fake server failed", failure);
        }

        void reply(String response) {
            output.println(response);
        }

        // Actual Trick binary-no-names layout, including bool and unsigned counters.
        void replyHealth(String response) throws IOException {
            String[] values = response.split("\t");
            int[] types = {14, 14, 6, 17, 11, 11, 7, 7, 11, 15, 11};
            ByteBuffer packet = ByteBuffer.allocate(512).order(ByteOrder.LITTLE_ENDIAN);
            packet.putInt(0).putInt(0).putInt(values.length);
            for (int i = 0; i < values.length; i++) {
                int type = types[i];
                int size = type == 17 ? 1 : (type == 6 || type == 7 ? 4 : 8);
                packet.putInt(type).putInt(size);
                if (type == 11) {
                    packet.putDouble(Double.parseDouble(values[i]));
                } else if (size == 1) {
                    packet.put(Byte.parseByte(values[i]));
                } else if (size == 4) {
                    packet.putInt(Integer.parseInt(values[i]));
                } else {
                    packet.putLong(Long.parseLong(values[i]));
                }
            }
            packet.putInt(4, packet.position() - 4);
            socket.getOutputStream().write(packet.array(), 0, packet.position());
            socket.getOutputStream().flush();
        }

        void endResponses() throws IOException {
            socket.shutdownOutput();
        }

        @Override
        public void close() throws Exception {
            closing = true;
            listener.close();
            if (socket != null) {
                socket.close();
            }
            thread.join(TIMEOUT);
            assertFalse("Fake server leaked a thread", thread.isAlive());
        }
    }

    private static class Fixture implements AutoCloseable {
        final FakeServer server;
        final RecordingConnection connection;
        final RecordingGui gui = new RecordingGui();
        final AtomicInteger callbacks = new AtomicInteger();
        final CountDownLatch disconnected = new CountDownLatch(1);
        final RTJPerfVarServerClient client;
        final Thread worker;

        Fixture(boolean initializationEof) throws IOException {
            this(initializationEof, false, false);
        }

        Fixture(boolean initializationEof, boolean healthSupported, boolean binaryTelemetry) throws IOException {
            server = new FakeServer(initializationEof, healthSupported);
            connection = new RecordingConnection(server.listener.getLocalPort(), binaryTelemetry);
            client = new RTJPerfVarServerClient(connection, gui, () -> {
                callbacks.incrementAndGet();
                disconnected.countDown();
            });
            worker = new Thread(client, "rtjperf-test-worker");
            worker.setDaemon(true);
        }

        void start() {
            worker.start();
        }

        void awaitTermination() throws InterruptedException {
            assertTrue("Disconnect callback did not run", disconnected.await(TIMEOUT, TimeUnit.MILLISECONDS));
            worker.join(TIMEOUT);
            assertFalse("EOF must terminate the worker", worker.isAlive());
        }

        void assertCleanup() {
            assertEquals(1, callbacks.get());
            assertEquals(1, connection.closes.get());
            assertTrue(connection.socketClosed());
            for (String command : connection.commands) {
                assertFalse("Polling must remain paused", command.contains("var_unpause"));
            }
            assertNull("Fake server failed", server.failure);
        }

        @Override
        public void close() throws Exception {
            if (worker.isAlive()) {
                client.stop();
                worker.join(TIMEOUT);
            }
            server.close();
            if (!connection.socketClosed()) {
                connection.close();
            }
        }
    }
}
