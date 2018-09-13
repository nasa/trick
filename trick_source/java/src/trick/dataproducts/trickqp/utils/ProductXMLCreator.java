
//========================================
//	Package
//========================================
package trick.dataproducts.trickqp.utils;

//========================================
//	Imports
//========================================
import java.util.Iterator;
import java.util.List;

import org.w3c.dom.Element;

import trick.common.utils.XMLCreator;

/**
 * This XML creator for creating {@link Product} XML file 
 * using Java API for XML Processing.
 * 
 * @author Hong Chen
 * @since Trick 10
 */
public class ProductXMLCreator extends XMLCreator {
	
	//========================================
	//	Public data
	//========================================
	
	
	
	//========================================
	//	Protected data
	//========================================
	
	
	
	//========================================
	//	Private Data
	//========================================
	
	
	
	
	//========================================
	//	Constructors
	//========================================
	public ProductXMLCreator(){
		super();
	}

	//========================================
	//	Set/Get methods
	//========================================
	
	
	
	//========================================
	//	Methods
	//========================================	
	/**
	 * This shall create the desired XML structure.
	 * Required by XMLCreator.
	 * 
	 * @return true if root object is created successfully, false otherwise.
	 * 
	 * @see trick.common.utils.XMLCreator
	 */
	@Override
	protected boolean createDOMTree() {
		//create the root element and session XML file only has one <product> element.
		Element rootElem = dom.createElement("product");
		dom.appendChild(rootElem);

		if (rootObj instanceof Product) {
			Product productObj = (Product)rootObj;
			
			// <product> attributes
			rootElem.setAttribute("version", productObj.getVersion());
			if (productObj.getForegroundColor() != null) {
				rootElem.setAttribute("foreground_color", productObj.getForegroundColorCode());
			}
			if (productObj.getBackgroundColor() != null) {
				rootElem.setAttribute("background_color", productObj.getBackgroundColorCode());
			}
			
			// for <title> ... </title>
			createChildElement(rootElem, "title", productObj.getTitle());
			// for <tstart> ... </tsart>
			createChildElement(rootElem, "tstart", productObj.getTStart());
			// for <tstop> ... </tstop>
			createChildElement(rootElem, "tstop", productObj.getTStop());
			// for <frequency> ... </frequency>
			createChildElement(rootElem, "frequency", productObj.getFrequency());
						
			// for <extfn> ...</extfn>
			List<ProductExternalFunction> functions= productObj.getFunctionList();
			if (functions != null) {
				Iterator<ProductExternalFunction> it = functions.iterator();
				while (it.hasNext()) {
					ProductExternalFunction eachFunction = it.next();					
					buildExternalFunctionElement(rootElem, eachFunction);
				}
			}
			// for <page>...</page>
			List<ProductPage> pages = productObj.getPageList();			
			if (pages != null && pages.size() > 0) {
				for (ProductPage eachPage : pages) {
				    Element pageElem = createPageElement(eachPage);
				    if (pageElem != null) {
				        rootElem.appendChild(createPageElement(eachPage));
				    }
				}
			}
			// for <table>...</table>
			List<ProductTable> tables = productObj.getTableList();
			if (tables != null && tables.size() > 0) {
				for (ProductTable eachTable : tables) {
				    Element tableElem = createTableElement(eachTable);
				    if (tableElem != null) {
					    rootElem.appendChild(tableElem);
				    }
				}
			}
			
			if (!productObj.isValid()) {
			    return false;
			}
		}
		return true;
	}
	
