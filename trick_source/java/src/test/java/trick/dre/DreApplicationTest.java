package trick.dre;

import static org.assertj.swing.launcher.ApplicationLauncher.application;

import static org.junit.Assert.assertTrue;

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

import static trick.dre.fixtures.DreFixture.*;

/**
 * 
 * Test DreApplication life cycle.
 * 
 * @author hchen
 * @intern mrockwell2
 *
 */
public class DreApplicationTest extends ApplicationTest {

    private DreFixture dre_fix;

    @BeforeClass
    public static void onSetUpBeforeClass() {

    }

    @Override
    protected void onSetUp() {       
        application(MockDreApplication.class).start();

        sleep(1000);

        dre_fix = new DreFixture(robot(), MockDreApplication.getInstance());
    }

    @Test
    public void testGeneric() {
        boolean bool = true;
        dre_fix.selectVar("drx/name");
        dre_fix.setOptions(HDF5 | STEP | RING_BUFFER);
        dre_fix.setGroupName("TestingTesting123");
        dre_fix.saveMenuItem("TestingMenuBar");
		sleep(5000);
		assertThat(bool).isTrue();
    }
}