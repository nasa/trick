package trick.common.utils.vs;

import java.math.BigInteger;

import javax.xml.bind.annotation.XmlRootElement;

@XmlRootElement
public class VSShort extends VSValue {

    private static final long serialVersionUID = 5431648313760456676L;

    public short value;

    public VSShort() {}

    public VSShort(short value) {
        this.value = value;
    }

    public short getValue() {
        return value;
    }

    @Override
    public void fromVariableServer(String string) {
        value = new BigInteger(string.trim(), 10).shortValue();
    }

    @Override
    public String toVariableServer() {
        return Short.toString(value);
    }

    @Override
    public String toString() {
        return Short.toString(value);
    }

}
