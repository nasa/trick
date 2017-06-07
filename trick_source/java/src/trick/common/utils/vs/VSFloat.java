package trick.common.utils.vs;

public class VSFloat extends VSValue<Float> {

    public VSFloat() {
        this(0);
    }

    public VSFloat(float value) {
        super(value);
    }

    @Override
    public void fromVariableServer(String string) {
        // CTLaChance
        // JUN 07 2017
        /*
        Converts the C representation of +- infinity and +- not-a-number to the Java representation
        so that Float.parseFloat() can correctly assign those values to the variable value.
        -NaN does not exist in Java and is therefore converted to NaN.
        */
        if(string.equals("inf")) {
            string = "Infinity";
        }
        else if(string.equals("-inf")) {
            string = "-Infinity";
        }
        else if(string.equals("-nan") || string.equals("nan")) {
            string = "NaN";
        }

        setValue(Float.parseFloat(string.trim()));
    }
}
