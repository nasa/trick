
package trick.sie.utils;

import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.Iterator;

/**
 * represents a template entry in the S_sie.resource file. This is the
 * basic element from which the variable hierarchy is constructed.
 */
public class SieTemplate implements Comparable<SieTemplate>, Iterable<SieTemplate> {

    /** template indicating the parent template contains no members managed by Trick */
    public static final SieTemplate noManagedMembersTemplate =
      new SieTemplate("(no Trick-managed members)", "", 0); 

    // XML fields
    public String parameter;
    public String typeName;
    public String ioType;
    public String description;
    public String units;
    public int[] dimensions;
    public boolean dynamicAllocation;

    /** contained members */
    public ArrayList<SieTemplate> children = new ArrayList<SieTemplate>();

    /** Will be null if this template is not an enumeration. */
    public SieEnumeration enumeration;

    /**
     * default constructor
     */
    public SieTemplate() {}

    /**
     * constructor with specified information
     *
     * @param parameter the XML parameter field
     * @param typeName the XML type_name field
     * @param numDimensions the XML num_dimensions field
     */
    public SieTemplate(String parameter, String typeName, int numDimensions) {
        this.parameter = parameter;
        this.typeName = typeName;
        dimensions = new int[numDimensions];
    }

    /**
     * creates a new copy of specified SieTemplate without its children information
     * @param sieTemplate object to copy
     */
    public SieTemplate(SieTemplate sieTemplate) {
        description = sieTemplate.description;
        parameter = sieTemplate.parameter;
        typeName = sieTemplate.typeName;
        ioType = sieTemplate.ioType;
        units = sieTemplate.units;
        dimensions = sieTemplate.dimensions;
        enumeration = sieTemplate.enumeration;
        dynamicAllocation = sieTemplate.dynamicAllocation;
    }

    @Override
    public String toString() {
        String result = parameter;
        for (int dimension : dimensions) {
            result += "[" + dimension + "]";
        }
        return result;
    }

    /**
     * returns text suitable for display in a tool tip, or <code>null</code> if
     * this template is not managed by Trick
     *
     * @return a descrption formatted for a tool tip
     */
    public String getToolTipText() {
        return
          isTrickManaged() ?
          "<html>" +
          typeName + " " + toString() + " (" + units + ")<br>" +
          description +
          "</html>" : null;
    }

    @Override
    public int compareTo(SieTemplate template) {
        if (parameter != null) {
            return parameter.compareTo(template.parameter);
        }
        else if (template.parameter != null) {
            return -1;
        }
        return 0;
    }

    /**
     * returns whether or not the instance represented by this template is an enumeration
     *
     * @return <code>true</code> is this is an enumeration
     */
    public boolean isEnumerated() {
        return enumeration != null;
    }

    /**
     * returns whether or not this represents a Trick-managed template
     *
     * @return <code>true</code> if Trick manages this template
     */
    public boolean isTrickManaged() {
        return this != noManagedMembersTemplate;
    }

    /**
     * returns an iterator over the leaf template paths rooted at this template
     */
    public Iterator<SieTemplate> iterator() {
        return new Iterator<SieTemplate>() {

            /** consituent iterators */
            ArrayDeque<Iterator<SieTemplate>> iterators = new ArrayDeque<Iterator<SieTemplate>>() {{
                add(children.iterator());
            }};

            /** current leaf template path */
            ArrayDeque<SieTemplate> templatePath = new ArrayDeque<SieTemplate>() {{
                add(SieTemplate.this);
            }};

            /** current leaf type path */
            ArrayDeque<String> typePath = new ArrayDeque<String>() {{
                add(SieTemplate.this.typeName);
            }};

            {
            if (!children.isEmpty()) {
                advance();
            }
            }

            @Override
            public boolean hasNext() {
                return !templatePath.isEmpty();
            }

            @Override
            public SieTemplate next() {
                SieTemplate result = new SieTemplate(templatePath.getLast());
                result.parameter = "";
                for (SieTemplate sieTemplate : templatePath) {
                    result.parameter += sieTemplate + ".";
                }

                // Trim trailing "." and any indices.
                result.parameter = result.parameter.substring(0, result.parameter.length() - 1);
                for (int dummy : result.dimensions) {
                    result.parameter = result.parameter.substring(0, result.parameter.lastIndexOf('['));
                }

                advance();
                return result;
            }

            @Override
            public void remove() throws UnsupportedOperationException {
                throw new UnsupportedOperationException();
            }

            /**
             * advance the iterator position one element
             */
            private void advance() {
                while (!iterators.isEmpty()) {
                    Iterator<SieTemplate> deepestIterator = iterators.peek();
                    if (!deepestIterator.hasNext()) {
                        iterators.pop();
                        templatePath.removeLast();
                        typePath.removeLast();
                    }
                    else {
                        SieTemplate template = deepestIterator.next();
                        if (template.isTrickManaged() && !typePath.contains(template.typeName)) {
                            templatePath.add(template);
                            typePath.add(template.typeName);
                            iterators.push(template.children.iterator());
                            if (template.children.isEmpty()) {
                                return;
                            }
                        }
                    }
                }
            }

        };
    }

}
