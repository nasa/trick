package trick.trickify;

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import javax.swing.*;
import javax.swing.border.*; 
import java.util.ArrayList;
import java.io.FileWriter;
import java.io.FileReader;
import java.util.Properties;

import org.jdesktop.application.Action;

public class TrickifyPanel extends JPanel
{
    private JPanel fieldPanel;
    private JPanel boxPanel;
    private JPanel buttonPanel;

    private JMenuBar menu = new JMenuBar();
    private JMenu fileMenu = new JMenu();
    private JMenuItem saveMenuItem = new JMenuItem();
    private JMenuItem loadMenuItem = new JMenuItem();
    
    private JMenu viewMenu = new JMenu();
    private JMenuItem defaultViewMenuItem = new JMenuItem(); 

    private String trick_home;

    private int mainFrameWidth = 1000;
    private int mainFrameHeight = 600;

    private JPanel mainPanel;
    private DirSelect src_dirs;
    private DirSelect trick_home_dirs;
    private DirSelect build_path_dirs;
    private DirSelect trickify_path_dirs;
    private DirSelect source_make_dirs;
    private DirSelect log_dirs;
    private DirSelect s_overrides_dirs;
    private LabeledTextField name_field;
    private LabeledTextField trickify_args_field;
    private LabeledTextField source_make_args_field;
    private String src_dirs_config = "SOURCE_DIRS";
    private String trick_home_config = "TRICK_HOME";
    private String build_path_config = "BUILD_PATH";
    private String trickify_path_config = "TRICKIFY_PATH";
    private String source_make_config = "SOURCE_MAKE";
    private String log_config = "LOG";
    private String s_overrides_config = "S_OVERRIDES";
    private String name_config = "NAME";
    private String trickify_args_config = "TRICKIFY_ARGS";
    private String source_make_args_config = "SOURCE_MAKE_ARGS";
    private int fields_x = 50;
    private int fields_y = 0;
    private int fields_offset = 50;

    private JCheckBox full_build_box;
    private JCheckBox no_clean_obj_box;
    private JCheckBox no_clean_src_box;
    private JCheckBox no_clean_s_source_box;
    private JCheckBox debug_box;
    private JComboBox build_type_box;
    private String full_build_config = "FULL_BUILD";
    private String no_clean_obj_config = "NO_CLEAN_OBJ";
    private String no_clean_src_config = "NO_CLEAN_SRC";
    private String no_clean_s_source_config = "NO_CLEAN_S_SOURCE";
    private String debug_config = "DEBUG";
    private String build_type_config = "BUILD_TYPE";
    private int checkbox_x = 200;
    private int checkbox_y = 0;
    private int checkbox_width = 250;
    private int checkbox_offset = 20;

    private JButton runButton;
    private JButton exportButton;

    public JMenuBar getMenuBar()
    {
        return menu;
    }