	/**
	 * Creates an extfn {@link Element} and appends to <product> element.
	 * 
	 * @param function	The {@link ProductExternalFunction} needs creating the XML representation.
	 * 
	 */
	private void buildExternalFunctionElement(Element parent, ProductExternalFunction function) {
	    if (!function.isValid()) {
	        return;
	    }
	    
		Element functionElem = dom.createElement("extfn");
		if (function.getName() != null && function.getName().length() >0 ) {
			// <fname> ... </fname>
			createChildElement(functionElem, "fname", function.getName());
		}
		
		// for inputs
		Element inputsElem = dom.createElement("inputs");
		for (ProductVar eachInput : function.getInputs()) {
			if (eachInput != null && !"".equals(eachInput.getName())) {
				inputsElem.appendChild(createVarElement(eachInput));
			}
		}
		functionElem.appendChild(inputsElem);
		
		// for outputs
		Element outputsElem = dom.createElement("outputs");
		for (ProductMeasurement eachOutput : function.getOutputs()) {			
			if (eachOutput != null && eachOutput.isValid()) {
			    Element measurementElem = dom.createElement("measurement");
				// <var>...</var>
				createChildElement(measurementElem, "var", eachOutput.getName());

                                // The units element is OPTIONAL and can be null,
                                // indicating that the user doesn't want units conversion.
                                String outPutUnits  = eachOutput.getUnits();
                                if (outPutUnits != null)  {
				    createChildElement(measurementElem, "units", outPutUnits);
                                }
				outputsElem.appendChild(measurementElem);
			}
		}
		functionElem.appendChild(outputsElem);
			
		parent.appendChild(functionElem);
	}
	
	/**
	 * Creates a page {@link Element} and appends to <product> element.
	 * 
	 * @param page	The {@link ProductPage} needs creating the XML representation.
	 * 
	 * @return An instance of {@link Element} the page data.
	 * 
	 */
	private Element createPageElement(ProductPage page) {
	    if (!page.isValid()) {
	        return null;
	    }
	     
		Element pageElem = dom.createElement("page");
		if (page.getTitle() != null && !("".equals(page.getTitle().trim()))) {
			// <title> ... </title>
			createChildElement(pageElem, "title", page.getTitle());
		}
		// for <tstart> ... </tsart>
		createChildElement(pageElem, "tstart", page.getTStart());
		// for <tstop> ... </tstop>
		createChildElement(pageElem, "tstop", page.getTStop());
		// for <frequency> ... </frequency>
		createChildElement(pageElem, "frequency", page.getFrequency());
		if (page.getForegroundColor() != null) {
			pageElem.setAttribute("foreground_color", page.getForegroundColorCode());
		}
		if (page.getBackgroundColor() != null) {
			pageElem.setAttribute("background_color", page.getBackgroundColorCode());
		}
		
		if (page.getHcells() != null) {
			pageElem.setAttribute("hcells", page.getHcells().toString());
		}
		if (page.getVcells() != null) {
			pageElem.setAttribute("vcells", page.getVcells().toString());
		}
		
		// for plots
		for (ProductPlot eachPlot : page.getPlotList()) {
		    if (eachPlot != null && eachPlot.isValid()) {
			    pageElem.appendChild(createPlotElement(eachPlot));
			}
	    }
			
		if (!pageElem.hasChildNodes()) {
		    pageElem = null;
		}
			
		return pageElem;
	}	
	
