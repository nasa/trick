package trick.common.utils;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.ListIterator;
import java.util.Map;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

/**
 * enumerates the available categories of units. Each catagory contains the
 * specific units appropriate to its measure.
 *
 * @author Derek Bankieris
 */
public enum UnitType {

    Angle(new ArrayList<Unit>() {{
        add(new Unit("radians", "rad", true, 0.0, 1.0));
        add(new Unit("degrees", "degree", false, 0.0, 0.0174532925199433));
        add(new Unit("arc-seconds", "arcsecond", false, 0.0, 4.848136811095362e-06));
        add(new Unit("arc-minutes", "arcminute", false, 0.0, 0.000290888208666));
        add(new Unit("revolutions", "revolution", false, 0.0, 6.28318530717958647));
    }}),

    Current(new ArrayList<Unit>() {{
        add(new Unit("amperes", "A", true, 0.0, 1.0));
    }}),

    Distance(new ArrayList<Unit>() {{
        add(new Unit("meters", "m", true, 0.0, 1.0));
        add(new Unit("inches", "in", false, 0.0, 0.0254));
        add(new Unit("feet", "ft", false, 0.0, 0.3048));
        add(new Unit("kilo-feet", "kft", false, 0.0, 0.3048 * KILO));
        add(new Unit("yards", "yd", false, 0.0, 0.9144));
        add(new Unit("miles", "mi", false, 0.0, 1609.344));
        add(new Unit("nautical-miles", "nmile", false, 0.0, 1852.0));
    }}),

    Energy(new ArrayList<Unit>() {{
        add(new Unit("joules", "J", true, 0.0, 1.0));
        add(new Unit("british-thermal-units", "BTU", false, 0.0, 1055.056));
        add(new Unit("tons-tnt", "ton_TNT", false, 0.0, 4.184 * GIGA));
    }}),

    Force(new ArrayList<Unit>() {{
        add(new Unit("newtons", "N", true, 0.0, 1.0));
        add(new Unit("ounces", "ozf", false, 0.0, 0.27801385));
        add(new Unit("pounds-force", "lbf", false, 0.0, 4.4482216152605));
    }}),

    Frequency(new ArrayList<Unit>() {{
        add(new Unit("hertz", "Hz", true, 0.0, 1.0));
    }}),

    Gain(new ArrayList<Unit>() {{
        add(new Unit("decibels", "dB", false, 0.0, 1.0));
    }}),

    Mass(new ArrayList<Unit>() {{
        add(new Unit("grams", "g", true, 0.0, MILLI));
        add(new Unit("metric-tons", "t", false, 0.0, 1000.0));
        add(new Unit("slugs", "slug", false, 0.0, 14.5939029372064));
        add(new Unit("pounds-mass", "lb", false, 0.0, 0.4535923697760192));
    }}),

    Power(new ArrayList<Unit>() {{
        add(new Unit("watts", "W", true, 0.0, 1.0));
        add(new Unit("horse-power", "hp", false, 0.0, 745.69987));
    }}),

    Pressure(new ArrayList<Unit>() {{
        add(new Unit("atmospheres", "atm", false, 0.0, 101325.00000));
        add(new Unit("pascals", "Pa", true, 0.0, 1.0));
        add(new Unit("pounds-sq-inch", "psi", false, 0.0, 249.08891));
        add(new Unit("milli-meters-mercury", "mmHg", false, 0.0, MILLI*133322.387415));
        add(new Unit("inches-mercury", "inHg", false, 0.0, 3386.389));
        add(new Unit("milli-meters-water", "mmWater", false, 0.0, MILLI*9806.65));
        add(new Unit("inches-water", "inWater", false, 0.0, 249.08891));
    }}),

    Resistance(new ArrayList<Unit>() {{
        add(new Unit("ohms", "ohm", true, 0.0, 1.0));
    }}),

    Temperature(new ArrayList<Unit>() {{
        add(new Unit("degrees-Celsius", "degC", false, 0.0, 1.0));
        add(new Unit("degrees-Kelvin", "degK", false, -273.15, 1.0));
        add(new Unit("degrees-Fahrenheit", "degF", false, -17.777777777777778, 0.55555555555555556));
        add(new Unit("degrees-Rankine", "degR", false, -273.15, 0.55555555555555556));
    }}),

    Time(new ArrayList<Unit>() {{
        add(new Unit("seconds", "s", true, 0.0, 1.0));
        add(new Unit("minutes", "min", false, 0.0, 60.0));
        add(new Unit("hours", "hr", false, 0.0, 3600.0));
        add(new Unit("days", "d", false, 0.0, 86400.0));
    }}),

