package trick.common.utils.vs;

import java.math.BigInteger;

public class VSInteger extends VSValue<Integer> {

    public VSInteger() {
        this(0);
    }

    public VSInteger(int value) {
        super(value);
    }

    @Override
    public void fromVariableServer(String string) {
        setValue(new BigInteger(string.trim(), 10).intValue());
    }

}
