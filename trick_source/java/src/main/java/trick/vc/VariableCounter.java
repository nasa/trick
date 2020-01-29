package trick.vc;

import java.io.File;
import java.util.ArrayDeque;
import java.util.Collection;
import java.util.HashMap;
import java.util.List;

import joptsimple.OptionException;
import joptsimple.OptionParser;
import joptsimple.OptionSet;
import joptsimple.OptionSpec;

import org.xml.sax.InputSource;

import trick.sie.utils.SieResourceDomParser;
import trick.sie.utils.SieTemplate;

public class VariableCounter {

    private static ArrayDeque<String> typePath = new ArrayDeque<String>();

    private static boolean expandArrays;

    private static OptionParser optionParser = new OptionParser() {{
        accepts("a", "expand fixed arrays");
    }};

    public static void main(String args[]) {
        List<?> arguments;
        try {
            OptionSet options = optionParser.parse(args);
            expandArrays = options.has("a");
            arguments = options.nonOptionArguments();
            if (arguments.isEmpty()) {
                printHelp();
                return;
            }
        }
        catch (OptionException exception) {
            printHelp();
            return;
        }

        try {
            long count = 0;
            String file = arguments.get(0).toString();
            System.out.println("Parsing " + file);
            Collection<SieTemplate> rootInstances = SieResourceDomParser.parse(new InputSource(new File(file).toURI().toString()));
            System.out.println("Compile time Trick-visible variables per top level instance:");
            System.out.println("Fixed arrays expanded = " + expandArrays);
            for (SieTemplate rootInstance : rootInstances) {
                long result = countVariables(rootInstance);
                System.out.format("%,15d - " + rootInstance + "%n", result);
                count += result;
            }
            System.out.format("Total: %,d%n", count);
        }
        catch (Exception exception) {
            exception.printStackTrace();
        }
    }

    public static long countVariables(SieTemplate template) {
        if (!template.isTrickManaged() || typePath.contains(template.typeName)) {
            return 0;
        }
        long count;
        if (template.children.isEmpty()) {
            count = 1;
        }
        else {
            count = 0;
            typePath.add(template.typeName);
            HashMap<String, Long> hashMap = new HashMap<String, Long>();
            for (SieTemplate child : template.children) {
                long childCount;
                Long hashedCount = hashMap.get(child.typeName);
                if (hashedCount != null) {
                    childCount = hashedCount;
                }
                else {
                    childCount = countVariables(child);
                    hashMap.put(child.typeName, childCount);
                }
                if (expandArrays) {
                    for (int dimension : child.dimensions) {
                        if (dimension > 0) {
                            childCount *= dimension;
                        }
                    }
                }
                count += childCount;
            }
            typePath.removeLast();
        }
        return count;
    }

    static void printHelp() {
        try {
            System.out.println("use: java trick.vc.VariableCounter [options] sieFile");
            optionParser.printHelpOn(System.out);
        }
        catch (Exception e) {
            e.printStackTrace();
        }
    }

};