    Unitless(new ArrayList<Unit>() {{
        add(new Unit("unitless", "--", false, 0.0, 1.0));
        add(new Unit("count", "count", false, 0.0, 1.0));
        add(new Unit("one", "1", false, 0.0, 1.0));
        add(new Unit("moles", "mol", false, 0.0, 6.02214179e+23));
    }}),

    Voltage(new ArrayList<Unit>() {{
        add(new Unit("volts", "V", true, 0.0, 1.0));
    }}),

    Volume(new ArrayList<Unit>() {{
        add(new Unit("liters", "l", true, 0.0, 1.0));
        add(new Unit("gallons", "gallon", false, 0.0, 3.785412));
        add(new Unit("fluid-ounces", "floz", false, 0.0, 0.02957353));
    }});

    public static final double PICO  = 1e-12;
    public static final double NANO  = 1e-09;
    public static final double MICRO = 1e-06;
    public static final double MILLI = 1e-03;
    public static final double CENTI = 1e-02;
    public static final double DECI  = 1e-01;
    public static final double DECA  = 1e+01;
    public static final double HECTO = 1e+2;
    public static final double KILO  = 1e+3;
    public static final double MEGA  = 1e+6;
    public static final double GIGA  = 1e+9;
    public static final double TERA  = 1e+12;

    /** SI unit prefixes */
    private final Unit[] prefixes = {
      new Unit("pico-", "p", false, 0.0, PICO),
      new Unit("nano-", "n", false, 0.0, NANO),
      new Unit("micro-", "u", false, 0.0, MICRO),
      new Unit("milli-", "m", false, 0.0, MILLI),
      new Unit("centi-", "c", false, 0.0, CENTI),
      new Unit("deci-", "d", false, 0.0, DECI),
      new Unit("deca-", "da", false, 0.0, DECA),
      new Unit("hecto-", "h", false, 0.0, HECTO),
      new Unit("kilo-", "k", false, 0.0, KILO),
      new Unit("mega-", "M", false, 0.0, MEGA),
      new Unit("giga-", "G", false, 0.0, GIGA),
      new Unit("tera-", "T", false, 0.0, TERA)
    };


    /** valid units for this type */
    private final ArrayList<Unit> units;

    /**
     * constructs the type and adds prefixes to units that accept SI prefixes
     *
     * @param units the units appropriate to this type
     */
    private UnitType(ArrayList<Unit> units) {
        this.units = units;
        ListIterator<Unit> iterator = units.listIterator();
        while (iterator.hasNext()) {
            Unit unit = iterator.next();
            if (unit.isPrefixable) {
                for (Unit prefix : prefixes) {
                    iterator.add(new Unit(prefix.name + unit.name,
                      prefix.abbreviation + unit.abbreviation, false, unit.factor1, prefix.factor2 * unit.factor2));
                }
            }
        }
    }

    /**
     * gets all units appropriate for this <code>UnitType</code>
     *
     * @return the units available for this type
     */
    public List<Unit> getAll() {
        return Collections.unmodifiableList(units);
    }

    /**
     * converts the value of a specified units to the preferred units
     *
     * @param fromValue from value
     * @param fromUnitStr from unit
     * @param toUnitStr to unit
     * @return new value based on the new units
     * @throws IllegalUnitConversionException IllegalUnitConversionException
     */
    public static double convertUnits(double fromValue, String fromUnitStr, String toUnitStr) throws IllegalUnitConversionException {
        Unit fromUnit = getExpressionUnit(fromUnitStr);
        Unit toUnit = getExpressionUnit(toUnitStr);

        if (!isConvertible(fromUnitStr, toUnitStr)) {
            throw new IllegalUnitConversionException(fromUnitStr, toUnitStr);
        }

        double derivedFactor1 = fromUnit.factor1 - toUnit.factor1 / toUnit.factor2;
        double derivedFactor2 = fromUnit.factor2 / toUnit.factor2;

        return (fromValue * derivedFactor2 + derivedFactor1);
    }

    /**
     * gets the <code>UnitType</code> of which <code>abbreviation</code> is a
     * member. This method handles only simple units, not compound units
     * resulting from multiplication, division, or exponentiation.
     *
     * @param abbreviation abbreviation of unit to search
     * @return the corresponding type, or null if none exists
     */
    public static UnitType getType(String abbreviation) {
        for (UnitType type : UnitType.values()) {
            for (Unit unit : type.getAll()) {
                if (unit.abbreviation.equals(abbreviation)) {
                    return type;
                }
            }
        }
        return null;
    }

