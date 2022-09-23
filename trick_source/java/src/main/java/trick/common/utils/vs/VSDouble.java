package trick.common.utils.vs;

public class VSDouble extends VSValue<Double> {

    public VSDouble() {
        this(0);
    }

    public VSDouble(double value) {
        super(value);
    }

    @Override
    public void fromVariableServer(String string) {
        setValue(Double.parseDouble(handleUndefinedValues(string).trim()));
    }

    @Override
    public String toVariableServer() {
        return toString();
    }

}
