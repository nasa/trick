package main.java.trick.jetDisplay;
import javax.swing.*;

public class myExperiment extends JPanel{
    public static void main (String[] args){
        System.out.println("We're here");
        Display display = new Display();
        display.setVisible(true);
    }

    
}
class Display extends JFrame {
    public Display(){
        Display();
    }
    private static void displayGUI(){
        System.out.println("GUI is going to display");
        add(new myExperiment());
        setTitle("Java Display Experiment");
        setSize(1300,800);
        setLocationRelativeTo(null);
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

    }
}