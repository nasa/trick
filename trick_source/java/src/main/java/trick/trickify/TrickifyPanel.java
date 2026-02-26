package trick.trickify;

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import javax.swing.*;
import javax.swing.border.*; 
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import java.util.ArrayList;
import java.io.FileWriter;
import java.io.FileReader;
import java.nio.file.*;
import java.util.Properties;

import trick.common.TrickApplication;
import org.jdesktop.application.Action;

public class TrickifyPanel extends JPanel
{
    private TrickApplication myApp;
    private Properties trickProp;

    private JTabbedPane tabs;
    private JPanel inputPanel;
    private JPanel sdefinePanel;
    private JPanel outputPanel;
    private JPanel advancedPanel;
    private JPanel boxPanel;
    private JPanel buttonPanel;

    private JMenuBar menu = new JMenuBar();
    private JMenu fileMenu = new JMenu();
    private JMenuItem saveMenuItem = new JMenuItem();
    private JMenuItem loadMenuItem = new JMenuItem();
    private JMenuItem loadDefaultMenuItem = new JMenuItem();
    
    private JMenu viewMenu = new JMenu();
    private JMenuItem defaultViewMenuItem = new JMenuItem(); 

    private String trickHome;

    private int mainFrameWidth = DirSelect.textFieldWidth + DirSelect.buttonWidth + 150;
    private int mainFrameHeight = 550;

    private JPanel mainPanel;
    private DirSelect srcDirs;
    private DirSelect trickHomeDirs;
    private DirSelect buildPathDirs;
    private DirSelect trickifyPathDirs;
    private DirSelect trickifyIncludeDirs;
    private DirSelect trickifyExcludeDirs;
    private DirSelect sourceMakeDirs;
    private DirSelect sOverridesDirs;
    private LabeledTextField nameField;
    private LabeledTextField trickifyArgsField;
    private LabeledTextField sourceMakeArgsField;
    private DirSelect sDefineDirs;
    private DirSelect sDefineFilter;
    private String srcDirsConfig = "SOURCE_DIRS";
    private String trickHomeConfig = "TRICK_HOME";
    private String buildPathConfig = "BUILD_PATH";
    private String trickifyPathConfig = "TRICKIFY_PATH";
    private String trickifyIncludeConfig = "TRICKIFY_INCLUDE";
    private String trickifyExcludeConfig = "TRICKIFY_EXCLUDE";
    private String sourceMakeConfig = "SOURCE_MAKE";
    private String sOverridesConfig = "S_OVERRIDES";
    private String nameConfig = "NAME";
    private String trickifyArgsConfig = "TRICKIFY_ARGS";
    private String sourceMakeArgsConfig = "SOURCE_MAKE_ARGS";
    private String sDefineConfig = "S_DEFINE";
    private String sDefineFilterConfig = "S_DEFINE_FILTER";


    private JCheckBox fullBuildBox;
    private JCheckBox noCleanObjBox;
    private JCheckBox noCleanSrcBox;
    private JCheckBox noCleanSSourceBox;
    private JCheckBox debugBox;
    private JComboBox buildTypeBox;
    private String fullBuildConfig = "FULL_BUILD";
    private String noCleanObjConfig = "NO_CLEAN_OBJ";
    private String noCleanSrcConfig = "NO_CLEAN_SRC";
    private String noCleanSSourceConfig = "NO_CLEAN_S_SOURCE";
    private String debugConfig = "DEBUG";
    private String buildTypeConfig = "BUILD_TYPE";

    private JButton runButton;
    private JButton exportButton;

    public JMenuBar getMenuBar()
    {
        return menu;
    }

