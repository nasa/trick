
package trick.common.utils;

import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;
import java.net.UnknownHostException;

/**
 * a generic variable server client that provides for sending commands and
 * receiving responses. Currently only handles a subset of Trick types in binary
 * mode.
 *
 * @author Derek Bankieris
 */
public class VariableServerConnection implements AutoCloseable {

    /** maximum binary packet size sent by the Variable Server */
    public static final int maximumPacketSize = 8192;

    /** Variable Server data mode */
    public enum DataMode {ASCII, BINARY, BINARY_NO_NAMES};

    /** This really shouldn't be necessary. MTV needs to retrieve Variable Server data better. */
    public String results;

    /** connection to the Variable Server */
    protected Socket socket;

    /** Variable Server data mode */
    protected DataMode dataMode = DataMode.ASCII;

    /** Variable Server synchronized mode */
    protected boolean sync = false;

    /** tracks the paused status of the Variable Server */
    protected boolean isPaused = false;

    /** receives commands from the Variable Server */
    protected BufferedReader inputStream;

    /** sends commands to the Variable Server */
    protected DataOutputStream outputStream;

    /**
     * attempts to connect to the Variable Server on the given host and port
     *
     * @param host Variable Server machine name
     * @param port Variable Server port number
     * @throws UnknownHostException UnknownHostException
     * @throws IOException IOException
     * @throws SecurityException SecurityException
     */
    public VariableServerConnection(String host, int port) throws UnknownHostException, IOException, SecurityException {
        socket = new Socket(host, port);
        inputStream = new BufferedReader(new InputStreamReader(socket.getInputStream()));
        outputStream = new DataOutputStream(new BufferedOutputStream(socket.getOutputStream()));
    }

    /**
     * sends the given command and any commands in the output stream's buffer
     * to the Variable Server. The command is written to the output stream,
     * and the buffer is flushed. A newline is automatically appended to the
     * given command.
     *
     * @param command the Variable Server command to be sent
     * @throws IOException IOException
     */
    public void put(String command) throws IOException {
        outputStream.writeBytes(command + "\n");
        outputStream.flush();
    }

    /**
     * writes the command to the output stream. A newline is
     * automatically appended. The buffer is not flushed.
     *
     * @param command the Variable Server command to be written
     * @throws IOException IOException
     */
    public void write(String command) throws IOException {
        outputStream.writeBytes(command + "\n");
    }

    /**
     * flushes the buffer, sending any pending commands to the Variable
     * Server
     * @throws IOException IOException
     */
    public void flush() throws IOException {
        outputStream.flush();
    }

    /**
     * commands the Variable Server to send variable data in ASCII
     * @throws IOException IOException
     */
    public void setAscii() throws IOException {
        if (dataMode != DataMode.ASCII) {
            put("trick.var_ascii()");
            dataMode = DataMode.ASCII;
        }
    }

    /**
     * commands the Variable Server to send variable data in binary
     * @throws IOException IOException
     */
    public void setBinary() throws IOException {
        if (dataMode != DataMode.BINARY) {
            put("trick.var_binary()");
            dataMode = DataMode.BINARY;
        }
    }

    /**
     * commands the Variable Server to send variable data in binary and to
     * omit names
     * @throws IOException IOException
     */
    public void setBinaryNoNames() throws IOException {
        if (dataMode != DataMode.BINARY_NO_NAMES) {
            put("trick.var_binary_nonames()");
            dataMode = DataMode.BINARY_NO_NAMES;
        }
    }

    /**
     * sets the Variable Server to synchronized mode
     * @throws IOException IOException
     */
    public void setSync() throws IOException {
        if (!sync) {
            put("trick.var_sync(1)");
            sync = true;
        }
    }

    /**
     * commands the Variable Server to stop sending data
     * @throws IOException IOException
     */
    public void pause() throws IOException {
        put("trick.var_pause()");
        isPaused = true;
    }

    /**
     * commands the Variable Server to resume sending data
     * @throws IOException IOException
     */
    public void unpause() throws IOException {
        put("trick.var_unpause()");
        isPaused = false;
    }

    /**
     * returns the paused state of the Variable Server
     *
     * @return whether or not the Variable Server is paused
     */
    public boolean isPaused() {
        return isPaused;
    }

    /**
     * adds the named variable to the Variable Server
     *
     * @param name the name of the variable to be added
     * @throws IOException IOException
     */
    public void add(String name) throws IOException {
        put("trick.var_add(\"" + name + "\")");
    }

    /**
     * adds the named variable to the Variable Server
     *
     * @param name the name of the variable to be added
     * @param units the units to use
     * @throws IOException IOException
     */
    public void add(String name, String units) throws IOException {
        boolean invalidUnits = units == null || units.isEmpty();
        put("trick.var_add(\"" + name + (invalidUnits ? "" : "\", \"" + units) + "\")");
    }

