package trick.trickify;

import java.awt.event.*;
import java.io.*;
import javax.swing.*;

public class LabeledTextField
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

    private void updatePosition()
    {
        label_pos_x = pos_x;
        label_pos_y = pos_y;

        textfield_pos_x = pos_x;
        textfield_pos_y = pos_y + label_height;
    }

    private void redraw()
    {
        label.setBounds(label_pos_x, label_pos_y, label_width, label_height);
        textfield.setBounds(textfield_pos_x, textfield_pos_y, textfield_width, textfield_height);
    }

    public void setPosition(int x, int y)
    {
        pos_x = x;
        pos_y = y;

        updatePosition();
        redraw();
    }

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
        panel.add(label);
        panel.add(textfield);
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

        setPosition(pos_x, pos_y);
    }

}
