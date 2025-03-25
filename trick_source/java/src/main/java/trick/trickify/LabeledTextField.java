package trick.trickify;

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import javax.swing.*;

public class LabeledTextField extends JPanel
{
    protected JLabel label;
    public final static int label_width = 200;
    public final static int label_height = 20;

    protected JTextField textfield;
    public final static int textfield_width = 1000;
    public final static int textfield_height = 20;

    public void setLabel(String text)
    {
        label.setText(text);
    }

    public void setToolTipText(String tip)
    {
        textfield.setToolTipText(tip);       
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
        setLayout(new BorderLayout());

        label = new JLabel();
        label.setPreferredSize(new Dimension(label_width, label_height));

        textfield = new JTextField();
        textfield.setPreferredSize(new Dimension(textfield_width, textfield_height));

        add(label, BorderLayout.NORTH);
        add(textfield, BorderLayout.CENTER);
    }

}