    public ArrayList<String> getTrickifyCmd(boolean useQuotes)
    {   
        ArrayList<String> cmdLine = new ArrayList<String>();
        cmdLine.add(trick_home + "/bin/trick-ify");

        String src_dirs_txt = src_dirs.getText().trim();
        if(!src_dirs_txt.equals(""))
        {
            cmdLine.add("-d");
            if(useQuotes)
            {
                cmdLine.add("\"" + src_dirs_txt + "\"");
            }
            else
            {
                cmdLine.add(src_dirs_txt);
            }
        }

        String trick_home_dirs_txt = trick_home_dirs.getText().trim();
        if(!trick_home_dirs_txt.equals(""))
        {
            cmdLine.add("--trick_home");
            cmdLine.add(trick_home_dirs_txt);
        }

        String trickify_path_dirs_txt = trickify_path_dirs.getText().trim();
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

        String source_make_dirs_txt = source_make_dirs.getText().trim();
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
        String build_path_dirs_txt = build_path_dirs.getText().trim();
        if(!build_path_dirs_txt.equals(""))
        {
            lib_name = build_path_dirs_txt;
        }
 
        String s_overrides_dirs_txt = s_overrides_dirs.getText().trim();
        if(!s_overrides_dirs_txt.equals(""))
        {
            cmdLine.add("--s_overrides");
            cmdLine.add(s_overrides_dirs_txt);
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
            cmdLine.add("--no_clean_s_srouce");
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

    private void trickify()
    {
        ArrayList<String> cmd = getTrickifyCmd(false);
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

            String log_dirs_path = log_dirs.getText().trim();
            PrintWriter logfile = new PrintWriter(log_dirs_path + System.getProperty("file.separator") + "trickify.log", "UTF-8");
            logfile.println(output);
            logfile.close();

            System.out.println("Your technological distinctiveness has been trickified."); 
        }
        catch (IOException e) 
        {
            e.printStackTrace();
        }
    }

    public void save_properties(Properties p)
    {
        p.setProperty(src_dirs_config, src_dirs.getText().trim());
        p.setProperty(trick_home_config, trick_home_dirs.getText().trim());
        p.setProperty(trickify_path_config, trickify_path_dirs.getText().trim());
        p.setProperty(trickify_args_config, trickify_args_field.getText().trim());
        p.setProperty(source_make_config, source_make_dirs.getText().trim());
        p.setProperty(source_make_args_config, source_make_args_field.getText().trim());
        p.setProperty(build_path_config, build_path_dirs.getText().trim());
        p.setProperty(s_overrides_config, s_overrides_dirs.getText().trim());
        p.setProperty(name_config, name_field.getText().trim());
        p.setProperty(full_build_config, Boolean.toString(full_build_box.isSelected()));
        p.setProperty(no_clean_obj_config, Boolean.toString(no_clean_obj_box.isSelected()));
        p.setProperty(no_clean_src_config, Boolean.toString(no_clean_src_box.isSelected()));
        p.setProperty(no_clean_s_source_config, Boolean.toString(no_clean_s_source_box.isSelected()));
        p.setProperty(debug_config, Boolean.toString(debug_box.isSelected()));
        p.setProperty(build_type_config, build_type_box.getSelectedItem().toString());
    }

    public void load_properties(Properties p)
    {
        String prop = null;

        if((prop = p.getProperty(src_dirs_config)) != null)
        {
            src_dirs.setText(prop);
        }
        prop = null;

        if((prop = p.getProperty(trick_home_config)) != null)
        {
            trick_home_dirs.setText(prop);
        }
        prop = null;

        if((prop = p.getProperty(trickify_path_config)) != null)
        {
            trickify_path_dirs.setText(prop);
        }
        prop = null;

        if((prop = p.getProperty(trickify_args_config)) != null)
        {
            trickify_args_field.setText(prop);
        }
        prop = null;

        if((prop = p.getProperty(source_make_config)) != null)
        {
            source_make_dirs.setText(prop);
        }
        prop = null;

        if((prop = p.getProperty(source_make_args_config)) != null)
        {
            source_make_args_field.setText(prop);
        }
        prop = null;

        if((prop = p.getProperty(build_path_config)) != null)
        {
            build_path_dirs.setText(prop);
        }
        prop = null;

        if((prop = p.getProperty(s_overrides_config)) != null)
        {
            s_overrides_dirs.setText(prop);
        }
        prop = null;

        if((prop = p.getProperty(name_config)) != null)
        {
            name_field.setText(prop);
        }
        prop = null;

        if((prop = p.getProperty(full_build_config)) != null)
        {
            if(prop.equals("true"))
            {
                full_build_box.setSelected(true);
            }
            else
            {
                full_build_box.setSelected(false);
            }
        }
        prop = null;

        if((prop = p.getProperty(no_clean_obj_config)) != null)
        {
            if(prop.equals("true"))
            {
                no_clean_obj_box.setSelected(true);
            }
            else
            {
                no_clean_obj_box.setSelected(false);
            }
        }
        prop = null;

        if((prop = p.getProperty(no_clean_src_config)) != null)
        {
            if(prop.equals("true"))
            {
                no_clean_src_box.setSelected(true);
            }
            else
            {
                no_clean_src_box.setSelected(false);
            }
        }
        prop = null;

        if((prop = p.getProperty(no_clean_s_source_config)) != null)
        {
            if(prop.equals("true"))
            {
                no_clean_s_source_box.setSelected(true);
            }
            else
            {
                no_clean_s_source_box.setSelected(false);
            }
        }
        prop = null;

        if((prop = p.getProperty(debug_config)) != null)
        {
            if(prop.equals("true"))
            {
                debug_box.setSelected(true);
            }
            else
            {
                debug_box.setSelected(false);
            }
        }
        prop = null;

        if((prop = p.getProperty(build_type_config)) != null)
        {
            build_type_box.setSelectedItem(prop);
        }
    }

    private void set_default()
    {
        String s = System.getProperty("file.separator");

        src_dirs.setText("");
        trick_home_dirs.setText(trick_home); 
        trickify_path_dirs.setText(trick_home + s + "share" + s + "trick" + s + "makefiles" + s + "trickify.mk"); 
        trickify_args_field.setText("");
        source_make_dirs.setText("");
        source_make_args_field.setText("");
        s_overrides_dirs.setText("");
        build_path_dirs.setText(System.getProperty("user.dir")); 
        name_field.setText("TrickifiedLibrary");
        log_dirs.setText(System.getProperty("user.dir")); 
        full_build_box.setSelected(false);
        no_clean_obj_box.setSelected(false);
        no_clean_src_box.setSelected(false);
        no_clean_s_source_box.setSelected(false);
        debug_box.setSelected(false);
        build_type_box.setSelectedItem("PLO");
    }

    @Action
    public void restoreDefaultView(ActionEvent e)
    {
        setSize(mainFrameWidth, mainFrameHeight);
    }

    TrickifyPanel()
    {

        String s = System.getProperty("file.separator");
        trick_home = new File(TrickifyPanel.class.getProtectionDomain().getCodeSource().getLocation().getPath()).getParentFile().getParentFile().getParentFile().getParentFile().getParentFile().getAbsolutePath();

        setBounds(0, 0, mainFrameWidth, mainFrameHeight);
        setPreferredSize(new Dimension(mainFrameWidth, mainFrameHeight));
        setBackground(Color.lightGray);
        setLayout(new GridBagLayout());
        GridBagConstraints gbc = new GridBagConstraints();
        gbc.gridwidth = GridBagConstraints.REMAINDER;

        fieldPanel = new JPanel();
        fieldPanel.setPreferredSize(new Dimension(mainFrameWidth, 400));
        fieldPanel.setLayout(new GridLayout(10, 1));
        add(fieldPanel, gbc);

        boxPanel = new JPanel();
        boxPanel.setLayout(new GridLayout(3, 2));
        add(boxPanel, gbc);

        buttonPanel = new JPanel();
        add(buttonPanel, gbc);

        //Build File Menu
        fileMenu.setName("trickify.fileMenu");
        saveMenuItem.setName("trickify.saveMenuItem");
        fileMenu.add(saveMenuItem);
        loadMenuItem.setName("trickify.loadMenuItem");
        fileMenu.add(loadMenuItem);
        menu.add(fileMenu);

        //Build View Menu
        viewMenu.setName("trickify.viewMenu");
        defaultViewMenuItem.setName("trickify.defaultViewMenuItem");
        viewMenu.add(defaultViewMenuItem);
        menu.add(viewMenu);

        int fields_relative_offset = fields_y;

        src_dirs = new DirSelect();
        src_dirs.setLabel("Source Directories");
        src_dirs.setButtonText("Choose");
        fields_relative_offset += fields_offset;
        src_dirs.allowMultiple(true);
        src_dirs.setToolTipText("Directories to build trickified library from.");
        fieldPanel.add(src_dirs);

        trick_home_dirs = new DirSelect();
        trick_home_dirs.setLabel("Trick Home Directory");
        trick_home_dirs.setButtonText("Choose");
        fields_relative_offset += fields_offset;
        trick_home_dirs.allowMultiple(false);
        trick_home_dirs.setToolTipText("Trick directory to use.");
        fieldPanel.add(trick_home_dirs);

        trickify_path_dirs = new DirSelect();
        trickify_path_dirs.setLabel("Trickify Makefile");
        trickify_path_dirs.setButtonText("Choose");
        fields_relative_offset += fields_offset;
        trickify_path_dirs.allowMultiple(false);
        trickify_path_dirs.selectFile(true);
        trickify_path_dirs.setToolTipText("trickify.mk to use. Defaults to your $TRICK_HOME/share/trick/makefiles/trickify.mk");
        fieldPanel.add(trickify_path_dirs);

        trickify_args_field = new LabeledTextField();
        trickify_args_field.setLabel("Trickify Args");
        fields_relative_offset += fields_offset;
        trickify_args_field.setToolTipText("Arguments to provide to trickify.mk make call");
        fieldPanel.add(trickify_args_field);

        source_make_dirs = new DirSelect();
        source_make_dirs.setLabel("Source Make");
        source_make_dirs.setButtonText("Choose");
        fields_relative_offset += fields_offset;
        source_make_dirs.allowMultiple(false);
        source_make_dirs.selectFile(true);
        source_make_dirs.setToolTipText("Make file to use for building source files. If none provdided just uses generic g++/gcc calls.");
        fieldPanel.add(source_make_dirs);

        source_make_args_field = new LabeledTextField();
        source_make_args_field.setLabel("Source Args");
        fields_relative_offset += fields_offset;
        source_make_args_field.setToolTipText("Arguments to provide to the above make file.");
        fieldPanel.add(source_make_args_field);

        s_overrides_dirs = new DirSelect();
        s_overrides_dirs.setLabel("S_overrides");
        s_overrides_dirs.setButtonText("Choose");
        fields_relative_offset += fields_offset;
        s_overrides_dirs.allowMultiple(false);
        s_overrides_dirs.selectFile(true);
        s_overrides_dirs.setToolTipText("S_overrides to incorporate");
        fieldPanel.add(s_overrides_dirs);

        build_path_dirs = new DirSelect();
        build_path_dirs.setLabel("Build Path");
        build_path_dirs.setButtonText("Choose");
        fields_relative_offset += fields_offset;
        build_path_dirs.allowMultiple(false);
        build_path_dirs.setToolTipText("Where to drop the library.");
        fieldPanel.add(build_path_dirs);

        name_field = new LabeledTextField();
        name_field.setLabel("Library Name");
        fields_relative_offset += fields_offset;
        name_field.setToolTipText("Library name (doesn't need extension).");
        fieldPanel.add(name_field);

        log_dirs = new DirSelect();
        log_dirs.setLabel("Trickify Log");
        log_dirs.setButtonText("Choose");
        fields_relative_offset += fields_offset;
        log_dirs.allowMultiple(false);
        log_dirs.selectFile(false);
        log_dirs.setToolTipText("Where to drop the log file.");
        fieldPanel.add(log_dirs);

        int checkbox_relative_offset = 0;
        checkbox_y = fields_relative_offset;

        full_build_box = new JCheckBox(" Full library build");
        full_build_box.setBounds(checkbox_x, checkbox_y + checkbox_relative_offset, checkbox_width, checkbox_offset);
        full_build_box.setBackground(getBackground());
        full_build_box.setToolTipText("Full build includes swig files, icg files, and source files. Disabling excludes source files.");
        boxPanel.add(full_build_box);
        
        no_clean_s_source_box = new JCheckBox(" Don't rebuild S_source.hh");
        no_clean_s_source_box.setBounds(checkbox_x, checkbox_y + checkbox_relative_offset, checkbox_width, checkbox_offset);
        no_clean_s_source_box.setBackground(getBackground());
        no_clean_s_source_box.setToolTipText("Trickify automatically generates an S_source.hh for the build process. Can disable rebuilding if you wish to use a manually created file.");
        boxPanel.add(no_clean_s_source_box);

        debug_box = new JCheckBox(" Debug info");
        debug_box.setBounds(checkbox_x, checkbox_y + checkbox_relative_offset, checkbox_width, checkbox_offset);
        debug_box.setBackground(getBackground());
        boxPanel.add(debug_box);
        
        no_clean_src_box = new JCheckBox(" Don't rebuild source file list");
        no_clean_src_box.setBounds(checkbox_x, checkbox_y + checkbox_relative_offset, checkbox_width, checkbox_offset);
        no_clean_src_box.setBackground(getBackground());
        no_clean_src_box.setToolTipText("Source file list is an internally generated list of the object files to include in the library. Can disable rebuilding if you wish to use a manually created file.");
        boxPanel.add(no_clean_src_box);

        String options[] = { "Object", "Shared", "Static" };
        build_type_box = new JComboBox(options);
        build_type_box.setBounds(checkbox_x, checkbox_y + checkbox_relative_offset, 200, 20);
        boxPanel.add(build_type_box);

        no_clean_obj_box = new JCheckBox(" Don't rebuild object file list");
        no_clean_obj_box.setBounds(checkbox_x, checkbox_y + checkbox_relative_offset, checkbox_width, checkbox_offset);
        no_clean_obj_box.setBackground(getBackground());
        no_clean_obj_box.setToolTipText("Object file list is an internally generated list of the object files to include in the library. Can disable rebuilding if you wish to use a manually created file.");
        boxPanel.add(no_clean_obj_box);

        set_default();

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
        buttonPanel.add(runButton);

        exportButton = new JButton();
        exportButton.setBounds(600, mainFrameHeight-30, 150, 20);
        exportButton.setText("Print");
        exportButton.addActionListener(new ActionListener() 
        { 
            public void actionPerformed(ActionEvent e) 
            { 
                System.out.println(String.join(" ", getTrickifyCmd(true)));
            } 
        } );
        buttonPanel.add(exportButton);

        setVisible(true);
    }
}
