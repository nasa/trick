
//========================================
//	Package
//========================================
package trick.test;

//========================================
//	Imports
//========================================
import java.io.IOException;

import trick.common.utils.VariableServerConnection;

/**
 * A test for testing the communication between the Trick Variable Server and a client.
 * 
 * @since Trick 7.2.2
 *
 */
public class Client {
	//========================================
	//	Public data
	//========================================
	
	
	//========================================
	//	Protected data
	//========================================
	
	
	//========================================
	//	Private Data
	//========================================
	
	
	//========================================
	//	Constructors
	//========================================
	
	
	//========================================
	//	Set/Get methods
	//========================================
	
	
	//========================================
	//	Methods
	//========================================
	/**
	 * Main call for the Trick Variable Server connection test.
	 * 
	 * @param args	arguments passed from JVM
	 */
        public static void main ( String[] args ) {

                VariableServerConnection vs = null ;

		if (args.length > 1) {
		} 
		
                try {
			if (args.length > 1) {
				try {
					vs = new VariableServerConnection(args[0], Integer.parseInt(args[1]));
				} catch (NumberFormatException nfe) {
					System.out.println("Please provide appropriate port number!"); 
					System.exit(0);
				}
			} else {
                        	vs = new VariableServerConnection("localhost", 7000);
			}
                } catch (IOException e) {
                        System.out.println("Couldn't connect!");
                        System.exit(1);
                }

                try {
                        // vs.put("var_clear ;" );
                        vs.put("var_cycle = 1.0 ;" +
                            "get sys.exec.out.time ; " +
                            "get sun_predictor.sun.solar_azimuth ; " +
                            "get sun_predictor.sun.test_string ; " +
                            "var_send ;");

                } catch (IOException e) {
                        System.out.println("Put failed!");
                }

                while (true) {
                        try {
                                String foo = vs.get();
                                int len = foo.length();
                                System.out.println(foo + Integer.toString(len) + "\n");
                        } catch (IOException e) {
                                System.out.println("get failed.\n");
                        } catch (NullPointerException npe) {
                                System.out.println("Done!");
                                System.exit(0);
                        }
                }
        }
}

