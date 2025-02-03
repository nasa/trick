package trick.trickify;

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import javax.swing.*;
import javax.swing.border.*; 
import java.util.ArrayList;

public class TrickifyFrame 
{
    private String defaultDirectory;

    private JFrame mainFrame;
    private int mainFrameWidth = 1000;
    private int mainFrameHeight = 600;

    private JPanel mainPanel;
    private DirSelect src_dirs;
    private DirSelect trick_home_dirs;
    private DirSelect build_path_dirs;
    private DirSelect trickify_path_dirs;
    private DirSelect source_make_dirs;
    private DirSelect log_dirs;
    private LabeledTextField name_field;
    private LabeledTextField trickify_args_field;
    private LabeledTextField source_make_args_field;
    private int fields_x = 50;
    private int fields_y = 0;
    private int fields_offset = 50;

    private JCheckBox full_build_box;
    private JCheckBox no_clean_obj_box;
    private JCheckBox no_clean_src_box;
    private JCheckBox no_clean_s_source_box;
    private JCheckBox debug_box;
    private JComboBox build_type_box;
    private int checkbox_x = 200;
    private int checkbox_y = 0;
    private int checkbox_width = 250;
    private int checkbox_offset = 20;

    private JButton runButton;
    private JButton exportButton;

    public ArrayList<String> getTrickifyCmd()
    {   
        ArrayList<String> cmdLine = new ArrayList<String>();
        cmdLine.add("trick-ify");

        String src_dirs_txt = src_dirs.getDirs().trim();
        if(!src_dirs_txt.equals(""))
        {
            cmdLine.add("-d");
            cmdLine.add(src_dirs_txt);
        }

        String trick_home_dirs_txt = trick_home_dirs.getDirs().trim();
        if(!trick_home_dirs_txt.equals(""))
        {
            cmdLine.add("--trick_home");
            cmdLine.add(trick_home_dirs_txt);
        }

        String trickify_path_dirs_txt = trickify_path_dirs.getDirs().trim();
        if(!trickify_path_dirs_txt.equals(""))
        {
            cmdLine.add("--trickify_make");
            cmdLine.add(trickify_path_dirs_txt);
        }

        String trickify_args_field_txt = trickify_args_field.getText().trim();
        if(!trickify_args_field_txt.equals(""))
        {
            cmdLine.add("--trickify_args");
            cmdLine.add(trickify_args_field_txt);
        }

        String source_make_dirs_txt = source_make_dirs.getDirs().trim();
        if(!source_make_dirs_txt.equals(""))
        {
            cmdLine.add("--source_make");
            cmdLine.add(source_make_dirs_txt);
        }

        String source_make_args_field_txt = source_make_args_field.getText().trim();
        if(!source_make_args_field_txt.equals(""))
        {
            cmdLine.add("--source_make_args");
            cmdLine.add(source_make_args_field_txt);
        }

        String lib_name = "";
        String build_path_dirs_txt = build_path_dirs.getDirs().trim();
        if(!build_path_dirs_txt.equals(""))
        {
            lib_name = build_path_dirs_txt;
        }

        String name_field_txt = name_field.getText().trim();
        if(!name_field_txt.equals(""))
        {
            if(!lib_name.equals(""))
            {
                lib_name += System.getProperty("file.separator") + name_field_txt;
            }
            else
            {
                lib_name = name_field_txt;
            }
            cmdLine.add("-n");
            cmdLine.add(lib_name);
        }

        if(!(full_build_box.isSelected()))
        {
            cmdLine.add("--no_source");
        }

        if(no_clean_obj_box.isSelected())
        {
            cmdLine.add("--no_clean_obj_list");
        }

        if(no_clean_src_box.isSelected())
        {
            cmdLine.add("--no_clean_src_list");
        }

        if(no_clean_s_source_box.isSelected())
        {
            cmdLine.add("--no_clean_src_list");
        }

        if(debug_box.isSelected())
        {
            cmdLine.add("-v");
        }

        if(build_type_box.getSelectedItem().equals("Object"))
        {
            cmdLine.add("-b");
            cmdLine.add("o");
        }
        else if(build_type_box.getSelectedItem().equals("Shared"))
        {
            cmdLine.add("-b");
            cmdLine.add("so");
        }
        else if(build_type_box.getSelectedItem().equals("Static"))
        {
            cmdLine.add("-b");
            cmdLine.add("a");
        }

        return cmdLine;
    }

