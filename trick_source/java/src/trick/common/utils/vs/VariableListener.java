package trick.common.utils.vs;

/**
 * designates a class as being capable receiving variable events
 *
 * @author Derek Bankieris
 */
public interface VariableListener {

    /** invoked after the variables have been updated */
    public void variablesUpdated();

    /** invoked if an exception is thrown during variable tracking
     * @param exception the exception
     */
    public void variableUpdateFailed(Exception exception);
}
