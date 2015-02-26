/*
 * VarClient.java: A generic variable server client
 *                 Developed for use by MTV, currently only handles a subset of Trick types in binary mode
 * @author dstrauss
 */

package trick.utils;

import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;
import java.net.UnknownHostException;

public class VarClient
{
  protected Socket socket = null;
  protected boolean ascii = true;
  protected boolean binary = false;
  protected boolean binary_nonames = false;
  protected boolean sync = false;
  protected BufferedReader vs_in = null;
  protected DataOutputStream vs_out = null;
  protected byte[] buf = new byte[8192]; // maximum binary packet size sent by var server

  public String results; // tabbed string returned by get()
  public boolean paused = false;

  // Connect to variable server on host:port
  public VarClient(String host, int port) throws IOException {
      //String clientTag = "var_client";
      try {
          socket = new Socket(host, port);
      } catch (UnknownHostException e1) {
          throw new IOException("VarClient could not connect to " + host + ":" + port);
      }
      try {
          vs_out = new DataOutputStream(new BufferedOutputStream(socket.getOutputStream()));
          vs_in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
      } finally {
      }
  }

  // Send command to variable server
  public void put(String command) throws IOException {
      vs_out.writeBytes(command + "\n");
      vs_out.flush();
  }

  // Set var_ascii mode
  public void setAscii() throws IOException {
      if (!ascii) {
          put("trick.sim_services.var_ascii()");
          ascii = true;
          binary = false;
          binary_nonames = false;
      }
  }

  // Set var_binary mode
  public void setBinary() throws IOException {
      if (!binary) {
          put("trick.sim_services.var_binary()");
          binary = true;
          binary_nonames = false;
          ascii = false;
      }
  }

  // Set var_binary_nonames mode
  public void setBinaryNoNames() throws IOException {
      if (!binary_nonames) {
          put("trick.sim_services.var_binary_nonames()");
          binary_nonames = true;
          binary = false;
          ascii = false;
      }
  }

  // Set var_sync mode
  public void setSync() throws IOException {
      if (!sync) {
          put("trick.sim_services.var_sync(1)");
          sync = true;
      }
  }

  // Set var_pause
  public void pause() throws IOException {
      if (!paused) {
          put("trick.sim_services.var_pause()");
          paused = true;
      }
  }

  // Set var_unpause
  public void unpause() throws IOException {
      if (paused) {
          put("trick.sim_services.var_unpause()");
          paused = false;
      }
  }

  // in binary mode, must convert C types (little endian) to java types (big endian)
  // return a java long (64bits) for bytes in buffer starting at i, size = 4 or 8
  private long convert_binary_data(byte[] buf, int i, int size) {
      long byte1, byte2, byte3, byte4, byte5, byte6, byte7, byte8;
      // int is size 4, longlong and double are size 8
      byte1 = (0xFF & ((int)buf[i]));
      byte2 = (0xFF & ((int)buf[i+1]));
      byte3 = (0xFF & ((int)buf[i+2]));
      byte4 = (0xFF & ((int)buf[i+3]));
      if (size==8) {
          byte5 = (0xFF & ((int)buf[i+4]));
          byte6 = (0xFF & ((int)buf[i+5]));
          byte7 = (0xFF & ((int)buf[i+6]));
          byte8 = (0xFF & ((int)buf[i+7]));
          // little -> big endian
          return (long) (byte8<<56 | byte7<<48 | byte6<<40 | byte5<<32 | byte4<<24 | byte3<<16 | byte2<<8 | byte1);
      } else {
          // little -> big endian
          return (long) (byte4<<24 | byte3<<16 | byte2<<8 | byte1);
      }
  }

  /***
  public enum TrickType { VOID, CHAR, UCHAR, CHARSTRING, SHORT, USHORT, INT, UINT, LONG, ULONG, FLOAT, DOUBLE,
                          BITFIELD, UBITFIELD, LONGLONG, ULONGLONG, FILEPTR, BOOLEAN, WCHAR, WCHARSTRING, VOIDPTR, ENUM, STRUCT }
   ***/

