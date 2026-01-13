package trick.trickify;

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import javax.swing.*;

public class DirSelect extends LabeledTextField
{
    private JButton button;
    public final static int buttonWidth = 100;
    public final static int buttonHeight = 20;

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

    public void selectFile(int b)
    {
        switch (b)
        {
            case 0:
                browser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
                break;
            case 1:
                browser.setFileSelectionMode(JFileChooser.FILES_ONLY);
                break;
            case 2:
                browser.setFileSelectionMode(JFileChooser.FILES_AND_DIRECTORIES);
                break;
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
                String fileName = "";
                if(multiDir)
                {
                    File[] files = browser.getSelectedFiles();
                    for(int i = 0; i < files.length; ++i)
                    {
                        fileName += files[i].getAbsolutePath() + " ";
                    }
                    textField.setText(textField.getText().trim() + " " + fileName.trim());
                }
                else
                {
                    File file = browser.getSelectedFile();
                    fileName = file.getAbsolutePath();
                    textField.setText(fileName);
                } 
            } 
        } );

        add(button, BorderLayout.EAST);
    }

}
