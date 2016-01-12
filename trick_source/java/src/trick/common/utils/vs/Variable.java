package trick.common.utils.vs;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;

import javax.xml.bind.annotation.XmlElement;
import javax.xml.bind.annotation.XmlElementRef;

import trick.common.utils.VariableServerConnection;

/**
 * @author Derek Bankieris
 *
 * encapsulates the data necessary to represent a Variable Server variable
 */
public class Variable<T extends VariableServerFluent> implements Serializable, Cloneable {

    /** possible states */
    public enum State {Unknown, Invalid, Valid};

    /** name */
    public final String name;

    /** value */
    @XmlElementRef(type=VSValue.class)
    public final T value;

    /** units */
    private String units;

    /** state */
    @XmlElement
    private State state = State.Unknown;

    /**
     * constructor
     *
     * @param name the name
     * @param value the initial value
     * @param units the initial units
     */
    public Variable(String name, T value, String units) {
        this.name = name;
        this.value = value;
        this.units = units;
    }

    /**
     * constructor
     *
     * @param name the name
     * @param value the initial value
     */
    public Variable(String name, T value) {
        this(name, value, null);
    }

    /**
     * provided for JAXB compatibility
     */
    private Variable() {
        name = null;
        value = null;
    }

    /**
     * commands the Variable Server to set the variable's value to <code>value</code>
     *
     * @param value the value to be sent
     * @param variableServerConnection the socket to send data to.
     * @throws IOException IOException
     */
    public void sendValueToVariableServer(T value,
      VariableServerConnection variableServerConnection) throws IOException {
        boolean invalidUnits = units == null || units.isEmpty();
        variableServerConnection.put("trick.var_set(\"" +
          name + "\", " + value.toVariableServer() +
          (invalidUnits ? "" : ", \"" + units + "\"") + ")");
    }

    /**
     * commands the Variable Server to set the variable's value to its current value
     * @param variableServerConnection the socket to send data to.
     * @throws IOException IOException
     */
    public void sendValueToVariableServer(VariableServerConnection variableServerConnection)
      throws IOException {
        sendValueToVariableServer(value, variableServerConnection);
    }

    /**
     * commands the Variable Server to set the variable's units to <code>units</code>
     *
     * @param units the units to be sent
     * @param variableServerConnection the socket to send data to.
     * @throws IOException IOException
     */
    public void sendUnitsToVariableServer(String units,
      VariableServerConnection variableServerConnection) throws IOException {
        if (units != null && !units.isEmpty()) {
            variableServerConnection.put("trick.var_units(\"" + name + "\", \"" + units + "\")");
        }
    }

    /**
     * commands the Variable Server to set the variable's units to its current units
     * @param variableServerConnection the socket to send data to.
     * @throws IOException IOException
     */
    public void sendUnitsToVariableServer(VariableServerConnection variableServerConnection)
      throws IOException {
        sendUnitsToVariableServer(units, variableServerConnection);
    }

    /**
    * sets this <code>Variable</code>'s value to <code>value</code>
    *
    * @param value the value to be set, expressed in Variable Server format
    */
    public void setValue(String value) {
        if (value.equals("BAD_REF")) {
            state = State.Invalid;
        }
        else {
            this.value.fromVariableServer(value);
            state = State.Valid;
        }
    }

    /**
     * returns the value
     *
     * @return the value
     */
    public T getValue() {
        return value;
    }

    /**
    * sets this <code>Variable</code>'s units to <code>units</code>
    *
    * @param units the units to be set
    */
    public void setUnits(String units) {
        this.units = units;
    }

    /**
     * returns the units
     *
     * @return the units
     */
    public String getUnits() {
        return units;
    }

    /**
     * provided for JAXB compatibility
     */
    private void setState(State state) {
        this.state = state;
    }

    /**
     * returns the state
     *
     * @return the state
     */
    public State getState() {
        return state;
    }

    @Override
    public String toString() {
        return name;
    }

    /**
     * Equality among <code>Variable</code>s requires only that their names
     * be equal as implemented by <code>String.equals</code>.
     *
     * @param object the object to compare
     *
     * @return the equality of <code>object</code> to this <code>Variable</code>
     */
    @Override
    public boolean equals(Object object) {
        return object != null && object instanceof Variable &&
          name.equals(((Variable)object).name);
    }

    /**
     * a <code>Variable</code>'s hashcode is equal to the hashcode of its name
     *
     * @return a hashcode for this object
     */
    @Override
    public int hashCode() {
        return name.hashCode();
    }

    @Override
    @SuppressWarnings("unchecked")
    public Variable<T> clone() {
        try {
            ByteArrayOutputStream byteArrayOutputStream = new ByteArrayOutputStream();
            new ObjectOutputStream(byteArrayOutputStream).writeObject(this);
            return (Variable<T>)new ObjectInputStream(new ByteArrayInputStream(
              byteArrayOutputStream.toByteArray())).readObject();
        }
        catch (Exception e) {
            return null;
        }
    }

}
