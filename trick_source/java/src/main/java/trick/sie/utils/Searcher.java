package trick.sie.utils;

import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.util.ArrayList;
import java.util.Collection;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.Executors;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.TimeUnit;
import java.util.List;
import java.util.regex.Pattern;

import javax.swing.SwingWorker;

/**
 * searches a collection of <code>SieTemplate</code>s for target text
 */
public class Searcher {

    /** the templates from which to begin the search */
    private Collection<SieTemplate> rootTemplates;

    /** listener to be notified of search events */
    private final SearchListener searchListener;

    /** listener to be notified of progress */
    private final PropertyChangeListener propertyChangeListener;

    /** total number of searchable elements */
    long elementCount = 0;

    /** number of elements searched */
    private long count = 0;

    /** number of threads performing searches */
    private int threads;

    /** the executor for the search threads */
    ExecutorService executorService = Executors.newFixedThreadPool(1);

    /**
     * constructor
     *
     * @param rootTemplates the templates from which to begin the search
     * @param searchListener a listener to notify of search events
     * @param propertyChangeListener a listener to notify of property changes
     */
    public Searcher(Collection<SieTemplate> rootTemplates, SearchListener searchListener,
      PropertyChangeListener propertyChangeListener) {
        this.rootTemplates = rootTemplates;
        this.searchListener = searchListener;
        this.propertyChangeListener = propertyChangeListener;
    }

    /**
     * cancels any executing search and sets the templates from which to begin
     * searches
     *
     * @param rootTemplates the templates to search
     */
    public void setRootTemplates(Collection<SieTemplate> rootTemplates) {
        if (this.rootTemplates != rootTemplates) {
            cancelSearch();
            this.rootTemplates = rootTemplates;
            elementCount = 0;
        }
    }

    /**
     * cancels any executing search
     */
    public void cancelSearch() {
        executorService.shutdownNow();
        try {
            executorService.awaitTermination(1, TimeUnit.SECONDS);
        }
        catch (InterruptedException ie) {
            executorService.shutdownNow();
            Thread.currentThread().interrupt();
        }
        searchListener.searchCanceled();
    }

    /**
     * cancels any executing search and begins a new search for
     * <code>targetText</code>
     *
     * @param targetText the text for which to search
     * @param caseSensitive enables case sensitive searching
     * @param regularExpression enables regular expression searching
     */
    public void search(final String targetText, final boolean caseSensitive,
      final boolean regularExpression) {

        final SearchFunction searchFunction = regularExpression ?

            new SearchFunction() {

                final Pattern pattern = Pattern.compile(
                  targetText, caseSensitive ? 0 : Pattern.CASE_INSENSITIVE);

                @Override
                public boolean search(SieTemplate sieTemplate) {
                    return pattern.matcher(sieTemplate.toString()).find();
                }

            } :

            new SearchFunction() {

                String modifiedTargetText = caseSensitive ? targetText : targetText.toLowerCase();

                @Override
                public boolean search(SieTemplate sieTemplate) {
                    String text = sieTemplate.toString();
                    if (!caseSensitive) {
                        text = text.toLowerCase();
                    }
                    return text.contains(modifiedTargetText);
                }
                
            };

        cancelSearch();
        count = 0;
        threads = Runtime.getRuntime().availableProcessors();
        propertyChangeListener.propertyChange(new PropertyChangeEvent(this, "progress", 0, 0));
        final ConcurrentLinkedQueue<SieTemplate> roots = new ConcurrentLinkedQueue<SieTemplate>(rootTemplates);
        executorService = Executors.newFixedThreadPool(threads);

        for (int i = 0; i < threads; ++i) {
            executorService.submit(new SwingWorker<Void, SieTemplate>() {

                /** the source of a failed search */
                private Exception exception;

                {
                addPropertyChangeListener(propertyChangeListener);
                }

                /**
                 * In a background Worker thread, scan for variables that match the input string.
                 */
                @Override
                public Void doInBackground() {
                    SieTemplate rootTemplate;
                    while ((rootTemplate = roots.poll()) != null) {
                        for (SieTemplate variable : rootTemplate) {
                            if (Thread.interrupted()) {
                                cancel(false);
                                return null;
                            }
                            try {
                                publish(searchFunction.search(variable) ? variable : (SieTemplate)null);
                            }
                            catch (Exception e) {
                                exception = e;
                                return null;
                            }
                        }
                    }
                    return null;
                }

                @Override
                public void process(List<SieTemplate> variables) {
                    /**
                     * Since this method is executed on the Event Dispatch Thread
                     * whenever it gets around to it, make sure the this
                     * SwingWorker wasn't cancelled in the meantime.
                     */
                    if (!isCancelled()) {
                        ArrayList<SieTemplate> matches = new ArrayList<SieTemplate>();
                        for (SieTemplate variable : variables) {
                            if (variable != null) {
                                matches.add(variable);
                            }
                            ++count;
                        }
                        if (count > elementCount) {
                            long old = elementCount;
                            elementCount = count;
                            propertyChangeListener.propertyChange(new PropertyChangeEvent(Searcher.this, "count", old, elementCount));
                        }
                        else {
                            setProgress((int)(100 * count / elementCount));
                        }
                        if (!matches.isEmpty()) {
                            searchListener.matchFound(matches);
                        }
                    }
                }

                @Override
                public void done() {
                    if (!isCancelled()) {
                        if (exception != null) {
                            executorService.shutdownNow();
                            searchListener.searchFailed(exception);
                        }
                        else if (--threads == 0) {
                            executorService.shutdownNow();
                            setProgress(100);
                            searchListener.searchComplete();
                        }
                    }
                }

            });
        }
    }

    /**
     * This interface allows the search criteria to be independent
     * of the search loop.
     */
    private interface SearchFunction {
        /**
         * returns whether or not the search was successful
         *
         * @param sieTemplate the variable to search
         */
        public boolean search(SieTemplate sieTemplate);
    };

}