    /**
     * removes the named variable from the Variable Server
     *
     * @param name the name of the variable to be removeed
     * @throws IOException IOException
     */
    public void remove(String name) throws IOException {
        put("trick.var_remove(\"" + name + "\")");
    }

    /**
     * clears all variables from the Variable Server
     * @throws IOException IOException
     */
    public void clear() throws IOException {
        put("trick.var_clear()");
    }

    /**
     * sets the period at which the Variable Server sends updates
     *
     * @param period the update period
     * @throws IOException IOException
     */
    public void setCycle(double period) throws IOException {
        put("trick.var_cycle(" + period + ")");
    }

    /**
     * requests a single update from the Variable Server
     * @throws IOException IOException
     */
    public void poll() throws IOException {
        put("trick.var_send()");
    }

    public void setValidateAddresses(boolean validate) throws IOException {
        put("trick.var_validate_address(" + (validate ? "True" : "False") + ")");
    }

    /**
     * sends a freeze command to the simulation
     * @throws IOException IOException
     */
    public void freeze() throws IOException {
        put("trick.exec_freeze()");
    }

    /**
     * sends a run command to the simulation
     * @throws IOException IOException
     */
    public void run() throws IOException {
        put("trick.exec_run()");
    }

    /**
     * sends an enable or disable real time command to the simulation
     * @param enabled true or false
     * @throws IOException IOException
     */
    public void setRealTimeEnabled(boolean enabled) throws IOException {
        put("trick.real_time_" + (enabled ? "enable" : "disable") + "()");
    }

    /**
     * sets the Variable Server's debug level
     *
     * @param level the debug level
     * @throws IOException IOException
     */
    public void setDebugLevel(int level) throws IOException {
        put("trick.var_debug(" + level + ")");
    }

    /**
     * sets this client's tag
     *
     * @param tag the tag that the Variable Server will associate with this client
     * @throws IOException IOException
     */
    public void setClientTag(String tag) throws IOException {
        put("trick.var_set_client_tag(\"" + tag + "\")");
    }

    // in binary mode, must convert C types (little endian) to java types (big endian)
    // return a java long (64bits) for bytes in buffer starting at i, size = 4 or 8
    /**
     * converts incoming binary data from little to big endian
     */
    private long convertBinaryData(byte[] buffer, int index, int size) {
        long byte1, byte2, byte3, byte4, byte5, byte6, byte7, byte8;
        // int is size 4, longlong and double are size 8
        byte1 = (0xFF & ((int)buffer[index]));
        byte2 = (0xFF & ((int)buffer[index+1]));
        byte3 = (0xFF & ((int)buffer[index+2]));
        byte4 = (0xFF & ((int)buffer[index+3]));
        if (size==8) {
            byte5 = (0xFF & ((int)buffer[index+4]));
            byte6 = (0xFF & ((int)buffer[index+5]));
            byte7 = (0xFF & ((int)buffer[index+6]));
            byte8 = (0xFF & ((int)buffer[index+7]));
            // little -> big endian
            return (long) (byte8<<56 | byte7<<48 | byte6<<40 | byte5<<32 | byte4<<24 | byte3<<16 |
              byte2<<8 | byte1);
        } else {
            // little -> big endian
            return (long) (byte4<<24 | byte3<<16 | byte2<<8 | byte1);
        }
    }

    /**
     * Reads characters into a portion of an array. This method will block until some input is available,
     * an I/O error occurs, or the end of the stream is reached.
     *
     * @param buffer destination buffer
     * @param index index within the buffer at which to start storing characters
     * @param maxCount maximum numbers of characters to read
     *
     * @return the number of characters read, or -1 if the end of the stream is reached
     * @throws IOException IOException
     */
    public int read(char[] buffer, int index, int maxCount) throws IOException {
        return inputStream.read(buffer, index, maxCount);
    }

    /**
     * reads a line of data from the Variable Server. Values are tab-delimited.
     *
     * @return a tab-delimited line of data
     * @throws IOException IOException
     */
    public String get() throws IOException {
        return get(1);
    }

