package trick.sie.utils;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;

import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

import org.w3c.dom.Element;
import org.w3c.dom.NodeList;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;

/**
 * parses the S_sie.resource file with a DOM XML parser.
 *
 * @author Derek Bankieris
 */
public class SieResourceDomParser {

    /**
     * Programmer note:
     *
     * NodeList.getLength() takes a non-trivial amount of time for large lists.
     * Therefore, it is not used directly in the condition of for loops, which
     * would cause it to be called each cycle. Instead, it is called once before
     * entering the loop, and the constant result is used in the condition.
     */

    /**
     * parses the provided S_sie.resource file and returns the top level instances
     *
     * @param inputSource the source containing the S_sie.resource file
     *
     * @return the simulation's root instances
     * @throws ParserConfigurationException ParserConfigurationException
     * @throws SAXException SAXException
     * @throws IOException IOException
     */
    public static Collection<SieTemplate> parse(InputSource inputSource)
      throws ParserConfigurationException, SAXException, IOException {

        // parse the source
        final Element documentElement =
          DocumentBuilderFactory.newInstance().newDocumentBuilder().parse(inputSource).getDocumentElement();

        // the top level instances
        final ArrayList<SieTemplate> rootInstances = new ArrayList<SieTemplate>();

        // all instances
        final ArrayList<SieTemplate> instances = new ArrayList<SieTemplate>() {{
            NodeList topLevelInstances = documentElement.getElementsByTagName("top_level_object");
            int length = topLevelInstances.getLength();
            for (int i = 0; i < length; ++i) {
                SieTemplate template = createTemplate((Element)topLevelInstances.item(i));
                add(template);
                rootInstances.add(template);
            }
        }};

        // maps class members to their class's name
        HashMap<String, ArrayList<SieTemplate>> typeHashMap = new HashMap<String, ArrayList<SieTemplate>>() {{
            NodeList classes = documentElement.getElementsByTagName("class");
            int length = classes.getLength();
            for (int i = 0; i < length; ++i) {
                Element clazz = (Element)classes.item(i);
                ArrayList<SieTemplate> hashEntry = new ArrayList<SieTemplate>();
                put(clazz.getAttribute("name"), hashEntry);
                NodeList members = clazz.getElementsByTagName("member");
                int memberCount = members.getLength();
                for (int j = 0; j < memberCount; ++j) {
                    SieTemplate template  = createTemplate((Element)members.item(j));
                    instances.add(template);
                    hashEntry.add(template);
                }
            }
        }};

        // map SieEnumerations to their names
        HashMap<String, SieEnumeration> enumerationHashMap = getDocumentEnumerationMap(documentElement);

        // add children to all instances
        for (SieTemplate template : instances) {
            if (typeHashMap.containsKey(template.typeName)) {
                ArrayList<SieTemplate> children = typeHashMap.get(template.typeName);
                if (children.isEmpty()) {
                    template.children.add(SieTemplate.noManagedMembersTemplate);
                }
                else {
                    template.children = children;
                }
            }
            else if (enumerationHashMap.containsKey(template.typeName)) {
                template.enumeration = enumerationHashMap.get(template.typeName);
            }
        }

        return rootInstances;
    }
    
    /**
     * parses the provided S_sie.resource file and returns the enumerations
     *
     * @param inputSource the source containing the S_sie.resource file
     *
     * @return the simulation's enumerations
     * @throws ParserConfigurationException ParserConfigurationException
     * @throws SAXException SAXException
     * @throws IOException IOException
     */
    public static HashMap<String, SieEnumeration> getSieEnumerationMap(InputSource inputSource)
      throws ParserConfigurationException, SAXException, IOException {
        return getDocumentEnumerationMap(DocumentBuilderFactory.newInstance().newDocumentBuilder().parse(inputSource).getDocumentElement());
    }

    /**
     * returns the enumerations in the document
     *
     * @return the simulation's enumerations
     */
    private static HashMap<String, SieEnumeration> getDocumentEnumerationMap(final Element documentElement) {
        return new HashMap<String, SieEnumeration>() {{
            NodeList enumerations = documentElement.getElementsByTagName("enumeration");
            int length = enumerations.getLength();
            for (int i = 0; i < length; ++i) {
                Element enumeration = (Element)enumerations.item(i);
                SieEnumeration sieEnumeration = new SieEnumeration(enumeration.getAttribute("name"));
                put(sieEnumeration.name, sieEnumeration);
                NodeList entries = enumeration.getElementsByTagName("pair");
                int entryCount = entries.getLength();
                for (int j = 0; j < entryCount; ++j) {
                    Element entry = (Element)entries.item(j);
                    sieEnumeration.pairs.put(entry.getAttribute("label"), entry.getAttribute("value"));
                }
            }
        }};
    }
    
    private static SieTemplate createTemplate(final Element element) {
        return new SieTemplate() {{
            parameter = element.getAttribute("name");
            typeName =  element.getAttribute("type");
            ioType =  element.getAttribute("io_attributes");
            units = element.getAttribute("units");
            description = element.getAttribute("description");
            NodeList dims = element.getElementsByTagName("dimension");
            dimensions = new int[dims.getLength()];
            for (int i = 0; i < dimensions.length; ++i) {
                dimensions[i] = Integer.parseInt(dims.item(i).getTextContent());
            }
            
            // Only top level instances have this field.
            try {
                dynamicAllocation = Integer.parseInt(element.getAttribute("alloc_memory_init")) == 0;
            }
            catch (Exception ignored) {}
        }};
    }

}
