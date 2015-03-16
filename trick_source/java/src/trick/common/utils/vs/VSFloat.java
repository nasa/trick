package trick.common.utils.vs;

public class VSFloat extends VSValue<Float> {

    protected VSFloat() {
        this(0);
    }

    public VSFloat(float value) {
        super(value);
    }

    @Override
    public void fromVariableServer(String string) {
        setValue(Float.parseFloat(string.trim()));
    }

}
