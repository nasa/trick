package trick.sie.utils;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashSet;
import java.util.Set;
import java.util.regex.Pattern;

import javax.swing.event.TreeModelEvent;
import javax.swing.event.TreeModelListener;
import javax.swing.tree.TreePath;
import javax.swing.tree.TreeModel;

/**
 * enables filtering of a TreeModel
 *
 * @author Derek Bankieris
 */
public class TreeModelExclusionFilter extends TreeModelFilter {
   
    /** regular expressions which, when matched, filter out a node */
    private final HashSet<Pattern> patterns = new HashSet<Pattern>();

    /**
     * constructor
     *
     * @param sourceModel the target model the be sorted
     */
    public TreeModelExclusionFilter(TreeModel sourceModel) {
        super(sourceModel);
    }

    /**
     * creates an entry in the cache for <code>node</code> if it does not already exist
     *
     * @param node the node to cache
     */
    protected void updateCache(Object node) {
        if (!cache.containsKey(node)) {
            ArrayList<Object> children = new ArrayList<Object>();
            outer:
            for (Object child : getSourceChildren(node)) {
                for (Pattern pattern : patterns) {
                    if (pattern.matcher(child.toString()).find()) {
                        continue outer;
                    }
                }
                children.add(child);
            }
            cache.put(node, children);
        }
    }

    @Override
    public Object getChild(Object parent, int index) {
        if (!patterns.isEmpty()) {
            updateCache(parent);
            return cache.get(parent).get(index);
        }
        return sourceModel.getChild(parent, index);
    }

    @Override
    public int getChildCount(Object parent) {
        if (!patterns.isEmpty()) {
            updateCache(parent);
            return cache.get(parent).size();
        }
        return sourceModel.getChildCount(parent);
    }

    @Override
    public int getIndexOfChild(Object parent, Object child) {
        if (!patterns.isEmpty()) {
            updateCache(parent);
            return cache.get(parent).indexOf(child);
        }
        return sourceModel.getIndexOfChild(parent, child);
    }

    /**
     * adds <code>pattern</code> to the list of exclusion patterns if it is not already present
     *
     * @param pattern the regular expression which, when matched, filters out a node
     */
    public void addPattern(Pattern pattern) {
        for (Pattern existingPattern : patterns) {
            if (existingPattern.toString().equals(pattern.toString())) {
                return;
            }
        }
        patterns.add(pattern);
        fireRootChangedEvent();
    }

    /**
     * removes <code>pattern</code> from the list of exclusion patterns, if present
     *
     * @param pattern the regular expression to stop filtering against
     */
    public void removePattern(Pattern pattern) {
        for (Pattern existingPattern : patterns) {
            if (existingPattern.toString().equals(pattern.toString())) {
                patterns.remove(existingPattern);
                fireRootChangedEvent();
                return;
            }
        }
    }

    /**
     * removes all filtering regular expressions
     */
    public void removeAllPatterns() {
        patterns.clear();
        fireRootChangedEvent();
    }


    /**
     * returns the regular expressions being used to filter nodes
     *
     * @return the patterns which, when matched, exclude a node
     */
    public Set<Pattern> getPatterns() {
        return Collections.unmodifiableSet(patterns);
    }

}
