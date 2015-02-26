package trick.common.utils.vs;

import java.math.BigInteger;

import javax.xml.bind.annotation.XmlRootElement;

@XmlRootElement
public class VSLong extends VSValue {

    private static final long serialVersionUID = -7792501925528852232L;

    public long value;

    public VSLong() {}

    public VSLong(long value) {
        this.value = value;
    }

    public long getValue() {
        return value;
    }

    @Override
    public void fromVariableServer(String string) {
        value = new BigInteger(string.trim(), 10).longValue();
    }

    @Override
    public String toVariableServer() {
        return Long.toString(value);
    }

    @Override
    public String toString() {
        return Long.toString(value);
    }

}
