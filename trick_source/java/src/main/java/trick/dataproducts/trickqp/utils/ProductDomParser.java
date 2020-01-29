
//========================================
//    Package
//========================================
package trick.dataproducts.trickqp.utils;

//========================================
//    Imports
//========================================
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

import org.w3c.dom.Element;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;

/**
 * This XML parser uses Java XML DOM parser to parse a XML file.
 *
 * @author Hong Chen
 * @since Trick 10
 */
public class ProductDomParser {
    //========================================
    //    Constructors
    //========================================
    public ProductDomParser(){
    }

    //========================================
    //    Methods
    //========================================
    /**
     * Sets up the object data for based on the root element.
     * @param file the file name to read
     * @return the top level product object
     * @throws ParserConfigurationException ParserConfigurationException
     * @throws SAXException SAXException
     * @throws IOException IOException
     */
    public static Product parse(File file)
     throws ParserConfigurationException, SAXException, IOException {
        // parse the source
        DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance() ;

        // Ignore the Session.dtd reference in the session xml file.
        dbf.setFeature("http://apache.org/xml/features/nonvalidating/load-dtd-grammar", false);
        dbf.setFeature("http://apache.org/xml/features/nonvalidating/load-external-dtd", false);

        final Element elem = dbf.newDocumentBuilder().parse(file).getDocumentElement();

        Product productObj = new Product(elem.getAttribute("version"));

        productObj.setForegroundColorCode(elem.getAttribute("foreground_color"));
        productObj.setBackgroundColorCode(elem.getAttribute("background_color"));

        setCommonData(elem, productObj);

        // Retrieve all page elements
        NodeList childElements = elem.getElementsByTagName("page");
        if (childElements != null && childElements.getLength() > 0) {
            for (int i = 0 ; i < childElements.getLength(); i++) {
                //get the element
                Element pageElem = (Element)childElements.item(i);
                ProductPage pageObj = getPage(pageElem);
                if (pageObj != null) {
                    productObj.addPage(pageObj);
                }
            }
        }

        // Retrive all table elements
        childElements = elem.getElementsByTagName("table");
        if (childElements != null && childElements.getLength() > 0) {
            for (int i = 0 ; i < childElements.getLength(); i++) {
                Element tableElem = (Element)childElements.item(i);
                ProductTable tableObj = getTable(tableElem);
                if (tableObj != null) {
                    productObj.addTable(tableObj);
                }
            }
        }

        // Retrive all extrn elements
        childElements = elem.getElementsByTagName("extfn");
        if (childElements != null && childElements.getLength() > 0) {
            for (int i = 0 ; i < childElements.getLength(); i++) {
                Element extfnElem = (Element)childElements.item(i);
                ProductExternalFunction extfnObj = getExternalFunction(extfnElem);
                if (extfnObj != null) {
                    productObj.addExternalFunction(extfnObj);
                }
            }
        }

        return productObj;
    }

    /**
     * Gets a {@link ProductPage} instance based on the page element.
     *
     * @param pageElem    The page element.
     *
     * @return An instance of {@link ProductPage}
     */
    private static ProductPage getPage(Element pageElem) {
        ProductPage pageObj = new ProductPage();

        pageObj.setForegroundColorCode(pageElem.getAttribute("foreground_color"));
        pageObj.setBackgroundColorCode(pageElem.getAttribute("background_color"));

        pageObj.setHcells(pageElem.getAttribute("hcells"));
        pageObj.setVcells(pageElem.getAttribute("vcells"));

        // TODO: other attributes

        setCommonData(pageElem, pageObj);

        // go through all plots in this page
        NodeList childElements = pageElem.getElementsByTagName("plot");
        if (childElements != null && childElements.getLength() > 0) {
            for (int i = 0 ; i < childElements.getLength(); i++) {
                //get the element
                Element plotElem = (Element)childElements.item(i);
                ProductPlot plotObj = getPlot(plotElem);
                if (plotObj != null) {
                    pageObj.addPlot(plotObj);
                }
            }
        }

        return pageObj;
    }

