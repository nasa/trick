package trick.common.utils.vs;

import java.math.BigInteger;

public class VSByte extends VSValue<Byte> {

    public VSByte() {
        this((byte)0);
    }

    public VSByte(byte value) {
        super(value);
    }

    @Override
    public void fromVariableServer(String string) {
        setValue(new BigInteger(string.trim(), 10).byteValue());
    }

}
