package trick.common.utils.vs;

public class VSBoolean extends VSValue<Boolean> {

    public VSBoolean() {
        this(false);
    }

    public VSBoolean(boolean value) {
        super(value);
    }

    @Override
    public void fromVariableServer(String string) {
        setValue(Integer.parseInt(string.trim()) != 0);
    }


    @Override
    public String toString() {
        return getValue() ? "True" : "False";
    }

}
