package trick.common.utils.vs;

import java.math.BigInteger;

import javax.xml.bind.annotation.XmlRootElement;

@XmlRootElement
public class VSByte extends VSValue {

    private static final long serialVersionUID = 8405472938103497097L;

    public byte value;

    public VSByte() {}

    public VSByte(byte value) {
        this.value = value;
    }

    public byte getValue() {
        return value;
    }

    @Override
    public void fromVariableServer(String string) {
        value = new BigInteger(string.trim(), 10).byteValue();
    }

    @Override
    public String toVariableServer() {
        return Byte.toString(value);
    }

    @Override
    public String toString() {
        return Byte.toString(value);
    }

}
