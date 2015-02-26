package trick.common.utils;

import static org.junit.Assert.assertArrayEquals;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertTrue;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import trick.common.utils.UnitType.IllegalUnitConversionException;

public class UnitTypeTest {
	
	@BeforeClass
	public static void setUpBeforeClass() throws Exception {
	}

	@AfterClass
	public static void tearDownAfterClass() throws Exception {
	}

	@Before
	public void setUp() throws Exception {
		
	}

	@After
	public void tearDown() throws Exception {
	}

	@Test
	public void testGetPermutationList() {
		String units = "m/s";
		
		String[] expectedPermutableList = new String[] {
				"m/s", "m/ps", "m/ns", "m/us", "m/ms", "m/cs", "m/ds", "m/das", "m/hs", "m/ks", "m/Ms", "m/Gs", "m/Ts", "m/min", "m/hr", "m/day", 
				"pm/s", "pm/ps", "pm/ns", "pm/us", "pm/ms", "pm/cs", "pm/ds", "pm/das", "pm/hs", "pm/ks", "pm/Ms", "pm/Gs", "pm/Ts", "pm/min", "pm/hr", "pm/day", 
				"nm/s", "nm/ps", "nm/ns", "nm/us", "nm/ms", "nm/cs", "nm/ds", "nm/das", "nm/hs", "nm/ks", "nm/Ms", "nm/Gs", "nm/Ts", "nm/min", "nm/hr", "nm/day", 
				"um/s", "um/ps", "um/ns", "um/us", "um/ms", "um/cs", "um/ds", "um/das", "um/hs", "um/ks", "um/Ms", "um/Gs", "um/Ts", "um/min", "um/hr", "um/day", 
				"mm/s", "mm/ps", "mm/ns", "mm/us", "mm/ms", "mm/cs", "mm/ds", "mm/das", "mm/hs", "mm/ks", "mm/Ms", "mm/Gs", "mm/Ts", "mm/min", "mm/hr", "mm/day", 
				"cm/s", "cm/ps", "cm/ns", "cm/us", "cm/ms", "cm/cs", "cm/ds", "cm/das", "cm/hs", "cm/ks", "cm/Ms", "cm/Gs", "cm/Ts", "cm/min", "cm/hr", "cm/day",
				"dm/s", "dm/ps", "dm/ns", "dm/us", "dm/ms", "dm/cs", "dm/ds", "dm/das", "dm/hs", "dm/ks", "dm/Ms", "dm/Gs", "dm/Ts", "dm/min", "dm/hr", "dm/day", 
				"dam/s", "dam/ps", "dam/ns", "dam/us", "dam/ms", "dam/cs", "dam/ds", "dam/das", "dam/hs", "dam/ks", "dam/Ms", "dam/Gs", "dam/Ts", "dam/min", "dam/hr", "dam/day",
				"hm/s", "hm/ps", "hm/ns", "hm/us", "hm/ms", "hm/cs", "hm/ds", "hm/das", "hm/hs", "hm/ks", "hm/Ms", "hm/Gs", "hm/Ts", "hm/min", "hm/hr", "hm/day", 
				"km/s", "km/ps", "km/ns", "km/us", "km/ms", "km/cs", "km/ds", "km/das", "km/hs", "km/ks", "km/Ms", "km/Gs", "km/Ts", "km/min", "km/hr", "km/day", 
				"Mm/s", "Mm/ps", "Mm/ns", "Mm/us", "Mm/ms", "Mm/cs", "Mm/ds", "Mm/das", "Mm/hs", "Mm/ks", "Mm/Ms", "Mm/Gs", "Mm/Ts", "Mm/min", "Mm/hr", "Mm/day", 
				"Gm/s", "Gm/ps", "Gm/ns", "Gm/us", "Gm/ms", "Gm/cs", "Gm/ds", "Gm/das", "Gm/hs", "Gm/ks", "Gm/Ms", "Gm/Gs", "Gm/Ts", "Gm/min", "Gm/hr", "Gm/day", 
				"Tm/s", "Tm/ps", "Tm/ns", "Tm/us", "Tm/ms", "Tm/cs", "Tm/ds", "Tm/das", "Tm/hs", "Tm/ks", "Tm/Ms", "Tm/Gs", "Tm/Ts", "Tm/min", "Tm/hr", "Tm/day", 
				"in/s", "in/ps", "in/ns", "in/us", "in/ms", "in/cs", "in/ds", "in/das", "in/hs", "in/ks", "in/Ms", "in/Gs", "in/Ts", "in/min", "in/hr", "in/day", 
				"ft/s", "ft/ps", "ft/ns", "ft/us", "ft/ms", "ft/cs", "ft/ds", "ft/das", "ft/hs", "ft/ks", "ft/Ms", "ft/Gs", "ft/Ts", "ft/min", "ft/hr", "ft/day", 
				"kft/s", "kft/ps", "kft/ns", "kft/us", "kft/ms", "kft/cs", "kft/ds", "kft/das", "kft/hs", "kft/ks", "kft/Ms", "kft/Gs", "kft/Ts", "kft/min", "kft/hr", "kft/day", 
				"yd/s", "yd/ps", "yd/ns", "yd/us", "yd/ms", "yd/cs", "yd/ds", "yd/das", "yd/hs", "yd/ks", "yd/Ms", "yd/Gs", "yd/Ts", "yd/min", "yd/hr", "yd/day", 
				"mi/s", "mi/ps", "mi/ns", "mi/us", "mi/ms", "mi/cs", "mi/ds", "mi/das", "mi/hs", "mi/ks", "mi/Ms", "mi/Gs", "mi/Ts", "mi/min", "mi/hr", "mi/day", 
				"n.m./s", "n.m./ps", "n.m./ns", "n.m./us", "n.m./ms", "n.m./cs", "n.m./ds", "n.m./das", "n.m./hs", "n.m./ks", "n.m./Ms", "n.m./Gs", "n.m./Ts", "n.m./min", "n.m./hr", "n.m./day"
				
		};
		assertArrayEquals("The permutable list of units (m/s) is wrong. Error in getPermutationList in UnitsUtils.java", expectedPermutableList, UnitType.getAll(units).toArray());
	}
	
