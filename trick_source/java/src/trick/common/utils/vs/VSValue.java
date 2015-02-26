package trick.common.utils.vs;

public abstract class VSValue implements VariableServerFluent, Cloneable {

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
