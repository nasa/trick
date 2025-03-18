package trick.trickify;

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import javax.swing.*;

public class DirSelect extends LabeledTextField
{
    private JButton button;
    public final static int button_width = 100;
    public final static int button_height = 20;

    private JFileChooser browser;

    private boolean multiDir = false;

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

    public DirSelect()
    {
        super();
        
        button = new JButton();

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
                    textfield.setText(textfield.getText().trim() + " " + filename.trim());
                }
                else
                {
                    File file = browser.getSelectedFile();
                    filename = file.getAbsolutePath();
                    textfield.setText(filename);
                } 
            } 
        } );

        add(button, BorderLayout.EAST);
    }

}
