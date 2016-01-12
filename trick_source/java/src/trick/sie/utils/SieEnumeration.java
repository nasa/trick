package trick.sie.utils;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.HashMap;

import javax.xml.bind.annotation.XmlElement;
import javax.xml.bind.annotation.XmlID;
import javax.xml.bind.annotation.XmlTransient;
import javax.xml.bind.annotation.XmlType;

/**
 * represents an enumeration in the S_sie.resource file
 *
 * @author Derek Bankieris
 */
@XmlType(name = "enumeration")
public class SieEnumeration implements Comparable<SieEnumeration>, Serializable {

    private static final long serialVersionUID = 1585038205339265319L;

    /** the enumerated type name */
    @XmlElement(required = true)
    @XmlID
    public String name;

    /* values of the enumerated type */
    @XmlTransient
    public ArrayList<String> enumVals;

    /** maps the labels to their corresponding values */
    public final HashMap<String, String> pairs;

    /** most recently parsed label */
    private String nextLabel;

    /** most recently parsed value */
    private String nextValue;

    /**
     * provided for JAXB compatibility
     */
    public SieEnumeration() {
        this(null);
    }

    /**
     * constructs an SieEnumeration with the given name
     *
     * @param name the name of the enumeration
     */
    public SieEnumeration(String name) {
        this.name = name;
        enumVals = new ArrayList<String>();
        pairs = new HashMap<String, String>();
    }

    /**
     * sets the values for this enumeration
     *
     * @param values the values to be set for this enumeration
     */
    public void setEnumVals(String[] values) {
        for (String value : values) {
            enumVals.add(value);
        }
    }

    /**
     * Adds the given label to the pending label-value pair. If this
     * completes the pair, it is added to this enumeration.
     *
     * @param label the label to be added
     */
    public void addLabel(String label) {
        enumVals.add(label);
        if (nextValue != null) {
            pairs.put(label, nextValue);
            nextLabel = nextValue = null;
        }
        else {
            nextLabel = label;
        }
    }

    /**
     * adds the given value to the pending label-value pair. If this
     * completes the pair, it is added to this enumeration.
     *
     * @param value the value to be added
     */
    public void addValue(String value) {
        if (nextLabel != null) {
            pairs.put(nextLabel, value);
            nextLabel = nextValue = null;
        }
        else {
            nextValue = value;
        }
    }

    @Override
    public String toString() {
        return name;
    }

    /**
     * returns the result of comparing the enumerations' names as implemented
     * by <code>String.compareTo</code>
     */
    @Override
    public int compareTo(SieEnumeration enumeration) {
        return name.compareTo(enumeration.name);
    }

    /**
     * Equality among <code>SieEnumeration</code>s requires only that their names
     * be equal as implemented by <code>String.equals</code>.
     *
     * @param object the object to compare
     *
     * @return the equality of <code>object</code> to this <code>SieEnumeration</code>
     */
    @Override
    public boolean equals(Object object) {
        return object != null && object instanceof SieEnumeration &&
          name.equals(((SieEnumeration)object).name);
    }

    /**
     * a <code>SieEnumeration</code>'s hashcode is equal to the hashcode of its name
     *
     * @return a hashcode for this object
     */
    @Override
    public int hashCode() {
        return name.hashCode();
    }

}