    /**
     * Gets an instance of {@link ProductPlot} for the specified plot element.
     *
     * @param plotElem    An instance of {@link Element} for the plot.
     *
     * @return An instance of {@link ProductPlot}.
     */
    private static ProductPlot getPlot(Element plotElem) {
        ProductPlot plotObj = new ProductPlot();

        setCommonData(plotElem, plotObj);

        plotObj.setXMin(plotElem.getAttribute("xmin"));
        plotObj.setXMax(plotElem.getAttribute("xmax"));
        plotObj.setYMin(plotElem.getAttribute("ymin"));
        plotObj.setYMax(plotElem.getAttribute("ymax"));
        plotObj.setXScale(plotElem.getAttribute("x_scale"));
        plotObj.setYScale(plotElem.getAttribute("y_scale"));

        plotObj.setForegroundColorCode(plotElem.getAttribute("foreground_color"));
        plotObj.setBackgroundColorCode(plotElem.getAttribute("background_color"));

        plotObj.setGridColor(plotElem.getAttribute("grid_color"));
        plotObj.setGrid(plotElem.getAttribute("grid"));
        plotObj.setFont(plotElem.getAttribute("font"));

        // axises
        parseAxis(plotElem, plotObj, "xaxis");
        parseAxis(plotElem, plotObj, "yaxis");
        parseAxis(plotElem, plotObj, "zaxis");

        // curve
        parseCurve(plotElem, plotObj);

        return plotObj;
    }

    /**
     * Gets an instance of {@link ProductTable} for the specified table element.
     * 
     * @param tableElem    An instance of {@link Element} for the table.
     * 
     * @return An instance of {@link ProductTable}.
     */
    private static ProductTable getTable(Element tableElem) {
        ProductTable tableObj = new ProductTable();

        setCommonData(tableElem, tableObj);

        tableObj.setForegroundColorCode(tableElem.getAttribute("foreground_color"));
        tableObj.setBackgroundColorCode(tableElem.getAttribute("background_color"));

        parseColumn(tableElem, tableObj);

        return tableObj;
    }

    /**
     * Gets an instance of {@link ProductExternalFunction} for the specified extfn element.
     * 
     * @param extfnElem    An instance of {@link Element} for the external function or program.
     * 
     * @return An instance of {@link ProductTable}.
     */
    private static ProductExternalFunction getExternalFunction(Element extfnElem) {
        ProductExternalFunction extfnObj = new ProductExternalFunction();
        extfnObj.setName(getTextValue(extfnElem, "fname"));

        parseInputs(extfnElem, extfnObj);
        parseOutputs(extfnElem, extfnObj);

        return extfnObj;
    }

    /**
     * Helper method for setting some common data in certain elements.
     */
    private static void setCommonData(Element elem, CommonProduct commonProduct) {
        commonProduct.setTitle(getTextValue(elem, "title"));

        // Need to have individual try...catch for each element, otherwise
        // if the first one is null, the rest would be ignored.
        try {
            commonProduct.setTStart(Double.parseDouble(getTextValue(elem, "tstart")));
        } catch (NullPointerException npe) {
            // meaning that any optional tstart couldn't be found
            // in the XML file, don't need to do anything.
        }
        try {
            commonProduct.setTStop(Double.parseDouble(getTextValue(elem, "tstop")));
        } catch (NullPointerException npe) {
            // meaning that any optional  tstop couldn't be found
            // in the XML file, don't need to do anything.
        }
        try {
            commonProduct.setFrequency(Double.parseDouble(getTextValue(elem, "frequency")));
        } catch (NullPointerException npe) {
            // meaning that any optional frequency couldn't be found
            // in the XML file, don't need to do anything.
        }
    }


    /**
     * Helper method for parsing xaxis, yaxis, and zaxis.
     */
    private static void parseAxis(Element plotElem, ProductPlot plotObj, String axis) {
        NodeList childElements = plotElem.getElementsByTagName(axis);
        if (childElements != null && childElements.getLength() > 0) {
            for (int i = 0; i < childElements.getLength(); i++) {
                Element axisElem = (Element)childElements.item(i);
                ProductAxis axisObj = getAxis(axisElem);
                if (axisObj != null) {
                    if (axis.equals("xaxis")) {
                        axisObj.setType(ProductAxis.X_AXIS);
                        plotObj.setXAxis(axisObj);
                    } else if (axis.equals("yaxis")) {
                        axisObj.setType(ProductAxis.Y_AXIS);
                        plotObj.setYAxis(axisObj);
                    } else if (axis.equals("zaxis")) {
                        axisObj.setType(ProductAxis.Z_AXIS);
                        plotObj.setZAxis(axisObj);
                    }
                }
            }
        }
    }

