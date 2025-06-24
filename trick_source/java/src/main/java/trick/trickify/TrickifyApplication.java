package trick.trickify;

import javax.swing.*;
import org.jdesktop.application.View;
import trick.common.TrickApplication;

public class TrickifyApplication extends TrickApplication
{
    private TrickifyPanel panel;

    public static void main(String[] args) 
    {
        launch(TrickifyApplication.class, args);
    }

    @Override
    protected JComponent createMainPanel()
    {
        if(panel == null)
        {
            panel = new TrickifyPanel(this);
        }
        return panel;
    }

    @Override
    protected void initialize(String[] args) 
    {
        super.initialize(args);
        trickProperties.setProperty("Application.title", "Trickify");
    }

    @Override
    protected void startup() 
    {
        super.startup();
        createMainPanel();

        panel.loadProperties();

        // Build the GUI.
        View view = getMainView();
        view.setComponent(panel);
        view.setMenuBar(panel.getMenuBar());

        // Display the GUI.
        show(view);
    }

    @Override
    protected void shutdown() 
    {
        panel.saveProperties();   

        super.shutdown();
    }
}
