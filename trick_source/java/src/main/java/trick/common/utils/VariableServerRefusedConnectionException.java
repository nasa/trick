package trick.common.utils;

import java.io.IOException;

/**
 * Thrown when a TCP connection to a simulation's variable server is established at
 * the socket level, but the variable server does not actually service the client.
 *
 * This happens when the variable server is disabled, is not accepting connections,
 * or has rejected this client (for example, because the client's IP address is not
 * on the variable server's allowlist). Without this, the client would connect and
 * then block forever waiting for data that the server never sends.
 */
public class VariableServerRefusedConnectionException extends IOException {

    /**
     * constructs the exception with the given detail message
     *
     * @param message the detail message describing why the connection was refused
     */
    public VariableServerRefusedConnectionException(String message) {
        super(message);
    }

    /**
     * constructs the exception with the given detail message and cause
     *
     * @param message the detail message describing why the connection was refused
     * @param cause the underlying cause
     */
    public VariableServerRefusedConnectionException(String message, Throwable cause) {
        super(message, cause);
    }
}
