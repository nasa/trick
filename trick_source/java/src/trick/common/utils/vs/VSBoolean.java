package trick.common.utils.vs;

import javax.xml.bind.annotation.XmlRootElement;

@XmlRootElement
public class VSBoolean extends VSValue {

    private static final long serialVersionUID = 3720173025885361193L;

    public boolean value;

    public VSBoolean() {}

    public VSBoolean(boolean value) {
        this.value = value;
    }

    public boolean getValue() {
        return value;
    }

    @Override
    public void fromVariableServer(String string) {
        value = Integer.parseInt(string.trim()) != 0;
    }

    @Override
    public String toVariableServer() {
        return value ? "1" : "0";
    }

    @Override
    public String toString() {
        return Boolean.toString(value);
    }

}
