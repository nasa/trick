/*
 * Trick
 * 2018 (c) National Aeronautics and Space Administration (NASA)
 * Programmers: Scott P. Fennell
 */



import javax.swing.*;
import javax.swing.JTextField;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

public class MsdController extends JFrame {
    JTextField positionTextField;
    JTextField velocityTextField;
    JTextField springTextField;
    JTextField dampingTextField;
    JTextField forceTextField;
    JTextField massTextField;

    ButtonPanel buttonPanel;
    JPanel positionPanel;
    JPanel labelPanel;

    public void armResetCommand() {
        buttonPanel.armResetCommand();
    }

    public void armStartCommand() {
        buttonPanel.armStartCommand();
    }

    public boolean getResetCommand() {
        return buttonPanel.getResetCommand();
    }

    public boolean getStartCommand() {
        return buttonPanel.getStartCommand();
    }

    public MsdController(MsdDisplay display){
        setTitle("Mass-Spring-Damper Control");
        setLayout(new GridLayout(1, 2));

        buttonPanel = new ButtonPanel(display);
        positionPanel = new JPanel();
        labelPanel = new JPanel();
        labelPanel.setAlignmentX(Component.RIGHT_ALIGNMENT);

        positionPanel.setLayout(new GridLayout(13, 1));
        labelPanel.setLayout(new GridLayout(13, 1));
        JPanel framePanel = new JPanel();
        GridLayout layout = new GridLayout(1, 3);
        framePanel.setLayout(layout);

        positionTextField = new JTextField(2);
        velocityTextField = new JTextField(2);
        massTextField = new JTextField(2);
        forceTextField = new JTextField(2);
        springTextField = new JTextField(2);
        dampingTextField = new JTextField(2);






        labelPanel.add(new JLabel());
        labelPanel.add(new JLabel("Position: "));
        labelPanel.add(new JLabel());
        labelPanel.add(new JLabel("Velocity: "));
        labelPanel.add(new JLabel());
        labelPanel.add(new JLabel("Mass: "));
        labelPanel.add(new JLabel());
        labelPanel.add(new JLabel("Spring Coefficient: "));
        labelPanel.add(new JLabel());
        labelPanel.add(new JLabel("Damping Coefficient: "));
        labelPanel.add(new JLabel());
        labelPanel.add(new JLabel("Applied Force: "));
        labelPanel.add(new JLabel());

        positionPanel.add(new JLabel());
        positionPanel.add(positionTextField);
        positionPanel.add(new JLabel());
        positionPanel.add(velocityTextField);
        positionPanel.add(new JLabel());
        positionPanel.add(massTextField);
        positionPanel.add(new JLabel());
        positionPanel.add(springTextField);
        positionPanel.add(new JLabel());
        positionPanel.add(dampingTextField);
        positionPanel.add(new JLabel());
        positionPanel.add(forceTextField);
        positionPanel.add(new JLabel());


        for(Component L : labelPanel.getComponents()){
            if(L instanceof JLabel){
                ((JLabel)L).setHorizontalAlignment(JLabel.RIGHT);
            }
        }
        framePanel.add(buttonPanel);
        framePanel.add(labelPanel);
        framePanel.add(positionPanel);

        add(framePanel);
        positionPanel.setSize(positionPanel.getLayout().preferredLayoutSize(positionPanel));
        buttonPanel.setSize(buttonPanel.getPreferredSize());
        setSize(getPreferredSize());
        setLocationByPlatform(true);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

    }

    class ButtonPanel extends JPanel implements ActionListener {

        private MsdDisplay msdDisplay;
        private boolean startCommand;
        private boolean resetCommand;
        private JButton startButton, resetButton, zoomOutButton, zoomInButton;


        public ButtonPanel(MsdDisplay cd) {
            msdDisplay = cd;
            startCommand = false;
            resetCommand = false;
            setLayout(new GridLayout(4,1 ));

            startButton = new JButton("Start");
            startButton.addActionListener(this);
            startButton.setActionCommand("start");
            startButton.setToolTipText("Start MSD");
            add(startButton);

            resetButton = new JButton("Reset");
            resetButton.addActionListener(this);
            resetButton.setActionCommand("reset");
            resetButton.setToolTipText("Reset MSD");
            add(resetButton);

            zoomOutButton = new JButton("\u25b2");
            zoomOutButton.addActionListener(this);
            zoomOutButton.setActionCommand("zoomout");
            zoomOutButton.setToolTipText("Zoom in");
            add(zoomOutButton);

            zoomInButton = new JButton("\u25bc");
            zoomInButton.addActionListener(this);
            zoomInButton.setActionCommand("zoomin");
            zoomInButton.setToolTipText("Zoom Out");
            add(zoomInButton);

        }

        public void actionPerformed(ActionEvent e) {
            String s = e.getActionCommand();
            switch (s) {
                case "start":
                    startCommand = true;
                    break;
                case "reset":
                    resetCommand = true;

                    break;
                case "zoomin":
                    msdDisplay.setScale(msdDisplay.getScale() / 2);
                    break;
                case "zoomout":
                    msdDisplay.setScale(msdDisplay.getScale() * 2);
                    break;
                default:
                    System.out.println("Unknown Action Command:" + s);
                    break;
            }
        }

        public void armResetCommand() {
            resetCommand = false;
            startButton.setForeground(Color.red);
        }

        public void armStartCommand() {
            startCommand = false;
            startButton.setForeground(Color.gray);
        }

        public boolean getResetCommand() {
            return resetCommand;
        }

        public boolean getStartCommand() {
            return startCommand;
        }

    } // class ButtonPanel


}
