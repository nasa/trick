//https://stackoverflow.com/questions/22549690/how-to-create-a-cube-in-swing
//https://gist.github.com/miku/4376898
//https://rosettacode.org/wiki/Draw_a_rotating_cube#Java
//https://www.bogotobogo.com/Java/tutorials/javagraphics3.php
//https://docs.oracle.com/javase/tutorial/uiswing/painting/step2.html

package main.java.trick.jetDisplay;
import java.awt.*;    // tools for design and graphics
import javax.swing.*; // tools for GUI 

/* JPanel represents area with controls/visuals 
 * extends == is-a .. so the class is a JPanel
 * inside class: setColor(color)
 * outside class: exp.setColor(color)
*/
/*Should main be within JPanel or JFrame? */
class exp extends JPanel {
    /* will drawing be done here or in the paintcomponent method??*/
    private void doDrawing(Graphics g){
        Graphics2D g2d = (Graphics2D) g;

        g2d.drawRect(1,1,200,150);
        g2d.setPaint(Color.MAGENTA);
        g2d.fillRect(1, 1, 200, 150);

        g2d.drawLine(35, 95, 200, 200);
    }
 
    @Override
    public void paintComponent(Graphics g){
        super.paintComponent(g);
        doDrawing(g);
    }
   
} 
/* JFrame represents a framed window */
class Display extends JFrame {
    public static void main(String[] args) {
        System.out.println("I've returned!");
        
       /*EventQueue.invokeLater(new Runnable(){
            @Override
            public void run(){
                Display ex = new Display();
                ex.setVisible(true);
            }
        });*/

        Display ex = new Display();
        ex.setVisible(true);
        
        
       
    }
    public Display(){
        //Display();
    }
    private void DisplayGUI(){
        add(new exp());
        setTitle("Java Experiment");
        setSize(1300,800);
        setLocationRelativeTo(null); // centers the GUI window 
        setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
    }
    
}
