package trick.tv;

import java.io.IOException;
import java.net.ServerSocket;

import org.jdesktop.application.Application;
import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import trick.common.ApplicationTest;
import trick.common.CheckApplicationProperties;
import trick.dre.MockDreApplication;
import trick.dre.fixtures.DreFixture;

import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assumptions.assumeThat;
import static org.assertj.swing.launcher.ApplicationLauncher.application;
import static org.junit.Assert.assertTrue;

/**
 * 
 * Test TVApplication life cycle.
 * 
 * @author hchen
 *
 */
public class TVApplicationTest extends ApplicationTest {
	private static final int PORT = getOpenPort();

    @BeforeClass
    public static void onSetUpBeforeClass() {
		System.out.println("Port: " + PORT);

    }

	public static int getOpenPort() {
		int port = -1;
		try {
			ServerSocket temp = new ServerSocket(0);
			port = temp.getLocalPort();
			temp.close();
		} catch(IOException e) {
			System.out.println("Free Port Not Found");
		}

		return port;
	}

    @Override
    protected void onSetUp() {       
        application(MockTVApplication.class).start();

        sleep(1500);
    }

	@Test
	public void testGeneric() {
		sleep(5000);
		assertThat(true).isTrue();
	}
}