package trick.common.utils.vs;

import java.math.BigInteger;

public class VSShort extends VSValue<Short> {

    public VSShort() {
        this((short)0);
    }

    public VSShort(short value) {
        super(value);
    }

    @Override
    public void fromVariableServer(String string) {
        setValue(new BigInteger(string.trim(), 10).shortValue());
    }

}
