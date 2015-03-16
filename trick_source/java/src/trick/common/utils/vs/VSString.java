package trick.common.utils.vs;

public class VSString extends VSValue<String> {

    public VSString() {}

    public VSString(String value) {
        super(value);
    }

    @Override
    public void fromVariableServer(String string) {
        setValue(string);
    }

    @Override
    public String toVariableServer() {
        return "\"" + super.toVariableServer() + "\"";
    }

}
