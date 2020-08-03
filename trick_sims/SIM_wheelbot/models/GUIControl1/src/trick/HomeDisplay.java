/*
 * Homing Varible Server Client
*/

package trick;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JFrame;
import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.Socket;
import java.util.*;
import java.io.*;

// Create the HomeButton class by extending JPanel and ActionListener
class HomeButton extends JPanel implements ActionListener {

    // Private variables
    private boolean homeCommand;
    private JButton homeButton1;
    private JLabel label;
    public HomeButton() {
        createButton();
    }

    // Constructor
    public void createButton() {
        homeCommand = false;

        setBorder(BorderFactory.createEmptyBorder(30, 30, 10, 30));
        setLayout(new GridLayout(0, 1));

        homeButton1 = new JButton("GO TO HOME");
        homeButton1.addActionListener(this);
        homeButton1.setActionCommand("home");
        add(homeButton1);

        label = new JLabel("Status : Deactivated");
        add(label);
    }

    // Action event function
    public void actionPerformed(ActionEvent e) {
        String s = e.getActionCommand();
        switch (s) {
            case "home" :
                homeCommand = true;
                label.setText("Status : Activated");
                break;
            default:
                System.out.println("Unknown Action Command: " + s);
                break;
        }
    }

    // HomeCommand variable functions
    public void resetHomeCommand() {
        homeCommand = false;
    }

    public boolean getHomeCommand() {
        return homeCommand;
    }
}

// Delcare trick sim modes
class TrickSimMode {
    public static final int INIT = 0;
    public static final int FREEZE = 1;
    public static final int RUN = 5;
}

// Main display class extending the JFrame
public class HomeDisplay extends JFrame {

    // Private variables
    private HomeButton buttonPanel;
    private BufferedReader in;
    private DataOutputStream out;

    // Constructor
    public HomeDisplay() {
        setTitle("WheelBot Control");

        buttonPanel = new HomeButton();
        add(buttonPanel, BorderLayout.CENTER);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    }

    // Extention functions for HomeCommand
    public void resetHomeCommand() {
        buttonPanel.resetHomeCommand();
    }

    public boolean getHomeCommand() {
        return buttonPanel.getHomeCommand();
    }

    // Connect to the Trick variable server
    public void connectToServer(String host, int port ) throws IOException {
        Socket socket = new Socket(host, port);
        in = new BufferedReader( new InputStreamReader( socket.getInputStream()));
        out = new DataOutputStream(new BufferedOutputStream(socket.getOutputStream()));
    }

    private static void  printHelpText() {
        System.out.println(
            "----------------------------------------------------------------------\n"
          + "usage: java jar HomeDisplay.jar <port-number>\n"
          + "----------------------------------------------------------------------\n"
          );
    }

    // Main: Displays the button and uses the variable server client to update veh.vehicle.homeCommanded
    public static void main(String[] args) throws IOException {

        String host = "localHost";
        int port = 0;

        int ii = 0;
        while (ii < args.length) {
            switch (args[ii]) {
                case "-help" :
                case "--help" : {
                    printHelpText();
                    System.exit(0);
                } break;
                default : {
                    port = (Integer.parseInt(args[ii]));
                } break;
            }
            ++ii;
        }

        // Display the GUI
        HomeDisplay displayGUI = new HomeDisplay();
        int CommandedHome = 0;
        boolean go = true;
        int simMode = 0;

        displayGUI.pack();
        displayGUI.setVisible(true);

        // Check for valid port
        if (port == 0) {
            System.out.println("No variable server port specified.");
            printHelpText();
            System.exit(0);
        }

        // Connect to Trick variable server
        System.out.println("Connecting to: " + host + ":" + port);
        displayGUI.connectToServer(host, port);

        displayGUI.out.writeBytes("trick.var_set_client_tag(\"Home Button\") \n");
        displayGUI.out.flush();

        // Cycically read the simulation mode and the homeCommanded value
        displayGUI.out.writeBytes("trick.var_pause() \n" +
                           "trick.var_add(\"trick_sys.sched.mode\")\n" +
                           "trick.var_add(\"veh.vehicle.homeCommanded\") \n" +
                           "trick.var_ascii() \n" +
                           "trick.var_cycle(0.1) \n" +
                           "trick.var_unpause() \n" );

        displayGUI.out.flush();

        // Loop for simulation
        while (go) {
          try {
            String line;
            String field[];
            line = displayGUI.in.readLine();
            field = line.split("\t");
            simMode = Integer.parseInt( field[1]);
            CommandedHome = Integer.parseInt( field[2]);
          } catch (IOException | NullPointerException e) {
              go = false;
          }

          // Check if homeCommanded is true (button pressed) and update veh.vehicle.homeCommanded
          if (simMode == TrickSimMode.RUN) {
              if (displayGUI.getHomeCommand()) {
                  displayGUI.out.writeBytes("veh.vehicle.homeCommanded = 1 ;\n");
                  displayGUI.out.flush();
                  displayGUI.resetHomeCommand();
              }
          }
        } // while
    } // main
} // class
