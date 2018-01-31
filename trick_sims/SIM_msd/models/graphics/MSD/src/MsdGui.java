/*
 * Trick
 * 2018 (c) National Aeronautics and Space Administration (NASA)
 * Programmers: Scott P. Fennell
 */


import java.awt.*;
import java.io.*;
import java.net.Socket;

public class MsdGui {

    static BufferedReader in;
    static DataOutputStream out;

    public static void connectToServer(String host, int port) throws IOException {
        Socket socket = new Socket(host, port);
        in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
        out = new DataOutputStream(new BufferedOutputStream(socket.getOutputStream()));
    }

    public static void main(String[] args) throws IOException {

        String host = "localHost";
        int port = 0;

        // ==========================================================
        // Handle program arguments.
        // ==========================================================
        int ii = 0;
        while (ii < args.length) {
            switch (args[ii]) {
                case "-help":
                case "--help": {
                    MsdUtils.printHelpText();
                    System.exit(0);
                }
                break;
                default: {
                    port = (Integer.parseInt(args[ii]));
                }
                break;
            }
            ++ii;
        }

        if (port == 0) {
            System.out.println("No variable server port specified.");
            MsdUtils.printHelpText();
            System.exit(0);
        }

        boolean go = true;
        double dt = 0.01; // Time between updates (seconds).
        double pos = 0.0;
        double vel = 0.0;
        boolean resetCommand;
        boolean startCommand;


        int simMode = 0;


        int mapScale = 32; // pixels per meter.

        MsdView msdView = new MsdView(mapScale);
        MsdDisplay display = new MsdDisplay(msdView);
        MsdController controller = new MsdController(display);
        display.setVisible(true);
        display.drawMsdView();
        controller.setVisible(true);
        controller.setLocation(new Point((int)display.getLocationOnScreen().getX() + display.getWidth(), (int)display.getLocationOnScreen().getY()));
        controller.setSize(controller.getWidth() + 10, display.getHeight());

        // Connect to the Trick simulation's variable server
        System.out.println("Connecting to: " + host + ":" + port);
        connectToServer(host, port);

        out.writeBytes("trick.var_set_client_tag(\"MsdGui\") \n");
        out.flush();

        // Have the Variable Server send us the simulation mode ONCE.
        out.writeBytes("trick.var_add(\"trick_sys.sched.mode\")\n" +
                "trick.var_add(\"dyn.msd.x_0\")\n" +
                "trick.var_add(\"dyn.msd.v_0\")\n" +
                "trick.var_add(\"dyn.msd.m\")\n" +
                "trick.var_add(\"dyn.msd.b\")\n" +
                "trick.var_add(\"dyn.msd.k\")\n" +
                "trick.var_add(\"dyn.msd.F\")\n" +
                "trick.var_send() \n" +
                "trick.var_clear() \n");
        out.flush();

        // Read the response and extract the simulation mode. Initialize text fields with initial values.
        try {
            String line;
            String field[];
            line = in.readLine();
            field = line.split("\t");
            simMode = Integer.parseInt(field[1]);
            controller.positionTextField.setText(String.format("%.2f", new Double(field[2])));
            controller.velocityTextField.setText(String.format("%.2f", new Double(field[3])));
            controller.massTextField.setText(String.format("%.2f", new Double(field[4])));
            controller.dampingTextField.setText(String.format("%.2f", new Double(field[5])));
            controller.springTextField.setText(String.format("%.2f", new Double(field[6])));
            controller.forceTextField.setText(String.format("%.2f", new Double(field[7])));
        } catch (IOException | NullPointerException e) {
            go = false;
        }

        // If we're in FREEZE, make ready to start.
        if (simMode == TrickSimMode.FREEZE) {
            controller.armResetCommand();
        }

        // Configure the Variable Server to cyclically send us the following variables.
        // Tell the variable server:
        //  1) We want the values of the following variables:
        out.writeBytes("trick.var_pause() \n" +
                "trick.var_add(\"dyn.msd.x\")\n" +
                "trick.var_add(\"dyn.msd.v\")\n" +
                "trick.var_add(\"trick_sys.sched.mode\")\n" +
                //  2) We want the responses in ASCII:
                "trick.var_ascii() \n" +
                /*  3) We want debug information:
                "trick.var_debug(1) \n" +
                */
                //  4) We want values to be updated at the specified rate:
                String.format("trick.var_cycle(%.3f)\n", dt) +
                //  5) Start sending values as specified.
                "trick.var_unpause() \n");
        out.flush();

        while (go) {

            // Receive and parse periodic data response from the variable server.
            try {
                String line;
                String field[];
                line = in.readLine();
                field = line.split("\t");
                pos = Double.parseDouble(field[1]);
                vel = Double.parseDouble(field[2]);
                simMode = Integer.parseInt(field[3]);
            } catch (IOException | NullPointerException e) {
                go = false;
            }
            // Get inputs from the GUI.
            resetCommand = controller.getResetCommand();
            startCommand = controller.getStartCommand();

            if (simMode == TrickSimMode.FREEZE) {

                controller.armResetCommand();
                resetCommand = false;
                if (!controller.positionTextField.getText().isEmpty() && new Double(controller.positionTextField.getText()) >= 0.0) {
                    display.msdView.hideWall = false;
                }
                if (!(controller.positionTextField.getText().isEmpty() ||
                        controller.velocityTextField.getText().isEmpty() ||
                        controller.springTextField.getText().isEmpty() ||
                        controller.dampingTextField.getText().isEmpty() ||
                        controller.massTextField.getText().isEmpty() ||
                        controller.forceTextField.getText().isEmpty())) {

                    out.writeBytes(
                            "dyn.msd.x_0 = " + controller.positionTextField.getText() + ";\n" +
                                    "dyn.msd.v_0 = " + controller.velocityTextField.getText() + ";\n" +
                                    "dyn.msd.k = " + controller.springTextField.getText() + ";\n" +
                                    "dyn.msd.b = " + controller.dampingTextField.getText() + ";\n" +
                                    "dyn.msd.m = " + controller.massTextField.getText() + ";\n" +
                                    "dyn.msd.F = " + controller.forceTextField.getText() + ";\n");
                    out.writeBytes("dyn.msd.x = dyn.msd.x_0 ;\n");
                    out.writeBytes("dyn.msd.v = dyn.msd.v_0 ;\n");
                }

                if (startCommand) {
                    out.writeBytes(String.format("trick.exec_run();\n"));

                    out.flush();
                }
                if (resetCommand) {
                    out.writeBytes(String.format("trick.exec_freeze() ;\n"));
                    continue;
                }
                out.flush();

            } else if (simMode == TrickSimMode.RUN) {
                controller.armStartCommand();
                startCommand = false;
                if (resetCommand) {
                    out.writeBytes(String.format("trick.exec_freeze() ;\n"));
                    out.flush();
                }
                if (!(controller.springTextField.getText().isEmpty() ||
                        controller.dampingTextField.getText().isEmpty() ||
                        controller.massTextField.getText().isEmpty() ||
                        controller.forceTextField.getText().isEmpty())) {
                    out.writeBytes(
                            "dyn.msd.k = " + controller.springTextField.getText() + ";\n" +
                                    "dyn.msd.b = " + controller.dampingTextField.getText() + ";\n" +
                                    "dyn.msd.m = " + controller.massTextField.getText() + ";\n" +
                                    "dyn.msd.F = " + controller.forceTextField.getText() + ";\n");
                }
            }

            // Update the display data.
            display.setPosition(pos);
            display.setVelocity(vel);

            //  Update the scene.
            display.drawMsdView();

        } // while

    }
}
