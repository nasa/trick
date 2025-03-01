package trick.trickify;

import java.awt.event.*;
import java.io.*;
import javax.swing.*;

public class DirSelect extends JPanel
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

    private JButton button;
    private int button_pos_x = textfield_pos_x + textfield_width;
    private int button_pos_y = textfield_pos_y;
    private int button_width = 100;
    private int button_height = 20;

    private JFileChooser browser;

    private boolean multiDir = false;

    public void setLabel(String text)
    {
        label.setText(text);
    }

    public void setToolTipText(String tip)
    {
        textfield.setToolTipText(tip);       
    }

    public void setButtonText(String text)
    {
        button.setText(text);
    }

    public void allowMultiple(boolean b)
    {
        browser.setMultiSelectionEnabled(b);
        multiDir = b;
    }

    public void selectFile(boolean b)
    {
        if(b)
        {
            browser.setFileSelectionMode(JFileChooser.FILES_ONLY);
        }
        else
        {
            browser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
        }
    }

    public String getText()
    {
        return textfield.getText();
    }

    void setText(String txt)
    {
        textfield.setText(txt);
    }

    public DirSelect()
    {
        label = new JLabel();
        label_width = 200;
        label_height = 20;

        textfield = new JTextField();
        textfield_width = 800;
        textfield_height = 20;

        button = new JButton();
        button_width = 100;
        button_height = 20;

        browser = new JFileChooser();
        browser.setCurrentDirectory(new File(System.getProperty("user.dir")));
        browser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);

        button.addActionListener(new ActionListener() 
        { 
            public void actionPerformed(ActionEvent e) 
            { 
                int result = browser.showOpenDialog(null);
                if(result == JFileChooser.CANCEL_OPTION)
                {
                    return;
                }
                String filename = "";
                if(multiDir)
                {
                    File[] files = browser.getSelectedFiles();
                    for(int i = 0; i < files.length; ++i)
                    {
                        filename += files[i].getAbsolutePath() + " ";
                    }
                    textfield.setText(textfield.getText() + filename);
                }
                else
                {
                    File file = browser.getSelectedFile();
                    filename = file.getAbsolutePath();
                    textfield.setText(filename);
                } 
            } 
        } );

        
        add(label);
        add(textfield);
        add(button);
    }

}
