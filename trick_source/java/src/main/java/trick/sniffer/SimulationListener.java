package trick.sniffer;

/**
 * <code>SimulationListener</code>s are notified by a SimulationSniffer when
 * simulations are added or removed from the list of currently running
 * simulations.
 *
 * @author Derek Bankieris
 */
public interface SimulationListener {

    /**
     * called when a new simulation is detected
     *
     * @param simulationInformation the new simulation
     */
    public void simulationAdded(SimulationInformation simulationInformation);

    /**
     * called when a simulation expires
     *
     * @param simulationInformation the expired simulation
     */
    public void simulationRemoved(SimulationInformation simulationInformation);

    /**
     * called when the source experiences an unhandled exception
     *
     * @param exception the exception that occured
     */
    public void exceptionOccurred(Exception exception);

}
