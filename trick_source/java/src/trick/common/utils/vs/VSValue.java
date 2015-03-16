package trick.common.utils.vs;

import javax.xml.bind.annotation.XmlElement;
import javax.xml.bind.annotation.XmlRootElement;

@XmlRootElement
public abstract class VSValue<T> implements VariableServerFluent, Cloneable {

    @XmlElement
    private T value;

    protected VSValue() {}

    protected VSValue(T value) {
        setValue(value);
    }

    public T getValue() {
        return value;
    }

    protected void setValue(T value) {
        this.value = value;
    }

    @Override
    public String toVariableServer() {
        return toString();
    }

    @Override
    public String toString() {
        // This method works even when value is null.
        return String.valueOf(value);
    }

    @Override
    public VSValue clone() {
        try {
            return (VSValue)super.clone();
        }
        catch (CloneNotSupportedException cloneNotSupportedException) {
            return null;
        }
    }

}
