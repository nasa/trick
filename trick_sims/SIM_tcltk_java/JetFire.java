import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import java.io.IOException;

import java.net.UnknownHostException;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JTextField;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.SwingWorker;

import trick.common.utils.VariableServerConnection;
 
public class JetFire extends JPanel{
   
    private JButton fireJetButton;    
    private JTextField dataField;
    private VariableServerConnection vsConnection;
    private static String host = "localhost";
    private static int port = 7000;

    public JetFire() {
        setLayout(new GridBagLayout());
	    dataField = new JTextField(15);
		dataField.setEditable(false);
        fireJetButton = new JButton("Fire Jet");
        fireJetButton.addActionListener(new MyButtonListener());

        // creates a constraints object
        GridBagConstraints c = new GridBagConstraints();
        c.insets = new Insets(12, 12, 12, 12); // insets for all components
        c.gridx = 0; // column 0
        c.gridy = 0; // row 0     
        c.ipadx = 5; // increases components width by 10 pixels
        c.ipady = 5; // increases components height by 10 pixels       
        c.gridwidth = 2;
        c.fill = GridBagConstraints.HORIZONTAL; // Horizontal only
        add(fireJetButton, c);

        c.gridx = 0; // column 0
        c.gridy = 1; // row 1
        c.gridwidth = 1;
        c.fill = GridBagConstraints.NONE; // Remember to reset to none
        add(new JLabel("pos[0]   "), c);

        c.gridx = 1; // column 1
        add(dataField, c);
        try {
            vsConnection = new VariableServerConnection(host, port);
        } catch (UnknownHostException host_exception) {
            /** The IP address of the host could not be determined. */              
            System.out.println(" Unknown host \""+host+"\"");        
        } catch (IOException ioe) {
            /** Port number is unavailable, or there is no connection, etc. */
            System.out.println(" Invalid TCP/IP port number \""+port+"\"");
            System.out.println(" Please check the server and enter a proper port number!");
            System.out.println(" IOException ..." + ioe);
            System.out.println(" If there is no connection, please make sure SIM is up running properly!");                      
        } 
        if (vsConnection != null) {
            (new MonitorVariableTask()).execute();
            try {
                //vsConnection.setDebugLevel(3);
                vsConnection.setCycle(0.5);
                vsConnection.add("dyn.baseball.pos[0]");
            } catch (IOException ioe) {
            }       
        } else {
			System.exit(0);
		}   
    }

    private static void createAndShowGUI() {
        //Create and set up the window.
        JFrame frame = new JFrame("Jet Fire");
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
 		frame.setLocationRelativeTo(null); 

        frame.getContentPane().add(new JetFire());
        
        frame.setSize(350, 200);
        frame.setVisible(true);
    }
 
    public static void main(String[] args) {
		// if a valid port number is provided, use it. 
		// Otherwise, the default 7000 would be used.
        if (args.length > 0) {
            try {
                port = Integer.parseInt(args[0]);
            } catch (Exception e) {
                // if the port provided is not a valid number, do nothing           
            }        
        }
        javax.swing.SwingUtilities.invokeLater(new Runnable() {
            public void run() {
                createAndShowGUI();
            }
        });
    }

    private class MyButtonListener implements ActionListener {
        MyButtonListener() {
        }

        public void actionPerformed(ActionEvent e) {
            if (e.getActionCommand().equals("Fire Jet")) {
                if (vsConnection != null) {
                    try {
                        vsConnection.put("trick_mm.mmw.mm.read_checkpoint_from_string(\"dyn.baseball.jet_on = 1 ;\")");
                    } catch (IOException ioe) {
                        // do nothing
                    }
                }
            }
        }
    }

    private class MonitorVariableTask extends SwingWorker<Void, Void> {
        @Override
        public Void doInBackground() {
            String results[] = null;
            while (true) {
                try {
                    if (vsConnection != null) {
                        results = vsConnection.get().split("\t");
                        if (results != null && results[0].equals("0")) {
                            dataField.setText(results[1]);                        
                        } else {
                            break;
                        }                  
                    }
                } catch (Exception e) {
                    break;
                }            
            }
            return null;
        }
        @Override
		public void done() {
        	fireJetButton.setEnabled(false);
            try {
                if (vsConnection != null) {
                    vsConnection.close();            
                }
            } catch (IOException ioe) { 
                // do nothing            
            }
        }
    }

}
