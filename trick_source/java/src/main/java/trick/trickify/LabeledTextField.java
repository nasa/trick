package trick.trickify;

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import javax.swing.*;

public class LabeledTextField extends JPanel
{
    protected JLabel label;
    public final static int labelWidth = 200;
    public final static int labelHeight = 20;

    protected JTextField textField;
    public final static int textFieldWidth = 1000;
    public final static int textFieldHeight = 20;

    public void setLabel(String text)
    {
        label.setText(text);
    }

    public void setToolTipText(String tip)
    {
        textField.setToolTipText(tip);       
    }

    public String getText()
    {
        return textField.getText().trim();
    }

    public String getText(boolean useQuotes)
    {
        String txt = textField.getText().trim();

        if(useQuotes && !txt.equals(""))
        {
            return "\"" + txt + "\"";
        }
        else
        {
            return txt;
        }
    }


    void setText(String txt)
    {
        textField.setText(txt);
    }

    public LabeledTextField()
    {
        setLayout(new BorderLayout());

        label = new JLabel();
        label.setPreferredSize(new Dimension(labelWidth, labelHeight));

        textField = new JTextField();
        textField.setPreferredSize(new Dimension(textFieldWidth, textFieldHeight));

        add(label, BorderLayout.NORTH);
        add(textField, BorderLayout.CENTER);
    }

}