    /**
     * Helper method for parsing curve.
     */
    private static void parseCurve(Element plotElem, ProductPlot plotObj) {
        ProductCurve curve = null;
        NodeList childElements = plotElem.getElementsByTagName("curve");
        if (childElements != null && childElements.getLength() > 0) {
            for (int i = 0; i < childElements.getLength(); i++) {
                curve = new ProductCurve();
                Element curveElem = (Element)childElements.item(i);

                // check to see if there is any <varcase> ... </varcase>
                NodeList varcaseElements = curveElem.getElementsByTagName("varcase");
                if (varcaseElements != null && varcaseElements.getLength() > 0) {
                    for (int j = 0; j < varcaseElements.getLength(); j++) {
                        Element varcaseElem = (Element)varcaseElements.item(j);
                        NodeList varElements = varcaseElem.getElementsByTagName("var");
                        ProductVarcase varcase = new ProductVarcase();
                        parseVarList(varElements, varcaseElem, varcase);
                        curve.addVarcase(varcase);
                    }
                } else {
                    // otherwise retrieve <var> ...</var>
                    NodeList varElements = curveElem.getElementsByTagName("var");
                    parseVarList(varElements, curveElem, curve);
                }
                plotObj.addCurve(curve);
            }
        }
    }


    /**
     * Helper method for parsing var list that has X and Y with an optional Z.
     * X, Y and Z var are in order in the XML file.
     */
    private static void parseVarList(NodeList varList, Element parentElem, Object parentObj) {
        if (varList != null && varList.getLength() > 0) {
            for (int i = 0; i < varList.getLength(); i++) {
                Element varElem = (Element)varList.item(i);
                switch (i) {
                    case 0:
                        if (parentObj instanceof ProductCurve) {
                            ((ProductCurve)parentObj).setX(getVar(varElem));
                        } else if (parentObj instanceof ProductVarcase) {
                            ((ProductVarcase)parentObj).setX(getVar(varElem));
                        }
                        break;
                    case 1:
                        if (parentObj instanceof ProductCurve) {
                            ((ProductCurve)parentObj).setY(getVar(varElem));
                        } else if (parentObj instanceof ProductVarcase) {
                            ((ProductVarcase)parentObj).setY(getVar(varElem));
                        }
                        break;
                    case 2:
                        if (parentObj instanceof ProductCurve) {
                            ((ProductCurve)parentObj).setZ(getVar(varElem));
                        } else if (parentObj instanceof ProductVarcase) {
                            ((ProductVarcase)parentObj).setZ(getVar(varElem));
                        }
                        break;
                }
            }
        }
    }

    /**
     * Helper method for parsing column data of a table.
     */
    private static void parseColumn(Element tableElem, ProductTable tableObj) {
        ProductColumn column = null;
        NodeList childElements = tableElem.getElementsByTagName("column");

        if (childElements != null && childElements.getLength() > 0) {
            for (int i = 0; i < childElements.getLength(); i++) {
                column = new ProductColumn();
                Element columnElem = (Element)childElements.item(i);

                // for <var> ...</var> and there is only one var for each column
                NodeList varElements = columnElem.getElementsByTagName("var");
                if (varElements != null && varElements.getLength() > 0) {
                    Element varElem = (Element)varElements.item(0);
                    column.setVar(getVar(varElem));
                }

                // for label, units, format
                column.setLabel(getTextValue(columnElem, "label"));
                column.setUnits(getTextValue(columnElem, "units"));
                column.setFormat(columnElem.getAttribute("format"));

                tableObj.addColumn(column);
            }
        }
    }