    public ArrayList<String> getTrickifyCmd(boolean useQuotes)
    {   
        ArrayList<String> cmdLine = new ArrayList<String>();
        cmdLine.add(trickHome + "/bin/trick-ify");

        String srcDirsTxt = srcDirs.getText(useQuotes);
        if(!srcDirsTxt.equals(""))
        {
            cmdLine.add("-d");
            cmdLine.add(srcDirsTxt);
        }

        String trickHomeDirsTxt = trickHomeDirs.getText(useQuotes);
        if(!trickHomeDirsTxt.equals(""))
        {
            cmdLine.add("--trick_home");
            cmdLine.add(trickHomeDirsTxt);
        }

        String trickifyPathDirsTxt = trickifyPathDirs.getText(useQuotes);
        if(!trickifyPathDirsTxt.equals(""))
        {
            cmdLine.add("--trickify_make");
            cmdLine.add(trickifyPathDirsTxt);
        }

        String trickifyIncludeDirsTxt = trickifyIncludeDirs.getText(useQuotes);
        if(!trickifyIncludeDirsTxt.equals(""))
        {
            cmdLine.add("--include");
            cmdLine.add(trickifyIncludeDirsTxt);
        }

        String trickifyExcludeDirsTxt = trickifyExcludeDirs.getText(useQuotes);
        if(!trickifyExcludeDirsTxt.equals(""))
        {
            cmdLine.add("--ex");
            cmdLine.add(trickifyExcludeDirsTxt);
        }

        String trickifyArgsFieldTxt = trickifyArgsField.getText(useQuotes);
        if(!trickifyArgsFieldTxt.equals(""))
        {
            cmdLine.add("--trickify_args");
            cmdLine.add(trickifyArgsFieldTxt);
        }

        String sourceMakeDirsTxt = sourceMakeDirs.getText(useQuotes);
        if(!sourceMakeDirsTxt.equals(""))
        {
            cmdLine.add("--source_make");
            cmdLine.add(sourceMakeDirsTxt);
        }

        String sourceMakeArgsFieldTxt = sourceMakeArgsField.getText(useQuotes);
        if(!sourceMakeArgsFieldTxt.equals(""))
        {
            cmdLine.add("--source_make_args");
            cmdLine.add(sourceMakeArgsFieldTxt);
        }

        String sDefineDirsTxt = sDefineDirs.getText(useQuotes);
        if(!sDefineDirsTxt.equals(""))
        {
            cmdLine.add("--s_define");
            cmdLine.add(sDefineDirsTxt);
        }

        String sDefineFilterTxt = sDefineFilter.getText(useQuotes);
        if(!sDefineFilterTxt.equals(""))
        {
            cmdLine.add("--s_define_filter");
            cmdLine.add(sDefineFilterTxt);
        }

        String buildPathDirsTxt = buildPathDirs.getText();
        if(!buildPathDirsTxt.equals(""))
        {
            cmdLine.add("--build");
            cmdLine.add(buildPathDirsTxt);
        }
 
        String sOverridesDirsTxt = sOverridesDirs.getText(useQuotes);
        if(!sOverridesDirsTxt.equals(""))
        {
            cmdLine.add("--s_overrides");
            cmdLine.add(sOverridesDirsTxt);
        }

        String nameFieldTxt = nameField.getText();
        if(!nameFieldTxt.equals(""))
        {
            cmdLine.add("-n");
            cmdLine.add(nameFieldTxt);
        }

        if(!(fullBuildBox.isSelected()))
        {
            cmdLine.add("--no_source");
        }

        if(noCleanObjBox.isSelected())
        {
            cmdLine.add("--no_clean_obj_list");
        }

        if(noCleanSrcBox.isSelected())
        {
            cmdLine.add("--no_clean_src_list");
        }

        if(noCleanSSourceBox.isSelected())
        {
            cmdLine.add("--no_clean_s_srouce");
        }

        if(debugBox.isSelected())
        {
            cmdLine.add("-v");
        }

        if(buildTypeBox.getSelectedItem().equals("Object"))
        {
            cmdLine.add("-b");
            cmdLine.add("o");
        }
        else if(buildTypeBox.getSelectedItem().equals("Shared"))
        {
            cmdLine.add("-b");
            cmdLine.add("so");
        }
        else if(buildTypeBox.getSelectedItem().equals("Static"))
        {
            cmdLine.add("-b");
            cmdLine.add("a");
        }

        return cmdLine;
    }

