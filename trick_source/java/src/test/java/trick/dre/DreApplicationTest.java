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

import static org.assertj.core.api.Assertions.assertThat;

/**
 * 
 * Test DreApplication life cycle.
 * 
 * @author hchen
 * @intern mrockwell2
 *
 */
public class DreApplicationTest extends ApplicationTest {

    @BeforeClass
    public static void onSetUpBeforeClass() {

    }

    @Override
    protected void onSetUp() {       
        application(MockDreApplication.class).start();
    }

    @Test
    public void testGeneric() {
        boolean bool = true;
		sleep(5000);
		assertThat(bool).isTrue();
    }
}