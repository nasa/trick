package trick.dre;

import org.jdesktop.application.Application;
import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import trick.common.ApplicationTest;
import trick.common.CheckApplicationProperties;
import trick.dre.fixtures.DreFixture;
import trick.dre.MockDreApplication;

import static org.assertj.core.api.Assertions.assertThat;
import static org.assertj.core.api.Assumptions.assumeThat;
import static org.assertj.swing.launcher.ApplicationLauncher.application;
import static org.junit.Assert.assertTrue;
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

    // @Test
    // public void testGeneric() {
    //     boolean bool = true;
    //     dre_fix.selectVar("drx/name");
    //     dre_fix.setOptions(HDF5 | STEP | RING_BUFFER);
    //     dre_fix.setGroupName("TestingTesting123");
    //     dre_fix.saveMenuItem("TestingMenuBar");
	// 	sleep(5000);
	// 	assertThat(bool).isTrue();
    // }

    @Test
    public void testFormatOptions() {
        // ARRANGE
        final int BINARY_EXP = BINARY | ALWAYS | BUFFER | SINGLE_PREC_OFF, // Should also be the initial options at startup
                  ASCII_EXP  = ASCII  | ALWAYS | BUFFER | SINGLE_PREC_OFF,
                  HDF5_EXP   = HDF5   | ALWAYS | BUFFER | SINGLE_PREC_OFF;

        int init_opts = dre_fix.getSelectedOptions(),
            binary_sel, ascii_sel, hdf5_sel;

        assumeThat(init_opts).withFailMessage("Unexpected options are selected at startup")
                             .isEqualTo(BINARY_EXP);
        
        // ACT
        dre_fix.setOptions(ASCII);
        ascii_sel = dre_fix.getSelectedOptions();

        dre_fix.setOptions(HDF5);
        hdf5_sel = dre_fix.getSelectedOptions();

        dre_fix.setOptions(BINARY);
        binary_sel = dre_fix.getSelectedOptions();

        // ASSERT
        assertThat(binary_sel).isEqualTo(BINARY_EXP);
        assertThat(ascii_sel).isEqualTo(ASCII_EXP);
        assertThat(hdf5_sel).isEqualTo(HDF5_EXP);
    }

    @Test
    public void testFrequencyOptions() {
        // ARRANGE
        final int ALWAYS_EXP  = BINARY | ALWAYS  | BUFFER | SINGLE_PREC_OFF, // Should also be the initial options at startup
                  CHANGES_EXP = BINARY | CHANGES | BUFFER | SINGLE_PREC_OFF,
                  STEP_EXP    = BINARY | STEP    | BUFFER | SINGLE_PREC_OFF;

        int init_opts = dre_fix.getSelectedOptions(),
            always_sel, changes_sel, step_sel;

        assumeThat(init_opts).withFailMessage("Unexpected options are selected at startup")
                             .isEqualTo(ALWAYS_EXP);
        
        // ACT
        dre_fix.setOptions(CHANGES);
        changes_sel = dre_fix.getSelectedOptions();

        dre_fix.setOptions(STEP);
        step_sel = dre_fix.getSelectedOptions();

        dre_fix.setOptions(ALWAYS);
        always_sel = dre_fix.getSelectedOptions();

        // ASSERT
        assertThat(always_sel).isEqualTo(ALWAYS_EXP);
        assertThat(changes_sel).isEqualTo(CHANGES_EXP);
        assertThat(step_sel).isEqualTo(STEP_EXP);
    }

    @Test
    public void testBufferOptions() {
        // ARRANGE
        final int BUFFER_EXP  = BINARY | ALWAYS | BUFFER      | SINGLE_PREC_OFF, // Should also be the initial options at startup
                  NO_BUFF_EXP = BINARY | ALWAYS | NO_BUFFER   | SINGLE_PREC_OFF,
                  RING_EXP    = BINARY | ALWAYS | RING_BUFFER | SINGLE_PREC_OFF;

        int init_opts = dre_fix.getSelectedOptions(),
            buffer_sel, no_buff_sel, ring_sel;

        assumeThat(init_opts).withFailMessage("Unexpected options are selected at startup")
                             .isEqualTo(BUFFER_EXP);
        
        // ACT
        dre_fix.setOptions(NO_BUFFER);
        no_buff_sel = dre_fix.getSelectedOptions();

        dre_fix.setOptions(RING_BUFFER);
        ring_sel = dre_fix.getSelectedOptions();

        dre_fix.setOptions(BUFFER);
        buffer_sel = dre_fix.getSelectedOptions();

        // ASSERT
        assertThat(buffer_sel).isEqualTo(BUFFER_EXP);
        assertThat(no_buff_sel).isEqualTo(NO_BUFF_EXP);
        assertThat(ring_sel).isEqualTo(RING_EXP);
    }

    @Test
    public void testSinglePrecOptions() {
        // ARRANGE
        final int SP_OFF_EXP = BINARY | ALWAYS | BUFFER | SINGLE_PREC_OFF, // Should also be the initial options at startup
                  SP_ON_EXP  = BINARY | ALWAYS | BUFFER | SINGLE_PREC_ON;

        int init_opts = dre_fix.getSelectedOptions(),
            sp_off_sel, sp_on_sel;

        assumeThat(init_opts).withFailMessage("Unexpected options are selected at startup")
                             .isEqualTo(SP_OFF_EXP);
        
        // ACT
        dre_fix.setOptions(SINGLE_PREC_ON);
        sp_on_sel = dre_fix.getSelectedOptions();

        dre_fix.setOptions(SINGLE_PREC_OFF);
        sp_off_sel = dre_fix.getSelectedOptions();

        // ASSERT
        assertThat(sp_off_sel).isEqualTo(SP_OFF_EXP);
        assertThat(sp_on_sel).isEqualTo(SP_ON_EXP);
    }
}