	/**
	 * Creates a plot {@link Element} for the specified plot data.
	 * 
	 * @param plot	An instance of {@link ProductPlot}.
	 * 
	 * @return An instance of {@link Element} to represent the plot data.
	 */
	private Element createPlotElement(ProductPlot plot) {
	    if (!plot.isValid()) {
	        return null;
	    }
		Element plotElem = dom.createElement("plot");
				
		// <title> ... </title>
		createChildElement(plotElem, "title", plot.getTitle());		
		// for <tstart> ... </tsart>
		createChildElement(plotElem, "tstart", plot.getTStart());
		// for <tstop> ... </tstop>
		createChildElement(plotElem, "tstop", plot.getTStop());
		// for <frequency> ... </frequency>
		createChildElement(plotElem, "frequency", plot.getFrequency());
		
		if (plot.getXMin() != null) {
			plotElem.setAttribute("xmin", plot.getXMin().toString());
		}
		if (plot.getXMax() != null) {
			plotElem.setAttribute("xmax", plot.getXMax().toString());
		}
		if (plot.getXScale() != null && !("".equals(plot.getXScale().trim()))) {
			plotElem.setAttribute("x_scale", plot.getXScale());
		}
		if (plot.getYMin() != null) {
			plotElem.setAttribute("ymin", plot.getYMin().toString());
		}
		if (plot.getYMax() != null) {
			plotElem.setAttribute("ymax", plot.getYMax().toString());
		}
		if (plot.getYScale() != null && !("".equals(plot.getYScale().trim()))) {
			plotElem.setAttribute("y_scale", plot.getYScale());
		}
		plotElem.setAttribute("grid", plot.getGrid());
		plotElem.setAttribute("grid_color", plot.getGridColorCode());
		
		if (plot.getForegroundColor() != null) {
			plotElem.setAttribute("foreground_color", plot.getForegroundColorCode());
		}
		
		if (plot.getBackgroundColor() != null) {
			plotElem.setAttribute("background_color", plot.getBackgroundColorCode());
		}
		
		// Don't set the optional "font" attribute if it's empty or default.
		if (plot.getFont() != null && !("".equals(plot.getFont().trim())) && !plot.getFont().equalsIgnoreCase("Default")) {
			plotElem.setAttribute("font", plot.getFont());
		}
		
		// xaxis
		if ( plot.getXAxis() != null && 
			 ((plot.getXAxis().getLabel() != null && !("".equals(plot.getXAxis().getLabel().trim())))) ) {
			plotElem.appendChild(createAxisElement(plot.getXAxis()));
		}
		// yaxis
		if ( plot.getYAxis() != null && !plot.getYAxis().isEmptyNode() ) {			
			plotElem.appendChild(createAxisElement(plot.getYAxis()));
		}
		// zaxis - future
		if ( plot.getZAxis() != null && 
			 ((plot.getZAxis().getLabel() != null && !("".equals(plot.getZAxis().getLabel().trim())))) ){
			plotElem.appendChild(createAxisElement(plot.getZAxis()));
		}
		
		//curve
		for (ProductCurve curve : plot.getCurveList()) {
			if (curve != null && curve.isValid()) {
				plotElem.appendChild(createCurveElement(curve));
			}
		}
			
		if (!plotElem.hasChildNodes()) {
		    plotElem = null;
		}
		return plotElem;
	}
	
	/**
	 * Creates an axis element based on the specified axis data.
	 * 
	 * @param axis	An instance of {@ProductAxis}.
	 * 
	 * @return	An instance of {@link Element} to represent the axis data.
	 */
	private Element createAxisElement(ProductAxis axis) {
		Element axisElem = null;
		if (axis != null) {		
			switch (axis.getType()) {
				case ProductAxis.X_AXIS:
					axisElem = dom.createElement("xaxis");
					break;
				case ProductAxis.Y_AXIS:
					axisElem = dom.createElement("yaxis");
					// for "format" attribute
					if (axis.getFormat() != null && !("".equals(axis.getFormat().trim()))) {
						axisElem.setAttribute("format", axis.getFormat());
					}	
					break;
				case ProductAxis.Z_AXIS:
					axisElem = dom.createElement("zaxis");
					break;
			}
		}
		createChildElement(axisElem, "label", axis.getLabel());
		return axisElem;
	}
	
	/**
	 * Creates a curve element based on the specified curve data.
	 * 
	 * @param curve	An instance of {@link ProductCurve}.
	 * 
	 * @return An instance of {@link Element} to represent the curve data.
	 */
	private Element createCurveElement(ProductCurve curve) {
	    if (!curve.isValid()) {
	        return null;
	    }
	    
		Element curveElem = dom.createElement("curve");
		
		// a curve either have one or more <varcase> ... </varcase>
		// or have X, Y (TODO: and Z)
		if (curve.hasVarcase()) {		
			for (ProductVarcase varcase : curve.getVarcaseList()) {
			    if (!varcase.isValid()) {
			        continue;
			    }
				// create the varcase element for this varcase object
				Element varcaseElem = dom.createElement("varcase");
				if (varcase.getX() != null) {
					varcaseElem.appendChild(createVarElement(varcase.getX()));
				}
				if (varcase.getY() != null) {
					varcaseElem.appendChild(createVarElement(varcase.getY()));
				}
				if (varcase.getZ() != null) {
					varcaseElem.appendChild(createVarElement(varcase.getZ()));
				}
				// add the varcase element to the curve element
				curveElem.appendChild(varcaseElem);
			}
		} else {
			if (curve.getX() != null) {
				curveElem.appendChild(createVarElement(curve.getX()));
			}
			if (curve.getY() != null) {
				curveElem.appendChild(createVarElement(curve.getY()));
			}
			if (curve.getZ() != null) {
				curveElem.appendChild(createVarElement(curve.getZ()));
			}
		}
		
		if (!curveElem.hasChildNodes()) {
		    curveElem = null;
		}
		
		return curveElem;
	}
	
