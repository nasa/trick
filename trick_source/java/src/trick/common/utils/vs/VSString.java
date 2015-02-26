package trick.common.utils.vs;

import javax.xml.bind.annotation.XmlRootElement;

@XmlRootElement
public class VSString extends VSValue {

    private static final long serialVersionUID = 2238385297450301960L;

    public String value;

    public VSString() {}

    public VSString(String value) {
        this.value = value;
    }

    public String getValue() {
        return value;
    }

    @Override
    public void fromVariableServer(String string) {
        value = string;
    }

    @Override
    public String toVariableServer() {
        return "\"" + value + "\"";
    }

    @Override
    public String toString() {
        return value;
    }

}