    /**
     * reads the specified number of variables from the Variable Server. This
     * supports multiple binary packets.
     *
     * @param  num_variables num variables
     * @return the tab-delimited data
     * @throws IOException IOException
     */
    public String get(int num_variables) throws IOException {
      // num_variables (optional) is number of variables you expect to get (needed for binary multiple packet support)

      int bytes_read, index, msg_size, len, type, size;
      int num_vars_processed, num_vars_in_msg;
      int ival;
      double dval;
      String name, typename, val;
      byte[] buffer = new byte[maximumPacketSize];
      String vals[] = new String[1000];
      int packet_count = 0;

      if (dataMode == DataMode.ASCII) {
          results = inputStream.readLine();
      } else { // binary
          // handle multiple packets (continue reading if more variables and not eof of socket)
          num_vars_processed = 0;
          bytes_read = 0;
          results = "";
          while ((num_vars_processed < num_variables) && (bytes_read != -1)) {
              vals[packet_count] = "";
              // read binary header
              bytes_read = socket.getInputStream().read(buffer, 0, 12);
              if (bytes_read == -1) { // eof
                  throw new IOException("Connection closed");
              }
              if ((int)buffer[0]==3) { // message indicator for send_event_data
                  // simply a message header where msg_size=0 and num_vars_in_msg = number of event variables
                  results += (int)convertBinaryData(buffer, 8, 4);
                  return results;
              } else if ((int)buffer[0] != 0) {
                  System.out.println("VarClient: Bad binary message indicator : " +(int)buffer[0]+ ".");
                  return "";
              }
              msg_size = (int)convertBinaryData(buffer, 4, 4);
              //System.out.println("msg size=" + msg_size);
              num_vars_in_msg = (int)convertBinaryData(buffer, 8, 4);
              //System.out.println("numvars=" + num_vars_in_msg);
              if (num_variables==1) {
              // if user did not specify num_variables, get all vars in this message
                  num_variables = num_vars_in_msg;
              }

              // read binary data
              bytes_read = socket.getInputStream().read(buffer, 0, msg_size-8);
              //System.out.println("bytes_read=" + bytes_read);
              index = 0;
              while (index < bytes_read) {
                  if (num_vars_processed > 0) {
                      vals[packet_count] += "\t";
                  }
                  if (dataMode != DataMode.BINARY_NO_NAMES) {
                      len = (int)convertBinaryData(buffer, index, 4);
                      index += 4;
                      name = new String(buffer, index, len);
                      index += len;
                  }
                  type = (int)convertBinaryData(buffer, index, 4);
                  index += 4;
                  size = (int)convertBinaryData(buffer, index, 4);
                  // Trick10 type of char* is CHAR, change it to CHARSTRING
                  if ((type == 1) && (size > 1)) {
                      type = 3 ;
                  }
                  //System.out.println("type=" + type + " size=" +size);

                  index += 4;
                  switch (type) {
                      case 1 :  // CHAR
                      case 17 : // BOOLEAN
                          //typename = "CHAR";
                          ival = (char)buffer[index];
                          vals[packet_count] += ival;
                          break;
                      case 3 :  // CHARSTRING
                          //typename = "CHARSTRING";
                          vals[packet_count] += new String(buffer, index, size-1); // do not include null terminator
                          break;
                      case 6 :  // INT
                      case 7 :  // UNSIGNED INT
                      case 21 : // ENUMERATED
                      case 24 : // BAD REF
                          //typename = "INT";
                          ival = (int)convertBinaryData(buffer, index, 4);
                          vals[packet_count] += ival;
                          break;
                      case 11 : // DOUBLE
                          //typename = "DOUBLE";
                          dval = Double.longBitsToDouble(convertBinaryData(buffer, index, 8));
                          vals[packet_count] += dval;
                          break;
                      case 14 : // LONGLONG
                          //typename = "LONGLONG";
                          dval = (double)convertBinaryData(buffer, index, 8);
                          vals[packet_count] += dval;
                          break;
                      default :
                          //typename = "???";
                          System.out.println("VarClient: Unknown type " +type+ " in get().");
                          break;
                  }
                  index += size;
                  num_vars_processed++;
              } // while message data being parsed
              results += vals[packet_count];
              packet_count++;
              //System.out.println("pkt " +packet_count+ " : numvars_processed= "+num_vars_processed + " / " +num_variables);
          } // end while num_vars_processed < num_variables
      } // end binary
      return results;
  }

    /**
     * flushes any data that is still on the input stream
     * @throws IOException IOException
     */
    public void flushInput() throws IOException {
        byte[] buffer = new byte[maximumPacketSize];
        int numAvailableBytes = socket.getInputStream().available();
        if (numAvailableBytes > 0) {
            int numBytesRead = socket.getInputStream().read(buffer, 0, numAvailableBytes);
            System.out.println("Flushed " + numBytesRead + " of " + numAvailableBytes +
              " available bytes.");
        }
    }

    /**
     * closes the connection to the Variable Server
     * @throws IOException IOException
     */
    public void close() throws IOException {
        try {
            put("trick.var_exit()");
        }
        finally {
            socket.close();
        }
    }

}
