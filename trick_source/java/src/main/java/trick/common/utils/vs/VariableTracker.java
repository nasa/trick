package trick.common.utils.vs;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.swing.SwingWorker;

import trick.common.utils.VariableServerConnection;

/**
 * @author Derek Bankieris
 */
public class VariableTracker extends SwingWorker<Void, String[]> {

    /** connection to the Variable Server */
    VariableServerConnection variableServerConnection;

    /** variables being tracked */
    LinkedHashSet<Variable> variables = new LinkedHashSet<Variable>(10);

    /** listeners to be informed of updates the the variables */
    ArrayList<VariableListener> variableListeners = new ArrayList<VariableListener>();

    /** regular expression for finding units */
    Pattern pattern = Pattern.compile("^(.*?)\\s*(?:\\{(.*)\\})?$");

    /**
     * sole constructor
     *
     * @param variableServerConnection connection to the Variable Server
     */
    public VariableTracker(VariableServerConnection variableServerConnection) {
        this.variableServerConnection = variableServerConnection;
    }

    /**
     * begin tracking <code>variable</code>
     *
     * @param variable the variable to track
     * @throws IOException IOException
     */
    public void add(Variable variable) throws IOException {
        if (variables.add(variable)) {
            variableServerConnection.add(variable.name, variable.getUnits());
        }
    }

    /**
     * stops tracking <code>variable</code>
     *
     * @param variable the variable to stop tracking
     * @throws IOException IOException
     */
    public void remove(Variable variable) throws IOException {
        variableServerConnection.remove(variable.name);
        variables.remove(variable);
    }

    /**
     * causes the <code>variableListener</code> to be notified of updates to the variables
     *
     * @param variableListener the listener to be notified
     */
    public void addVariableListener(VariableListener variableListener) {
        variableListeners.add(variableListener);
    }

    /**
     * causes the <code>variableListener</code> to no longer be notified of updates to the variables
     *
     * @param variableListener the listener to be removed
     */
    public void removeVariableListener(VariableListener variableListener) {
        variableListeners.remove(variableListener);
    }

    /** Forever listen for incoming messages from the Variable Server. */
    @Override
    public Void doInBackground() throws IOException {
        while (true) {
            if (!isCancelled()) {
                String line = variableServerConnection.get();
                if (line == null) {
                    throw new IOException("Reached end of stream.");
                }
                publish(line.split("\t", -1));
            }
        }
    }

    /**
     * inform listeners that the variables have been updated
     */
    @Override
    protected void process(List<String[]> valueList) {
        if (!isCancelled()) {
            String[] values = valueList.get(valueList.size() - 1);
            if (values.length == variables.size() + 1) {
                int i = 1;
                for (Variable variable : variables) {
                    Matcher matcher = pattern.matcher(values[i]);
                    matcher.find();
                    variable.setValue(matcher.group(1));
                    String units = matcher.group(2);
                    if (units != null) {
                        variable.setUnits(units);
                    }
                    ++i;
                }
                for (VariableListener variableListener : variableListeners) {
                    variableListener.variablesUpdated();
                }
            }
        }
    }

    /** executed on the Event Dispatch Thread after the doInBackground method is finished */
    @Override
    protected void done() {
        if (!isCancelled()) {
            try {
                    // Check for any exception that occurred during execution by calling get().
                    get();
            }
            catch (Exception exception) {
                for (VariableListener variableListener : variableListeners) {
                    variableListener.variableUpdateFailed(exception);
                }
            }
        }
    }

}