    /**
     * Helper method for parsing inputs data of extfn element.
     */
    private static void parseInputs(Element extfnElem, ProductExternalFunction extfnObj) {
        NodeList childElements = extfnElem.getElementsByTagName("inputs");

        if (childElements != null && childElements.getLength() > 0) {
            // only one <inputs> element for each <extfn>
            Element inputsElem = (Element)childElements.item(0);

            NodeList varElements = inputsElem.getElementsByTagName("var");
            if (varElements != null && varElements.getLength() > 0) {
                for (int i = 0; i < varElements.getLength(); i++) {
                    Element varElem = (Element)varElements.item(i);
                    extfnObj.addInput(getVar(varElem));
                }
            }
        }
    }

    /**
     * Helper method for parsing outputs data of extfn element.
     */
    private static void parseOutputs(Element extfnElem, ProductExternalFunction extfnObj) {
        ProductMeasurement measurement = null;
        NodeList childElements = extfnElem.getElementsByTagName("outputs");

        if (childElements != null && childElements.getLength() > 0) {
            // only one <outputs> element for each <extfn>
            Element outputsElem = (Element)childElements.item(0);

            // for <measurement> ...</measurement> and there is only one var&units for each <measurement>
            NodeList measurementElements = outputsElem.getElementsByTagName("measurement");
            if (measurementElements != null && measurementElements.getLength() > 0) {
                for (int i = 0; i < measurementElements.getLength(); i++) {
                    Element measurementElem = (Element)measurementElements.item(i);
                    measurement = new ProductMeasurement(getTextValue(measurementElem, "var"));
                    measurement.setUnits(getTextValue(measurementElem, "units"));
                    extfnObj.addOutput(measurement);
                }
            }
        }
    }

    /**
     * Gets an instance of {@link ProductAxis} for the specified axis element.
     *
     * @param axisElem    An instance of {@link Element} for the axis.
     *
     * @return An instance of {@link ProductAxis}.
     */
    private static ProductAxis getAxis(Element axisElem) {
        ProductAxis axisObj = new ProductAxis();
        axisObj.setLabel(getTextValue(axisElem, "label"));
        axisObj.setFormat(axisElem.getAttribute("format"));
        return axisObj;
    }

    /**
     * Gets an instance of {@link ProductVar} for the specified var element.
     */
    private static ProductVar getVar(Element varElem) {
        ProductVar varObj = new ProductVar(varElem.getFirstChild().getNodeValue());
        varObj.setLabel(varElem.getAttribute("label"));
        varObj.setFromUnits(varElem.getAttribute("from_units"));
        varObj.setUnits(varElem.getAttribute("units"));
        varObj.setBias(varElem.getAttribute("bias"));
        varObj.setScale(varElem.getAttribute("scale"));
        varObj.setMax(varElem.getAttribute("max"));
        varObj.setSymbolStyle(varElem.getAttribute("symbol_style"));
        varObj.setSymbolSize(varElem.getAttribute("symbol_size"));
        varObj.setLineStyleText(varElem.getAttribute("line_style"));
        varObj.setLineColorCode(varElem.getAttribute("line_color"));
        varObj.setGnuplotLineStyle(varElem.getAttribute("gnuplot_line_style"));
        return varObj;
    }

    protected static String getTextValue(Element elem, String tagName) {
        String textVal = null;
        NodeList nl = elem.getElementsByTagName(tagName);
        if(nl != null && nl.getLength() > 0) {
            Element el = (Element)nl.item(0);
            // making sure that the el is elem's direct child
            if (((Element)el.getParentNode()).equals(elem)) {
                textVal = el.getFirstChild().getNodeValue();
            }
        }

        return textVal;
    }

    protected static List<String> getTextListValue(Element elem, String tagName) {
        List<String> textValList = null;
        // the occurrence of tagName is one or more
        NodeList nl = elem.getElementsByTagName(tagName);
        if (nl != null && nl.getLength() > 0) {
            textValList = new ArrayList<String>();
            for (int i = 0; i < nl.getLength(); i++) {
                Element theNode = (Element)nl.item(i);
                textValList.add(theNode.getFirstChild().getNodeValue());
            }
        }

        return textValList;
    }

}