  // Read ascii or binary data from variable server, return as tab delimited string for either mode
  public String get() throws IOException {
      return get(1);
  }
  public String get(int num_variables) throws IOException {
      // num_variables (optional) is number of variables you expect to get (needed for binary multiple packet support)
      
      int bytes_read, index, msg_size, len, type, size;
      int num_vars_processed, num_vars_in_msg;
      int ival;
      double dval;
      String name, typename, val;

      if (ascii) {
          results = vs_in.readLine();
          
      } else { // binary
          // handle multiple packets (continue reading if more variables and not eof of socket)
          num_vars_processed = 0;
          bytes_read = 0;
          results = "";
          while ((num_vars_processed < num_variables) && (bytes_read != -1)) {
              // read binary header
              bytes_read = socket.getInputStream().read(buf, 0, 12);
              if (bytes_read == -1) { // eof
                  throw new IOException("Connection closed");
              }
              if ((int)buf[0] != 0) {
                  System.out.println("VarClient: Bad binary message indicator.");
                  return "";
              }
              msg_size = (int)convert_binary_data(buf, 4, 4);
              //System.out.println("msg size=" + msg_size);
              num_vars_in_msg = (int)convert_binary_data(buf, 8, 4);
              //System.out.println("numvars=" + num_vars_in_msg);

              // read binary data
              bytes_read = socket.getInputStream().read(buf, 0, msg_size-8);
              //System.out.println("bytes_read=" + bytes_read);
              index = 0;
              while (index < bytes_read) {
                  if (num_vars_processed > 0) {
                      results += "\t";
                  }
                  if (!binary_nonames) {
                      len = (int)convert_binary_data(buf, index, 4);
                      index += 4;
                      name = new String(buf, index, len);
                      index += len;
                  }
                  type = (int)convert_binary_data(buf, index, 4);
                  index += 4;
                  size = (int)convert_binary_data(buf, index, 4);
                  val = "";
                  //System.out.println("type=" + type + " size=" +size);
              
                  index += 4;
                  switch (type) {
                      case 1 :  // CHAR
                          typename = "CHAR";
                          ival = (char)buf[index];
                          val += ival;
                          break;
                      case 3 :  // CHARSTRING
                          typename = "CHARSTRING";
                          val = new String(buf, index, size);
                          break;
                      case 6 :  // INT
                          typename = "INT";
                          ival = (int)convert_binary_data(buf, index, 4);
                          val += ival;
                          break;
                      case 11 : // DOUBLE
                          typename = "DOUBLE";
                          dval = Double.longBitsToDouble(convert_binary_data(buf, index, 8));
                          val += dval;
                          break;
                      case 14 : // LONGLONG
                          typename = "LONGLONG";
                          dval = (double)convert_binary_data(buf, index, 8);
                          val += dval;
                          break;
                      default :
                          typename = "???";
                          System.out.println("VarClient: Unknown type " +type+ " in get().");
                          break;
                  }
                  results += val;
                  index += size;
                  //System.out.println("name=" +name+ " " +typename+ " size=" +size+ " val=" +val+ " index=" +index);
                  num_vars_processed++;
              } // while message data being parsed
          } // end while num_vars_processed < num_variables
      } // end binary
      return results;
  }

  // Flush any data remaining already sent by variable server
  public void flush() {
      int bytes_avail, bytes_read;
      try {
          bytes_avail = socket.getInputStream().available();
          if (bytes_avail > 0) {
              bytes_read = socket.getInputStream().read(buf, 0, bytes_avail);
              System.out.println("flushed " +bytes_read+ " of " +bytes_avail+ " bytes");
          }
      } catch (IOException ioe) {
          System.out.println("VarClient flush() " +ioe.toString() +".");
      }
  }

  // Close the variable server connection
  public void close() {
      try {
          if (vs_out != null) {
              vs_out.close();
          }
          if (vs_in != null) {
              vs_in.close();
          }
          if (socket != null) {
              socket.close();
              socket = null;
          }
      } catch (IOException ioe) {
          System.out.println("VarClient close() " +ioe.toString() +".");
      }
  }
  
}