    private void trickify()
    {
        trickifyDone = false;

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

            String logDirsPath = buildPathDirs.getText();
            Path filePath = Paths.get(logDirsPath + System.getProperty("file.separator") + "trickify.log");
            //Ensure the log filepath exists
            Files.createDirectories(filePath.getParent());
            if (Files.notExists(filePath)) 
            {
                Files.createFile(filePath);
            }

            PrintWriter logfile = new PrintWriter(logDirsPath + System.getProperty("file.separator") + "trickify.log", "UTF-8");
            logfile.println(output);
            logfile.close();

            if( process.exitValue() == 0 )
                System.out.println("\u001B[32mYour technological distinctiveness has been trickified.\u001B[0m");
            else
                System.out.println("\u001B[31mBad things happened. Check your trickify log.\u001B[0m");
        }
        catch (IOException e) 
        {
            e.printStackTrace();
        }

        trickifyDone = true;
    }

    
    public void saveProperties()
    {
        saveProperties(trickProp);
    }

    public void saveProperties(Properties prop)
    {
        prop.setProperty(srcDirsConfig, srcDirs.getText());
        prop.setProperty(trickHomeConfig, trickHomeDirs.getText());
        prop.setProperty(trickifyPathConfig, trickifyPathDirs.getText());
        prop.setProperty(trickifyIncludeConfig, trickifyIncludeDirs.getText());
        prop.setProperty(trickifyExcludeConfig, trickifyExcludeDirs.getText());
        prop.setProperty(trickifyArgsConfig, trickifyArgsField.getText());
        prop.setProperty(sourceMakeConfig, sourceMakeDirs.getText());
        prop.setProperty(sourceMakeArgsConfig, sourceMakeArgsField.getText());
        prop.setProperty(sDefineConfig, sDefineDirs.getText());
        prop.setProperty(sDefineFilterConfig, sDefineFilter.getText());
        prop.setProperty(buildPathConfig, buildPathDirs.getText());
        prop.setProperty(sOverridesConfig, sOverridesDirs.getText());
        prop.setProperty(nameConfig, nameField.getText());
        prop.setProperty(fullBuildConfig, Boolean.toString(fullBuildBox.isSelected()));
        prop.setProperty(noCleanObjConfig, Boolean.toString(noCleanObjBox.isSelected()));
        prop.setProperty(noCleanSrcConfig, Boolean.toString(noCleanSrcBox.isSelected()));
        prop.setProperty(noCleanSSourceConfig, Boolean.toString(noCleanSSourceBox.isSelected()));
        prop.setProperty(debugConfig, Boolean.toString(debugBox.isSelected()));
        prop.setProperty(buildTypeConfig, buildTypeBox.getSelectedItem().toString());
    }

    public void loadProperties()
    {
        loadProperties(trickProp);
    }

    public void loadProperties(Properties prop)
    {
        String propText = null;

        if((propText = prop.getProperty(srcDirsConfig)) != null)
        {
            srcDirs.setText(propText);
        }
        propText = null;

        if((propText = prop.getProperty(trickHomeConfig)) != null)
        {
            trickHomeDirs.setText(propText);
        }
        propText = null;

        if((propText = prop.getProperty(trickifyPathConfig)) != null)
        {
            trickifyPathDirs.setText(propText);
        }
        propText = null;

        if((propText = prop.getProperty(trickifyIncludeConfig)) != null)
        {
            trickifyIncludeDirs.setText(propText);
        }
        propText = null;

        if((propText = prop.getProperty(trickifyExcludeConfig)) != null)
        {
            trickifyExcludeDirs.setText(propText);
        }
        propText = null;

        if((propText = prop.getProperty(trickifyArgsConfig)) != null)
        {
            trickifyArgsField.setText(propText);
        }
        propText = null;

        if((propText = prop.getProperty(sourceMakeConfig)) != null)
        {
            sourceMakeDirs.setText(propText);
        }
        propText = null;

        if((propText = prop.getProperty(sourceMakeArgsConfig)) != null)
        {
            sourceMakeArgsField.setText(propText);
        }
        propText = null;

        if((propText = prop.getProperty(sDefineConfig)) != null)
        {
            sDefineDirs.setText(propText);
        }
        propText = null;

        if((propText = prop.getProperty(sDefineFilterConfig)) != null)
        {
            sDefineFilter.setText(propText);
        }
        propText = null;

        if((propText = prop.getProperty(buildPathConfig)) != null)
        {
            buildPathDirs.setText(propText);
        }
        propText = null;

        if((propText = prop.getProperty(sOverridesConfig)) != null)
        {
            sOverridesDirs.setText(propText);
        }
        propText = null;

        if((propText = prop.getProperty(nameConfig)) != null)
        {
            nameField.setText(propText);
        }
        propText = null;

        if((propText = prop.getProperty(fullBuildConfig)) != null)
        {
            if(propText.equals("true"))
            {
                fullBuildBox.setSelected(true);
            }
            else
            {
                fullBuildBox.setSelected(false);
            }
        }
        propText = null;

        if((propText = prop.getProperty(noCleanObjConfig)) != null)
        {
            if(propText.equals("true"))
            {
                noCleanObjBox.setSelected(true);
            }
            else
            {
                noCleanObjBox.setSelected(false);
            }
        }
        propText = null;

        if((propText = prop.getProperty(noCleanSrcConfig)) != null)
        {
            if(propText.equals("true"))
            {
                noCleanSrcBox.setSelected(true);
            }
            else
            {
                noCleanSrcBox.setSelected(false);
            }
        }
        propText = null;

        if((propText = prop.getProperty(noCleanSSourceConfig)) != null)
        {
            if(propText.equals("true"))
            {
                noCleanSSourceBox.setSelected(true);
            }
            else
            {
                noCleanSSourceBox.setSelected(false);
            }
        }
        propText = null;

        if((propText = prop.getProperty(debugConfig)) != null)
        {
            if(propText.equals("true"))
            {
                debugBox.setSelected(true);
            }
            else
            {
                debugBox.setSelected(false);
            }
        }
        propText = null;

        if((propText = prop.getProperty(buildTypeConfig)) != null)
        {
            buildTypeBox.setSelectedItem(propText);
        }
    }

    @Action
    public void loadDefaultPropertiesAction(ActionEvent e)
    {
        setDefault();
    }

    private void setDefault()
    {
        String s = System.getProperty("file.separator");

        srcDirs.setText("");
        trickHomeDirs.setText(trickHome); 
        trickifyPathDirs.setText(trickHome + s + "share" + s + "trick" + s + "makefiles" + s); 
        trickifyIncludeDirs.setText(""); 
        trickifyExcludeDirs.setText(""); 
        trickifyArgsField.setText("");
        sourceMakeDirs.setText("");
        sourceMakeArgsField.setText("");
        sDefineDirs.setText("");
        sDefineFilter.setText("");
        sOverridesDirs.setText("");
        buildPathDirs.setText(""); 
        nameField.setText("TrickifiedLibrary");
        fullBuildBox.setSelected(true);
        noCleanObjBox.setSelected(false);
        noCleanSrcBox.setSelected(false);
        noCleanSSourceBox.setSelected(false);
        debugBox.setSelected(false);
        buildTypeBox.setSelectedItem("PLO");
    }

    @Action
    public void restoreDefaultView(ActionEvent e)
    {
        myApp.getMainFrame().setSize(mainFrameWidth, mainFrameHeight);
    }

    @Action
    public void savePropertiesAction(ActionEvent e)
    {
        JFileChooser fc = new JFileChooser();
        fc.setCurrentDirectory(new File(System.getProperty("user.dir")));
        int result = fc.showOpenDialog(null);
        if(result == JFileChooser.CANCEL_OPTION)
        {
            return;
        }
        String filename = "";
        File file = fc.getSelectedFile();
        filename = file.getAbsolutePath();
        
        Properties p = new Properties();
        saveProperties(p);
        try
        {
            p.store(new FileWriter(filename), "Trickify Properties"); 
        }    
        catch (IOException exp) 
        {
            exp.printStackTrace();
        }     
    }

    @Action
    public void loadPropertiesAction(ActionEvent e)
    {
        JFileChooser fc = new JFileChooser();
        fc.setCurrentDirectory(new File(System.getProperty("user.dir")));
        int result = fc.showOpenDialog(null);
        if(result == JFileChooser.CANCEL_OPTION)
        {
            return;
        }
        String filename = "";
        File file = fc.getSelectedFile();
        filename = file.getAbsolutePath();
        
        Properties p = new Properties();
        try
        {
            p.load(new FileReader(filename)); 
        }    
        catch (IOException exp) 
        {
            exp.printStackTrace();
        }    
        loadProperties(p);   
    }

    TrickifyPanel(TrickApplication app)
    {
        currentPanel = this;

        myApp = app;
        trickProp = myApp.trickProperties;

        String s = System.getProperty("file.separator");
        trickHome = new File(TrickifyPanel.class.getProtectionDomain().getCodeSource().getLocation().getPath()).getParentFile().getParentFile().getParentFile().getParentFile().getParentFile().getAbsolutePath();

        setLayout(new GridBagLayout());
        GridBagConstraints gbc = new GridBagConstraints();
        gbc.gridwidth = GridBagConstraints.REMAINDER;
        gbc.gridx = 0;
        gbc.gridy = 0;
        gbc.gridwidth = 1;
        gbc.gridheight = 1;
        gbc.weightx = 1.0;
        gbc.weighty = 1.0;
        gbc.fill = GridBagConstraints.BOTH;
        gbc.insets = new Insets(0, 20, 0, 20);

        tabs = new JTabbedPane();
        inputPanel = new JPanel();
        sdefinePanel = new JPanel();
        outputPanel = new JPanel();
        advancedPanel = new JPanel();
        int rows = 5;
        inputPanel.setLayout(new GridLayout(rows, 1));
        sdefinePanel.setLayout(new GridLayout(rows, 1));
        outputPanel.setLayout(new GridLayout(rows, 1));
        advancedPanel.setLayout(new GridLayout(rows, 1));
        JScrollPane[] scrollPanes = {new JScrollPane(sdefinePanel), new JScrollPane(inputPanel), new JScrollPane(outputPanel), new JScrollPane(advancedPanel)};
        for(int i = 0; i < 4; ++i)
        {
            scrollPanes[i].setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_AS_NEEDED);
            scrollPanes[i].setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);
            scrollPanes[i].getVerticalScrollBar().setUnitIncrement(16);
        }
        tabs.add("S_define", scrollPanes[0]);
        tabs.add("Inputs", scrollPanes[1]);
        tabs.add("Outputs", scrollPanes[2]);
        tabs.add("Advanced", scrollPanes[3]);
        add(tabs, gbc);

        boxPanel = new JPanel();
        boxPanel.setLayout(new GridLayout(3, 5));
        gbc.gridx = 0;
        gbc.gridy = 1;
        gbc.gridwidth = 1;
        gbc.gridheight = 1;
        gbc.weightx = 1.0;
        gbc.weighty = 0.0;
        add(boxPanel, gbc);

        buttonPanel = new JPanel();
        gbc.gridx = 0;
        gbc.gridy = 2;
        gbc.gridwidth = 1;
        gbc.gridheight = 1;
        gbc.weightx = 1.0;
        gbc.weighty = 0.0;
        add(buttonPanel, gbc);

        javax.swing.ActionMap actionMap = org.jdesktop.application.Application.getInstance().getContext().getActionMap(TrickifyPanel.class, this);

        //Build File Menu
        fileMenu.setName("trickify.fileMenu");
        saveMenuItem.setName("trickify.saveMenuItem");
        saveMenuItem.setAction(actionMap.get("savePropertiesAction"));
        fileMenu.add(saveMenuItem);
        loadMenuItem.setName("trickify.loadMenuItem");
        loadMenuItem.setAction(actionMap.get("loadPropertiesAction"));
        fileMenu.add(loadMenuItem);
        loadDefaultMenuItem.setName("trickify.loadDefaultMenuItem");
        loadDefaultMenuItem.setAction(actionMap.get("loadDefaultPropertiesAction"));
        fileMenu.add(loadDefaultMenuItem);
        menu.add(fileMenu);

        //Build View Menu
        viewMenu.setName("trickify.viewMenu");
        defaultViewMenuItem.setName("trickify.defaultViewMenuItem");
        defaultViewMenuItem.setAction(actionMap.get("restoreDefaultView"));
        viewMenu.add(defaultViewMenuItem);
        menu.add(viewMenu);

        srcDirs = new DirSelect();
        srcDirs.setLabel("Source Directories");
        srcDirs.setButtonText("Choose");
        srcDirs.allowMultiple(true);
        srcDirs.selectFile(0);
        srcDirs.setToolTipText("Directories to build trickified library from.");
        inputPanel.add(srcDirs);

        trickHomeDirs = new DirSelect();
        trickHomeDirs.setLabel("Trick Home Directory");
        trickHomeDirs.setButtonText("Choose");
        trickHomeDirs.allowMultiple(false);
        trickHomeDirs.selectFile(0);
        trickHomeDirs.setToolTipText("Trick directory to use.");
        inputPanel.add(trickHomeDirs);

        trickifyPathDirs = new DirSelect();
        trickifyPathDirs.setLabel("trickify.mk Directory");
        trickifyPathDirs.setButtonText("Choose");
        trickifyPathDirs.allowMultiple(false);
        trickifyPathDirs.selectFile(0);
        trickifyPathDirs.setToolTipText("trickify.mk to use. Defaults to your $TRICK_HOME/share/trick/makefiles/trickify.mk");
        inputPanel.add(trickifyPathDirs);

        trickifyIncludeDirs = new DirSelect();
        trickifyIncludeDirs.setLabel("Includes");
        trickifyIncludeDirs.setButtonText("Choose");
        trickifyIncludeDirs.allowMultiple(true);
        trickifyIncludeDirs.selectFile(0);
        trickifyIncludeDirs.setToolTipText("Paths to include during compilation.");
        inputPanel.add(trickifyIncludeDirs);

        trickifyExcludeDirs = new DirSelect();
        trickifyExcludeDirs.setLabel("Excludes");
        trickifyExcludeDirs.setButtonText("Choose");
        trickifyExcludeDirs.allowMultiple(true);
        trickifyExcludeDirs.selectFile(0);
        trickifyExcludeDirs.setToolTipText("Paths to exclude during trickification.");
        inputPanel.add(trickifyExcludeDirs);

        sDefineDirs = new DirSelect();
        sDefineDirs.setLabel("S_define");
        sDefineDirs.setButtonText("Choose");
        sDefineDirs.allowMultiple(false);
        sDefineDirs.selectFile(1);
        sDefineDirs.setToolTipText("Search S_define for files to build into library.");
        sdefinePanel.add(sDefineDirs);

        sDefineFilter = new DirSelect();
        sDefineFilter.setLabel("S_define Includes");
        sDefineFilter.setButtonText("Choose");
        sDefineFilter.allowMultiple(true);
        sDefineFilter.selectFile(0);
        sDefineFilter.setToolTipText("Only use files from the S_define that are under these dirs.");
        sdefinePanel.add(sDefineFilter);

        sOverridesDirs = new DirSelect();
        sOverridesDirs.setLabel("S_overrides.mk");
        sOverridesDirs.setButtonText("Choose");
        sOverridesDirs.allowMultiple(false);
        sOverridesDirs.selectFile(1);
        sOverridesDirs.setToolTipText("S_overrides to incorporate");
        sdefinePanel.add(sOverridesDirs);
        
        //spacing
        sdefinePanel.add(new JPanel());
        sdefinePanel.add(new JPanel());

        trickifyArgsField = new LabeledTextField();
        trickifyArgsField.setLabel("Trickify Args");
        trickifyArgsField.setToolTipText("Arguments to provide to trickify.mk make call");
        advancedPanel.add(trickifyArgsField);

        sourceMakeDirs = new DirSelect();
        sourceMakeDirs.setLabel("Source Make");
        sourceMakeDirs.setButtonText("Choose");
        sourceMakeDirs.allowMultiple(false);
        sourceMakeDirs.selectFile(1);
        sourceMakeDirs.setToolTipText("Make file to use for building source files. If none provdided just uses generic g++/gcc calls.");
        advancedPanel.add(sourceMakeDirs);

        sourceMakeArgsField = new LabeledTextField();
        sourceMakeArgsField.setLabel("Source Args");
        sourceMakeArgsField.setToolTipText("Arguments to provide to the above make file.");
        advancedPanel.add(sourceMakeArgsField);
        
        //spacing
        advancedPanel.add(new JPanel());
        advancedPanel.add(new JPanel());

        buildPathDirs = new DirSelect();
        buildPathDirs.setLabel("Build Path");
        buildPathDirs.setButtonText("Choose");
        buildPathDirs.allowMultiple(false);
        buildPathDirs.setToolTipText("Where to drop the library.");
        outputPanel.add(buildPathDirs);

        nameField = new LabeledTextField();
        nameField.setLabel("Library Name");
        nameField.setToolTipText("Library name (doesn't need extension).");
        outputPanel.add(nameField);
        
        //spacing
        outputPanel.add(new JPanel());
        outputPanel.add(new JPanel());
        outputPanel.add(new JPanel());

        //spacing
        boxPanel.add(new JPanel());
        fullBuildBox = new JCheckBox(" Full library build");
        fullBuildBox.setToolTipText("Full build includes swig files, icg files, and source files. Disabling excludes source files.");
        boxPanel.add(fullBuildBox);
        
        //spacing
        boxPanel.add(new JPanel());
        noCleanSSourceBox = new JCheckBox(" Don't rebuild S_source.hh");
        noCleanSSourceBox.setToolTipText("Trickify automatically generates an S_source.hh for the build process. Can disable rebuilding if you wish to use a manually created file.");
        boxPanel.add(noCleanSSourceBox);

        //spacing
        boxPanel.add(new JPanel());

        //spacing
        boxPanel.add(new JPanel());
        debugBox = new JCheckBox(" Debug info");
        boxPanel.add(debugBox);
        
        //spacing
        boxPanel.add(new JPanel());
        noCleanSrcBox = new JCheckBox(" Don't rebuild source file list");
        noCleanSrcBox.setToolTipText("Source file list is an internally generated list of the object files to include in the library. Can disable rebuilding if you wish to use a manually created file.");
        boxPanel.add(noCleanSrcBox);

        //spacing
        boxPanel.add(new JPanel());

        //spacing
        boxPanel.add(new JPanel());
        String options[] = { "Object", "Shared", "Static" };
        buildTypeBox = new JComboBox(options);
        boxPanel.add(buildTypeBox);

        //spacing
        boxPanel.add(new JPanel());
        noCleanObjBox = new JCheckBox(" Don't rebuild object file list");
        noCleanObjBox.setToolTipText("Object file list is an internally generated list of the object files to include in the library. Can disable rebuilding if you wish to use a manually created file.");
        boxPanel.add(noCleanObjBox);

        //spacing
        boxPanel.add(new JPanel());

        setDefault();

        runButton = new JButton();
        runButton.setPreferredSize(new Dimension(150, 20));
        runButton.setText("Trickify");
        runButton.addActionListener(new ActionListener() 
        {
            public void actionPerformed(ActionEvent e) 
            { 
                setEnabled(false);
                SwingWorker<Integer, Integer> worker = new SwingWorker<Integer, Integer>() 
                {
                    @Override
                    protected Integer doInBackground()
                    {
                        trickifyDone=false;
                        trickify();
                        return 0;
                    }
                };
                worker.execute();
                TrickifyLoadingBar loadbar = new TrickifyLoadingBar(currentPanel);
                            
                setEnabled(false);
            } 
        } );
        buttonPanel.add(runButton);

        exportButton = new JButton();
        exportButton.setPreferredSize(new Dimension(150, 20));
        exportButton.setText("Print");
        exportButton.addActionListener(new ActionListener() 
        { 
            public void actionPerformed(ActionEvent e) 
            { 
                System.out.println("\n" + String.join(" ", getTrickifyCmd(true)));
            } 
        } );
        buttonPanel.add(exportButton);

        setVisible(true);
    }

    public static TrickifyPanel currentPanel;
    private static volatile boolean trickifyDone = true;

    public static boolean isTrickifyDone()
    {
        return trickifyDone;
    }

    public String getBuildDir()
    {
        return buildPathDirs.getText();
    }
}