    private void update_checkbox_pos()
    {
        if(checkbox_x >= mainFrameWidth/2)
        {
            checkbox_y += checkbox_offset;
        }
        checkbox_x = mainFrameWidth - checkbox_width - checkbox_x;
    }

    void trickify()
    {
        ArrayList<String> cmd = getTrickifyCmd();
        String[] cmdLine = new String[cmd.size()];
        cmdLine = cmd.toArray(cmdLine);
        System.out.println("Executing: " + String.join(" ", cmd));
        try
        {
            Process process = Runtime.getRuntime().exec(cmdLine);
            StringBuilder output = new StringBuilder();
            BufferedReader reader = new BufferedReader(new InputStreamReader(process.getInputStream()));
            String line;
            while ((line = reader.readLine()) != null) 
            {
                output.append(line + "\n");
            }

            String log_dirs_path = log_dirs.getDirs().trim();
            PrintWriter logfile = new PrintWriter(log_dirs_path + System.getProperty("file.separator") + "trickify.log", "UTF-8");
            logfile.println(output);
            logfile.close();
            
        }
        catch (IOException e) 
        {
            e.printStackTrace();
        }
    }

    TrickifyFrame()
    {
        String s = System.getProperty("file.separator");

        mainFrame = new JFrame();

        mainPanel = new JPanel();
        mainFrame.setTitle("Trickify");
        mainPanel.setBounds(0, 0, mainFrameWidth, mainFrameHeight);
        mainPanel.setPreferredSize(new Dimension(mainFrameWidth, mainFrameHeight));
        mainPanel.setBackground(Color.lightGray);
        mainPanel.setLayout(null);

        int fields_relative_offset = fields_y;

        src_dirs = new DirSelect();
        src_dirs.setLabel("Source Directories");
        src_dirs.setButtonText("Choose");
        src_dirs.setPosition(fields_x, fields_relative_offset);
        fields_relative_offset += fields_offset;
        src_dirs.allowMultiple(true);
        src_dirs.setToolTipText("Directories to build trickified library from.");
        src_dirs.addToPanel(mainPanel);

        trick_home_dirs = new DirSelect();
        trick_home_dirs.setLabel("Trick Home Directory");
        trick_home_dirs.setDirs(System.getenv("TRICK_HOME")); 
        trick_home_dirs.setButtonText("Choose");
        trick_home_dirs.setPosition(fields_x, fields_relative_offset);
        fields_relative_offset += fields_offset;
        trick_home_dirs.allowMultiple(false);
        trick_home_dirs.setToolTipText("Trick directory to use.");
        trick_home_dirs.addToPanel(mainPanel);

        trickify_path_dirs = new DirSelect();
        trickify_path_dirs.setLabel("Trickify Makefile");
        trickify_path_dirs.setDirs(System.getenv("TRICK_HOME") + s + "share" + s + "trick" + s + "makefiles" + s + "trickify.mk"); 
        trickify_path_dirs.setButtonText("Choose");
        trickify_path_dirs.setPosition(fields_x, fields_relative_offset);
        fields_relative_offset += fields_offset;
        trickify_path_dirs.allowMultiple(false);
        trickify_path_dirs.selectFile(true);
        trickify_path_dirs.setToolTipText("trickify.mk to use. Defaults to your $TRICK_HOME/share/trick/makefiles/trickify.mk");
        trickify_path_dirs.addToPanel(mainPanel);

        trickify_args_field = new LabeledTextField();
        trickify_args_field.setLabel("Trickify Args");
        trickify_args_field.setPosition(fields_x, fields_relative_offset);
        fields_relative_offset += fields_offset;
        trickify_args_field.setToolTipText("Arguments to provide to trickify.mk make call");
        trickify_args_field.addToPanel(mainPanel);

        source_make_dirs = new DirSelect();
        source_make_dirs.setLabel("Source Make");
        source_make_dirs.setButtonText("Choose");
        source_make_dirs.setPosition(fields_x, fields_relative_offset);
        fields_relative_offset += fields_offset;
        source_make_dirs.allowMultiple(false);
        source_make_dirs.selectFile(true);
        source_make_dirs.setToolTipText("Make file to use for building source files. If none provdided just uses generic g++/gcc calls.");
        source_make_dirs.addToPanel(mainPanel);

        source_make_args_field = new LabeledTextField();
        source_make_args_field.setLabel("Source Args");
        source_make_args_field.setPosition(fields_x, fields_relative_offset);
        fields_relative_offset += fields_offset;
        source_make_args_field.setToolTipText("Arguments to provide to the above make file.");
        source_make_args_field.addToPanel(mainPanel);

        build_path_dirs = new DirSelect();
        build_path_dirs.setLabel("Build Path");
        build_path_dirs.setDirs(System.getProperty("user.dir")); 
        build_path_dirs.setButtonText("Choose");
        build_path_dirs.setPosition(fields_x, fields_relative_offset);
        fields_relative_offset += fields_offset;
        build_path_dirs.allowMultiple(false);
        build_path_dirs.setToolTipText("Where to drop the library.");
        build_path_dirs.addToPanel(mainPanel);

        name_field = new LabeledTextField();
        name_field.setLabel("Library Name");
        name_field.setText("TrickifiedLibrary");
        name_field.setPosition(fields_x, fields_relative_offset);
        fields_relative_offset += fields_offset;
        name_field.setToolTipText("Library name (doesn't need extension).");
        name_field.addToPanel(mainPanel);

        log_dirs = new DirSelect();
        log_dirs.setLabel("Trickify Log");
        log_dirs.setDirs(System.getProperty("user.dir")); 
        log_dirs.setButtonText("Choose");
        log_dirs.setPosition(fields_x, fields_relative_offset);
        fields_relative_offset += fields_offset;
        log_dirs.allowMultiple(false);
        trickify_path_dirs.selectFile(false);
        log_dirs.setToolTipText("Where to drop the log file.");
        log_dirs.addToPanel(mainPanel);

        int checkbox_relative_offset = 0;
        checkbox_y = fields_relative_offset;

        full_build_box = new JCheckBox(" Full library build");
        full_build_box.setBounds(checkbox_x, checkbox_y + checkbox_relative_offset, checkbox_width, checkbox_offset);
        full_build_box.setBackground(mainPanel.getBackground());
        full_build_box.setToolTipText("Full build includes swig files, icg files, and source files. Disabling excludes source files.");
        full_build_box.setSelected(true);
        update_checkbox_pos();
        mainPanel.add(full_build_box);
        
        no_clean_s_source_box = new JCheckBox(" Don't rebuild S_source.hh");
        no_clean_s_source_box.setBounds(checkbox_x, checkbox_y + checkbox_relative_offset, checkbox_width, checkbox_offset);
        no_clean_s_source_box.setBackground(mainPanel.getBackground());
        no_clean_s_source_box.setToolTipText("Trickify automatically generates an S_source.hh for the build process. Can disable rebuilding if you wish to use a manually created file.");
        update_checkbox_pos();
        mainPanel.add(no_clean_s_source_box);

        debug_box = new JCheckBox(" Debug info");
        debug_box.setBounds(checkbox_x, checkbox_y + checkbox_relative_offset, checkbox_width, checkbox_offset);
        debug_box.setBackground(mainPanel.getBackground());
        update_checkbox_pos();
        mainPanel.add(debug_box);
        
        no_clean_src_box = new JCheckBox(" Don't rebuild source file list");
        no_clean_src_box.setBounds(checkbox_x, checkbox_y + checkbox_relative_offset, checkbox_width, checkbox_offset);
        no_clean_src_box.setBackground(mainPanel.getBackground());
        no_clean_src_box.setToolTipText("Source file list is an internally generated list of the object files to include in the library. Can disable rebuilding if you wish to use a manually created file.");
        update_checkbox_pos();
        mainPanel.add(no_clean_src_box);

        String options[] = { "Object", "Shared", "Static" };
        build_type_box = new JComboBox(options);
        build_type_box.setBounds(checkbox_x, checkbox_y + checkbox_relative_offset, 200, 20);
        update_checkbox_pos();
        mainPanel.add(build_type_box);

        no_clean_obj_box = new JCheckBox(" Don't rebuild object file list");
        no_clean_obj_box.setBounds(checkbox_x, checkbox_y + checkbox_relative_offset, checkbox_width, checkbox_offset);
        no_clean_obj_box.setBackground(mainPanel.getBackground());
        no_clean_obj_box.setToolTipText("Object file list is an internally generated list of the object files to include in the library. Can disable rebuilding if you wish to use a manually created file.");
        update_checkbox_pos();
        mainPanel.add(no_clean_obj_box);

        runButton = new JButton();
        runButton.setBounds(250, mainFrameHeight-30, 150, 20);
        runButton.setText("Trickify");
        runButton.addActionListener(new ActionListener() 
        {
            public void actionPerformed(ActionEvent e) 
            { 
                if(System.getProperty("user.name").charAt(0) == 'j')
                {
                    String pw = "Java is inferior to Perl";
                    String err = "Error! Try Again";

                    JFrame pwFrame = new JFrame();
                    pwFrame.setTitle("Enter Password");
                    JPanel pwPanel = new JPanel();

                    JLabel errorLabel = new JLabel();
                    errorLabel.setForeground(Color.RED);
                    errorLabel.setText(err);
                    errorLabel.setVisible(false);
                    pwPanel.add(errorLabel);

                    JTextField pwField = new JTextField();
                    pwField.setPreferredSize(new Dimension(300, 20));
                    pwField.setMaximumSize(pwField.getPreferredSize());
                    pwPanel.add(pwField);

                    JButton pwButton = new JButton();
                    pwButton.setText("Enter");
                    pwField.setMaximumSize(pwField.getPreferredSize());
                    pwButton.addActionListener(new ActionListener() 
                    {
                        public void actionPerformed(ActionEvent e) 
                        { 
                            String txt = pwField.getText().trim();
                            if(txt.equals(pw))  
                            {
                                pwFrame.dispose();
                                trickify();
                            }
                            else
                            {
                                errorLabel.setText(errorLabel.getText() + "!");
                                errorLabel.setVisible(true);
                            }
                        } 
                    } );
                    pwPanel.add(pwButton);
                    
                    pwFrame.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
                    pwFrame.setResizable(false);
                    pwPanel.setLayout(new GridLayout(3, 1));
                    pwFrame.add(pwPanel);
                    pwFrame.pack();
                    pwFrame.setVisible(true);
                }
                else
                {
                    trickify();
                }
            } 
        } );
        mainPanel.add(runButton);

        exportButton = new JButton();
        exportButton.setBounds(600, mainFrameHeight-30, 150, 20);
        exportButton.setText("Export");
        exportButton.addActionListener(new ActionListener() 
        { 
            public void actionPerformed(ActionEvent e) 
            { 
                System.out.println(String.join(" ", getTrickifyCmd()));
            } 
        } );
        mainPanel.add(exportButton);

        mainFrame.add(mainPanel);
        mainFrame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        mainFrame.pack();
        mainFrame.setResizable(false);
        mainFrame.setVisible(true);
    }
}
