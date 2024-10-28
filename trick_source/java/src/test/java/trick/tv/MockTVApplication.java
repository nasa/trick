package trick.tv;


import org.jdesktop.application.Application;


public class MockTVApplication extends TVApplication {

    @Override
    protected void initialize(String[] args) {
        for(int i = 0; i < args.length; i++)
            System.out.println((i+1) + ": " + args[i]);
        super.initialize(args);
    }
    public static void main(String[] args) {
        Application.launch(MockTVApplication.class, args);
	}
}