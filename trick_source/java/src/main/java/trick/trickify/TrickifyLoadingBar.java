package trick.trickify;

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import javax.swing.*;
import javax.swing.border.*; 
import java.io.FileWriter;
import java.io.FileReader;
import java.io.File;
import java.util.Properties;
import java.util.Random;

import trick.common.TrickApplication;
import org.jdesktop.application.Action;

public class TrickifyLoadingBar
{
    private TrickifyPanel parent;
    private int trickifyStage = 0;
    private int myTimes[] = {0, 0, 0, 0, 0, 100};
    private File files[] = {null, null, null, null};
    private int maxMillisec = 100;
    private int maxStep = 10;
    private int millisec = 0;
    private int step = 0;

    void updateTrickifyStage()
    {
        for(int i = trickifyStage; i < 4; ++i)
        {
            if(files[i].exists())
            {
                trickifyStage = i+1;
                rerandomize();
                return;
            }
        }
    }

    void rerandomize()
    {
        Random rand = new Random();
        millisec = rand.nextInt(maxMillisec)+1;
        step = rand.nextInt(maxStep)+1;
    }

    TrickifyLoadingBar(TrickifyPanel panel) 
    {
        parent = panel;

        new Thread()
        {
            public void run() 
            {
                for(int i = 0; i < 4; ++i)
                {
                    files[i] = new File(panel.getBuildDir()+"/trickify_stage_"+(i+1));
                }

                Random rand = new Random();
                int maxBar = 100000;
                myTimes[0] = rand.nextInt(12)+1; // 1-95
                myTimes[1] = rand.nextInt(23-myTimes[0])+1+myTimes[0]; // [0]-96
                myTimes[2] = rand.nextInt(34-myTimes[1])+1+myTimes[1]; // [1]-97
                myTimes[3] = rand.nextInt(88-myTimes[2])+1+myTimes[2]; // [2]-98
                myTimes[4] = rand.nextInt(99-myTimes[3])+1+myTimes[3]; // [3]-99

                JFrame frame = new JFrame("Trickifying In Progress!");
                JProgressBar progressBar = new JProgressBar(0, maxBar);
                progressBar.setStringPainted(true);

                frame.add(progressBar);
                frame.setSize(300, 100);
                frame.setLocationRelativeTo(parent);
                frame.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
                frame.setVisible(true);

                int i = 0;
                while (i <= maxBar)
                {
                    if(TrickifyPanel.isTrickifyDone())
                    {
                        trickifyStage = 5;
                        millisec = 5;
                        step = maxBar / 13;
                    }
                    else
                    {
                        updateTrickifyStage();
                    }

                    if(i < (maxBar * myTimes[trickifyStage] * 0.01))
                    {
                        try 
                        {
                            Thread.sleep(millisec);
                        } 
                        catch (InterruptedException e) 
                        {
                            e.printStackTrace();
                        }

                        i += step;
                        progressBar.setValue(i);
                    }
                }
                progressBar.setValue(maxBar);
            }
        }.start();
    }
}
