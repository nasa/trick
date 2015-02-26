package trick.common.utils.vs;

import java.math.BigInteger;

import javax.xml.bind.annotation.XmlRootElement;

@XmlRootElement
public class VSInteger extends VSValue {

    private static final long serialVersionUID = 1314788741378939173L;

    public int value;

    public VSInteger() {}

    public VSInteger(int value) {
        this.value = value;
    }

    public int getValue() {
        return value;
    }

    @Override
    public void fromVariableServer(String string) {
        value = new BigInteger(string.trim(), 10).intValue();
    }

    @Override
    public String toVariableServer() {
        return Integer.toString(value);
    }

    @Override
    public String toString() {
        return Integer.toString(value);
    }

}