    /**
     * returns the primitive unit if it is a primitive unit.
     * Otherwise, returns a complex unit that is made out of primitive units.
     *
     * @param expression full expression to parse
     * @return an instance of {@link Unit}
     */
    public static Unit getExpressionUnit(String expression) {
        Unit ret = null;

        ret = getPrimitiveUnit(expression);
        if (ret != null) {
            return ret;
        }

        UnitInfixExpression unitExpression = new UnitInfixExpression(expression);

        ret = unitExpression.getUnit();

        return ret;
    }

    /**
     * gets the {@link Unit} based on its abbreviation
     *
     * @param abbreviation the units abbreviation
     * @return the corresponding Unit, or null if the abbreviation doesn't exist
     */
    public static Unit getPrimitiveUnit(String abbreviation) {
        Unit ret = null;
        for (UnitType type : UnitType.values()) {
            for (Unit unit : type.getAll()) {
                if (unit.abbreviation.equals(abbreviation)) {
                    ret = unit;
                }
            }
        }
        return ret;
    }

    /**
     * gets all valid unit alternatives for <code>expression</code>.
     * This method handles compound units resulting from multiplication,
     * division, or exponentiation.
     * @param expression full expression to parse
     * @return list of valid unit alternatives
     */
    public static List<String> getAll(String expression) {
        ArrayList<String> results = new ArrayList<String>();
        Matcher matcher = Pattern.compile("([^\\d\\*/]+)(\\d*)").matcher(expression);
        if (matcher.find()) {
            UnitType type = UnitType.getType(matcher.group(1));
            if (type != null) {
                for (Unit unit : type.getAll()) {
                    getAll(unit + matcher.group(2), expression.substring(matcher.end()), results);
                }
            }
        }
        return results;
    }

    /**
     * gets all valid unit alternatives for <code>tail</code>, concatenating
     * each result to <code>head</code>, and appending each concatenation to
     * <code>results</code>. This method handles compound units resulting
     * from multiplication, division, or exponentiation.
     *
     * @param head the (already processed) head of the units expression
     * @param tail the (not yet processed) tail of the units expression
     * @param results the list to which to append results
     */
    private static void getAll(String head, String tail, List<String> results) {
        Matcher matcher = Pattern.compile("(\\*|/)([^\\d\\*/]+)(\\d*)").matcher(tail);
        if (matcher.find()) {
            UnitType type = UnitType.getType(matcher.group(2));
            if (type != null) {
                for (Unit unit : type.getAll()) {
                    getAll(head + matcher.group(1) + unit + matcher.group(3), tail.substring(matcher.end()), results);
                }
            }
        }
        else {
            results.add(head);
        }
    }

    /**
     * determines if <code>fromUnits</code> can legally be converted to
     * <code>toUnits</code>. This method handles compound units resulting
     * from multiplication, division, or exponentiation.
     *
     * @param fromUnits the unit expression from which to convert
     * @param toUnits the unit expression to which to convert
     *
     * @return whether or not such a conversion is valid
     */
    public static boolean isConvertible(String fromUnits, String toUnits) {
        return getAll(fromUnits).contains(toUnits);
    }

    /**
     * represents a unit
     */
    public static class Unit {

        /** represents default units */
        public static final Unit DEFAULT_UNITS = new Unit("default", "xx", false);

        /** the full name */
        public final String name;

        /** the abbreviation to use following a value */
        public final String abbreviation;

        public double factor1;
        public double factor2;

        /** whether or not metric prefixes are valid */
        final boolean isPrefixable;

        public Unit(String name, String abbreviation, boolean isPrefixable, double factor1, double factor2) {
            this(name, abbreviation, isPrefixable);
            this.factor1 = factor1;
            this.factor2 = factor2;
        }

        /**
         * constructor
         *
         * @param name name of unit
         * @param abbreviation abbreviation of unit
         * @param isPrefixable true or false
         */
        public Unit(String name, String abbreviation, boolean isPrefixable) {
            this.name = name;
            this.abbreviation = abbreviation;
            this.isPrefixable = isPrefixable;
        }

        @Override
        public String toString() {
            return abbreviation;
        }
    }

    public static class IllegalUnitConversionException extends Exception {

        private static final long serialVersionUID = 2800176399857985431L;

        public IllegalUnitConversionException(String fromUnit, String toUnit) {
            super("Illegal Unit Conversion", new Throwable("Can't convert " + fromUnit + " -> " + toUnit));
        }
    }

}
