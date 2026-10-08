package trick.common.utils;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.fail;

import java.io.BufferedReader;
import java.io.EOFException;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;
import org.junit.Test;

public class VariableServerConnectionTest {
    private static byte[] packet(int declaredSize) {
        return ByteBuffer.allocate(24).order(ByteOrder.LITTLE_ENDIAN)
                .putInt(0).putInt(declaredSize).putInt(1)
                .putInt(6).putInt(4).putInt(42).array();
    }

    @Test(timeout = 10000)
    public void readsFragmentedHeaderAndBody() throws Exception {
        exchange(packet(20), 24, true, connection -> {
            assertEquals("42", connection.get(1));
            assertEquals("42", connection.get(1));
        });
    }

    @Test(timeout = 10000)
    public void preservesIntegerCountersWithoutSignednessOrPrecisionLoss() throws Exception {
        byte[] counters = ByteBuffer.allocate(56).order(ByteOrder.LITTLE_ENDIAN)
                .putInt(0).putInt(52).putInt(3)
                .putInt(7).putInt(4).putInt(-1)
                .putInt(14).putInt(8).putLong(9007199254740993L)
                .putInt(15).putInt(8).putLong(-1L).array();
        exchange(counters, counters.length, false, connection -> {
            assertEquals("4294967295\t9007199254740993\t18446744073709551615", connection.get(3));
        });
    }

    @Test(timeout = 10000)
    public void rejectsTruncatedHeaderAndBody() throws Exception {
        for (int length : new int[] {0, 5, 12, 19}) {
            exchange(packet(20), length, false, connection -> {
                try {
                    connection.get(1);
                    fail("Expected premature EOF");
                } catch (EOFException expected) {
                    // A partial packet must never be returned as a valid response.
                }
            });
        }
    }

    @Test(timeout = 10000)
    public void rejectsInvalidDeclaredBodySizes() throws Exception {
        for (int size : new int[] {-1, 0, 7, VariableServerConnection.maximumPacketSize + 9}) {
            exchange(packet(size), 12, false, connection -> {
                try {
                    connection.get(1);
                    fail("Expected invalid size rejection");
                } catch (EOFException unexpected) {
                    throw unexpected;
                } catch (IOException expected) {
                    assertEquals("Invalid binary message size: " + size, expected.getMessage());
                }
            });
        }
    }

    private interface Check {
        void run(VariableServerConnection connection) throws Exception;
    }

    private static void exchange(byte[] packet, int length, boolean fragmented, Check check) throws Exception {
        ExecutorService executor = Executors.newSingleThreadExecutor();
        try (ServerSocket listener = new ServerSocket(0, 1, InetAddress.getLoopbackAddress())) {
            Future<?> server = executor.submit(() -> {
                try (Socket socket = listener.accept()) {
                    socket.setSoTimeout(3000);
                    socket.setTcpNoDelay(true);
                    BufferedReader commands = new BufferedReader(new InputStreamReader(socket.getInputStream()));
                    commands.readLine();
                    OutputStream output = socket.getOutputStream();
                    output.write("1\n".getBytes("US-ASCII"));
                    output.flush();
                    // Wait for mode selection so the handshake reader cannot consume binary data.
                    commands.readLine();
                    for (int response = 0; response < (fragmented ? 2 : 1); response++) {
                        if (fragmented) {
                            for (int i = 0; i < length; i++) {
                                output.write(packet[i]);
                                output.flush();
                                Thread.sleep(10);
                            }
                        } else {
                            output.write(packet, 0, length);
                            output.flush();
                        }
                    }
                } catch (Exception exception) {
                    throw new RuntimeException(exception);
                }
            });
            try (VariableServerConnection connection =
                    new VariableServerConnection(listener.getInetAddress().getHostAddress(), listener.getLocalPort())) {
                connection.socket.setSoTimeout(3000);
                connection.setBinaryNoNames();
                check.run(connection);
            }
            server.get(5, TimeUnit.SECONDS);
        } finally {
            executor.shutdownNow();
        }
    }
}
