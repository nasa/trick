package trick.common.utils.vs;

import java.io.Serializable;

/**
 * designates a class as being compatible with Trick's Variable Server protocol
 *
 * @author Derek Bankieirs
 */
public interface VariableServerFluent extends Serializable {

    /**
     * sets the state to reflect the value specified by the Variable Server <code>string</code>
     *
     * @param string the value from the Variable Server
     */
    public void fromVariableServer(String string);

    /**
     * returns the state formatted as a Variable Server string
     *
     * @return the state appropriately formatted for transmission to the Variable Server
     */
    public String toVariableServer();

    /**
     * returns a deep copy of this instance
     *
     * @return a deep copy of this instance
     */
    public VariableServerFluent clone();

}