	@Test
	public void testIsPermutable() {
		assertTrue("m/s2 and km/min2 should be convertable", UnitType.isConvertible("m/s2", "km/min2"));
		assertTrue("kg*m/s2 and lbm*ft/s2 should be convertable", UnitType.isConvertible("kg*m/s2", "lbm*ft/s2"));
	}
    
	@Test
	public void getUnitType() {
		assertNull("Foo UnitType doesn't exist", UnitType.getType("Foo"));
		assertNotNull("das (s with da meaning deca- prefix) UnitType should exist", UnitType.getType("das"));
	}
	
	@Test(expected=UnitType.IllegalUnitConversionException.class)
	public void testIllegalConvertUnits() throws IllegalUnitConversionException {
		UnitType.convertUnits(1.0, "g", "s");
	}
	
	@Test
	public void testConvertUnits() throws IllegalUnitConversionException {
		assertEquals("20s (second) = 1/3min (minute)", 1.0/3.0, UnitType.convertUnits(20.0, "s", "min"), 0.01);
		assertEquals("20m = 1/3hr", 1.0/3.0, UnitType.convertUnits(20.0, "min", "hr"), 0.01);
		assertEquals("1 C = 274.15K", 274.15, UnitType.convertUnits(1.0, "C", "K"), 0.001);
		assertEquals("1 C = 33.8F", 33.8, UnitType.convertUnits(1.0, "C", "F"), 0.001);
		assertEquals("1 C = 493.47R", 493.47, UnitType.convertUnits(1.0, "C", "R"), 0.001);
		assertEquals("17 Celsius = 62.599999999999994 Fahrenheit", 62.599999999999994, UnitType.convertUnits(17.0, "C", "F"), 0.01);
		assertEquals("1 F = 255.927778K", 255.927778, UnitType.convertUnits(1.0, "F", "K"), 0.001);
		//assertEquals("1 K = 1.8R", 1.8, UnitType.convertUnits(1.0, "K", "R"), 0.001);
		assertEquals("1s = 1000000us", 1000000.0, UnitType.convertUnits(1.0, "s", "us"), 0.001);
		assertEquals("1g = 0.0022046226229374033lbm", new Double(0.0022046226229374033), UnitType.convertUnits(1.0, "g", "lbm"), 0.01);
		assertEquals("1g = 0.001kg", 0.001, UnitType.convertUnits(1.0, "g", "kg"), 0.001);
		assertEquals("1 lbm/ft3 = 16.019 kg/m3", 16.01846336605033, UnitType.convertUnits(1.0, "lbm/ft3", "kg/m3"), 0.01);
		assertEquals("1 kg*m/s2 = 1 lbm*ft/s2", 7.233013854781507, UnitType.convertUnits(1.0, "kg*m/s/s", "lbm*ft/s/s"), 0.01);
		assertEquals("1 Mhz = 1000000 Hz", 1000000.0, UnitType.convertUnits(1.0, "MHz", "Hz"), 0.001);
		assertEquals("1 J/kg*K = 0.001 J/g*C", 0.001, UnitType.convertUnits(1.0, "J/kg*K", "J/g*C"), 0.001);
		assertEquals("1 BTU/lbf/C = 1.798796216558 BTU/lbf/F", 1.798796216558, UnitType.convertUnits(1.0, "BTU/lbf/C", "BTU/lbf/F"), 0.01);
		assertEquals("1 K*m2/W = 1.798796216558 F*m2/W", 1.798796216558, UnitType.convertUnits(1.0, "K*m2/W", "F*m2/W"), 0.01);
	}
}
