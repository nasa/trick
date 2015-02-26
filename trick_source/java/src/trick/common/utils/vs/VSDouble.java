package trick.common.utils.vs;

import javax.xml.bind.annotation.XmlRootElement;

@XmlRootElement
public class VSDouble extends VSValue {

    private static final long serialVersionUID = -2895628958106970334L;

    public double value;

    public VSDouble() {}

    public VSDouble(double value) {
        this.value = value;
    }

    public double getValue() {
        return value;
    }

    @Override
    public void fromVariableServer(String string) {
        value = Double.parseDouble(string.trim());
    }

    @Override
    public String toVariableServer() {
        return Double.toString(value);
    }

    @Override
    public String toString() {
        return Double.toString(value);
    }

}