	/**
	 * Creates a table element based on the specified table data.
	 * 
	 * @param table 	An instance of {@link ProductTable}
	 * 
	 * @return An instance of {@link Element} to represent the table data.
	 */
	private Element createTableElement(ProductTable table) {
	    if (!table.isValid()) {
	        return null;
	    }
	    
		Element tableElem = dom.createElement("table");
		
		if (table.getTitle() != null && !("".equals(table.getTitle().trim()))) {
			// <title> ... </title>
			createChildElement(tableElem, "title", table.getTitle());
		}
		// for <tstart> ... </tsart>
		createChildElement(tableElem, "tstart", table.getTStart());
		// for <tstop> ... </tstop>
		createChildElement(tableElem, "tstop", table.getTStop());
		// for <frequency> ... </frequency>
		createChildElement(tableElem, "frequency", table.getFrequency());
		
		// for column	
		if (table.getColumnList() != null && table.getColumnList().size()>0) {
			
			for (ProductColumn column : table.getColumnList()) {
				if (column != null) {
					tableElem.appendChild(createColumnElement(column));
				}
			}
		}
		return tableElem;
	}
	
	/**
	 * Creates a column element based on the specified column data.
	 * 
	 * @param column 	An instance of {@link ProductColumn}
	 * 
	 * @return An instance of {@link Element} to represent the column data.
	 */
	private Element createColumnElement(ProductColumn column) {
		Element columnElem = dom.createElement("column");
		createChildElement(columnElem, "label", column.getLabel());
                
                // The units element is OPTIONAL and can be null,
                // indicating that the user doesn't want units conversion.
                String columnUnits = column.getUnits();
                if (columnUnits != null) {
		    createChildElement(columnElem, "units", columnUnits);
                }
		
		columnElem.appendChild(createVarElement(column.getVar()));
		
		// for "format" attribute
		if (column.getFormat() != null && !("".equals(column.getFormat().trim()))) {
			columnElem.setAttribute("format", column.getFormat());
		}	
		return columnElem;
	}
	
	/**
	 * Creates a var {@link Element} for the specified var data.
	 * 
	 * @param var An instance of {@link ProductVar}.
	 * 
	 * @return An instance of {@link Element}
	 */
	private Element createVarElement(ProductVar var) {
		Element varElem = null;
		// this should not happen since each column has to have a var
		if (var == null) {
			return varElem;
		}
		varElem = createChildElement(null, "var", var.getName());
		
		if (varElem != null) {
			//Now go through all <var>...</var> attributes
			if (var.getLabel() != null && !("".equals(var.getLabel().trim()))) {
				varElem.setAttribute("label", var.getLabel());
			}
			if (var.getFromUnits() != null && !("".equals(var.getFromUnits().trim()))) {
				varElem.setAttribute("from_units", var.getFromUnits());
			}
			if (var.getUnits() != null && !("".equals(var.getUnits().trim()))) {
				varElem.setAttribute("units", var.getUnits());
			}
			if (var.getBias() != null) {
				varElem.setAttribute("bias", var.getBias().toString());
			}
			if (var.getScale() != null) {
				varElem.setAttribute("scale", var.getScale().toString());
			}
			if (var.getMax() != null) {
				varElem.setAttribute("max", var.getMax().toString());
			}
			if (var.getSymbolStyleText() != null && !("".equals(var.getSymbolStyleText().trim()))) {
				varElem.setAttribute("symbol_style", var.getSymbolStyleText());
			}
			if (var.getSymbolSize() != null && !("".equals(var.getSymbolSize().trim()))) {
				varElem.setAttribute("symbol_size", var.getSymbolSize());
			}
			if (var.getLineStyleText() != null && !("".equals(var.getLineStyleText().trim()))) {
				varElem.setAttribute("line_style", var.getLineStyleText());
			}
			if (var.getLineColorCode() != null && !("".equals(var.getLineColorCode().trim()))) {
				varElem.setAttribute("line_color", var.getLineColorCode());
			}
			if (var.getGnuplotLineStyle() != null && !("".equals(var.getGnuplotLineStyle().trim()))) {
				varElem.setAttribute("gnuplot_line_style", var.getGnuplotLineStyle());
			}
		}
		
		return varElem;
	}
}
