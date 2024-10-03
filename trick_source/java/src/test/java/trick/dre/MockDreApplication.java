package trick.dre;


import java.io.File;

import org.jdesktop.application.Application;

import trick.common.ApplicationTest;
import trick.common.TestUtils;


public class MockDreApplication extends DreApplication {
	
    public static void main(String[] args) {
		File sie;

		sieResourcePath = TestUtils.getTrickHome() + "/test/SIM_test_dr/S_sie.resource";
		sie = new File(sieResourcePath);

		if (!sie.exists()) {
			boolean success = TestUtils.compileTestSim("SIM_test_dr");
		}
		
        Application.launch(MockDreApplication.class, args);
    }
}