package trick.common.utils.vs;

import javax.xml.bind.annotation.XmlRootElement;

@XmlRootElement
public class VSFloat extends VSValue {

    private static final long serialVersionUID = -4629816234119061148L;

    public float value;

    public VSFloat() {}

    public VSFloat(float value) {
        this.value = value;
    }

    public float getValue() {
        return value;
    }

    @Override
    public void fromVariableServer(String string) {
        value = Float.parseFloat(string.trim());
    }

    @Override
    public String toVariableServer() {
        return Float.toString(value);
    }

    @Override
    public String toString() {
        return Float.toString(value);
    }

}
