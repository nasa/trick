package trick.trickify;

import java.awt.event.*;
import java.io.*;
import javax.swing.*;

public class LabeledTextField extends JPanel
{
    private int pos_x = 0;
    private int pos_y = 0;

    private JLabel label;
    private int label_pos_x = pos_x;
    private int label_pos_y = pos_y;
    private int label_width = 200;
    private int label_height = 20;

    private JTextField textfield;
    private int textfield_pos_x = pos_x;
    private int textfield_pos_y = pos_y + label_height;
    private int textfield_width = 800;
    private int textfield_height = 20;

    public void setLabel(String text)
    {
        label.setText(text);
    }

    public void setToolTipText(String tip)
    {
        textfield.setToolTipText(tip);       
    }

    public void addToPanel(JPanel panel)
    {
    }

    public void addToFrame(JFrame frame)
    {
        frame.add(label);
        frame.add(textfield);
    }

    public String getText()
    {
        return textfield.getText();
    }

    void setText(String txt)
    {
        textfield.setText(txt);
    }

    public LabeledTextField()
    {
        label = new JLabel();
        label_width = 200;
        label_height = 20;

        textfield = new JTextField();
        textfield_width = 800;
        textfield_height = 20;

        add(label);
        add(textfield);
    }

}
