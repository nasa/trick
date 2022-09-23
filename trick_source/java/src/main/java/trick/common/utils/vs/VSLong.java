package trick.common.utils.vs;

import java.math.BigInteger;

public class VSLong extends VSValue<Long> {

    public VSLong() {
        this(0);
    }

    public VSLong(long value) {
        super(value);
    }

    @Override
    public void fromVariableServer(String string) {
        setValue(new BigInteger(string.trim(), 10).longValue());
    }

}
