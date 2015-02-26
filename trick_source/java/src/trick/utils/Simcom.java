/*
 * $Id: Simcom.java 821 2010-07-14 22:12:48Z dbankie $
 *
 */

//========================================
//    Package
//========================================
package trick.utils;

//========================================
//    Imports
//========================================
import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;
import java.net.UnknownHostException;

/** 
 * Provides a Trick Variable Server connection and the means to send commands and retrieve responses.
 *    
 * @since Trick 7.2.2
 */

public class Simcom {
    //========================================
    //    Public data
    //========================================
    
    
    //========================================
    //    Protected data
    //========================================
    /** A <code>Socket</code> for communication. */
    public Socket socket             = null;
    /** A buffering input stream for getting data stream from the Variable Server. */ 
    protected BufferedReader  vs_in     = null; 
    /** A data output stream for writting the command to the Variable Server. */
    protected DataOutputStream vs_out      = null; 
    protected int server_byte_order        = 0x0000; 
    protected short client_byte_order      = 0x0000; 
    
    //========================================
    //    Private Data
    //========================================

        
    //========================================
    //    Constructors
    //========================================
    /** 
     * Class contructor specifing the name of the Variable Server and the port
     * with which it communicates. 
     * 
     * @param host    the name of the Variable Server
     * @param port     the port which the Variable Server communicate with
     * 
     * @throws IOException     If an I/O error occurs
     */
    public Simcom (String host, int port) throws IOException {
        configureSimcom(host, port);
    }

    //========================================
    //    Set/Get methods
    //========================================
    
        
    //========================================
    //    Methods
    //========================================
    /**
     * Helper method for the constructor.
     */
    private void configureSimcom (String host, int port) throws IOException {
        // TODO: Do we need to send the client tag???
        //String clientTag = "var_client";
        
        try {
             socket = new Socket( host, port ); 
                         
        } catch (UnknownHostException e1 ) {
             throw new IOException("Couldn't connect to: " + host + ":" + Integer.toString(port) );      
        }
        
        try {
            vs_out = new DataOutputStream(new BufferedOutputStream (socket.getOutputStream()));

            vs_in = new BufferedReader(new InputStreamReader ( socket.getInputStream()));

            //vs_out.writeShort( client_byte_order );
            //vs_out.writeBytes( clientTag );
                         
            //for (int i=0 ; i < (80-clientTag.length()) ; i++) {
            //    vs_out.writeByte(0x00);
            //}
                         
            //vs_out.flush();

            //Fixme
            //server_byte_order = vs_in.read();
            //server_byte_order = vs_in.read();             
        } finally {
        }
    }
    /** 
     * Send a command <code>String</code> to the Trick Varible Server.
     * 
     * @param command     a <code>String</code> specifying Variable Server commands
     * 
     * @throws IOException    If an I/O error occurs
     */
    public void put(String command) throws IOException {
            
        //vs_out.writeInt(command.length());
        vs_out.writeBytes(command);
        vs_out.flush();
    }

    /** 
     * Retrieve a response from the Trick Variable Server.
     *
     * @return     a <code>String</code> that is the data retrieved from the Variable Server
     * 
     * @throws IOException    If an I/O error occurs
     */
    public String get() throws IOException {
    
        String ret = vs_in.readLine();        
        return ret;
    }
    
    public String getchar() throws IOException {
    
        String ret = Character.toString((char)vs_in.read());        
        return ret;
    }

    /**
     * Clean up the socket.
     *
     */
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
            }            
        } catch (IOException ioe) {
            
        }
    }

}
