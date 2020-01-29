package trick.sie.utils;

import java.util.Collection;

/**
 * designates a class as being capable of responding to search events
 */
public interface SearchListener {

    /**
     * invoked when a match is found
     *
     * @param variables the matching variables
     */
    public void matchFound(Collection<? extends SieTemplate> variables);

    /**
     * invoked if an error occurs while searching
     *
     * @param exception the exception
     */
    public void searchFailed(Exception exception);

    /**
     * invoked if the search is canceled
     */
    public void searchCanceled();

    /**
     * invoked after the search completes
     */
    public void searchComplete();

}
