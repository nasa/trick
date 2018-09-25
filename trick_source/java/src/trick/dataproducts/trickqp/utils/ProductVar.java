
//========================================
//  Package
//========================================
package trick.dataproducts.trickqp.utils;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Shape;
import java.awt.Stroke;
import java.awt.geom.Ellipse2D;
import java.awt.geom.GeneralPath;
import java.awt.geom.Rectangle2D;

import org.jfree.util.ShapeUtilities;

import trick.common.ui.UIUtils;

//========================================
//  Imports
//========================================

/**
 * Defines var data for Trick QP GUI.
 * The var data would be mapping to the
 * <code>var</code> in the XML file.
 *
 * @since Trick 10
 */
public class ProductVar {

    //========================================
    //  Public data
    //========================================
	public static enum LineStyle {	
		PLAIN("Plain") {
			@Override
			public Stroke getStroke() {
				return new BasicStroke(1.0f);
			}
		},
		DASH("Dash") {
			@Override
			public Stroke getStroke() {
				return new BasicStroke(1.0f,
		                BasicStroke.CAP_SQUARE, BasicStroke.JOIN_MITER, 10.0f,
		                new float[] {3.0f, 6.0f}, 0.0f);
			}			
		},
		NO_LINE("No_Line") {
			@Override
			public Stroke getStroke() {
				return new BasicStroke(0.0f);
			}
		},
		X_THICK_LINE("X_Thick_Line") {
			@Override
			public Stroke getStroke() {
				return new BasicStroke(5.0f);
			}
		},
		FINE_DASH("Fine_Dash") {
			@Override
			public Stroke getStroke() {
				return new BasicStroke(1.0f,
		                BasicStroke.CAP_SQUARE, BasicStroke.JOIN_MITER, 10.0f,
		                new float[] {2.0f, 2.0f}, 0.0f);
			}
		},
		MED_FINE_DASH("Med_Fine_Dash") {
			@Override
			public Stroke getStroke() {
				return new BasicStroke(1.0f,
		                BasicStroke.CAP_SQUARE, BasicStroke.JOIN_MITER, 10.0f,
		                new float[] {2.0f, 5.0f}, 0.0f);
			}
		},
		LONG_DASH("Long_Dash") {
			@Override
			public Stroke getStroke() {				
				return new BasicStroke(1.0f,
		                BasicStroke.CAP_SQUARE, BasicStroke.JOIN_MITER, 10.0f,
		                new float[] {5.0f, 5.0f}, 0.0f);
			}
		},
		X_LONG_DASH("X_Ling_Dash") {
			@Override
			public Stroke getStroke() {
				return new BasicStroke(1.0f,
		                BasicStroke.CAP_SQUARE, BasicStroke.JOIN_MITER, 10.0f,
		                new float[] {10.0f, 5.0f}, 0.0f);
			}
		},
		DOT_DASH("Dot_Dash") {
			@Override
			public Stroke getStroke() {
				return new BasicStroke(1.0f,
		                BasicStroke.CAP_SQUARE, BasicStroke.JOIN_MITER, 10.0f,
		                new float[] {10.0f, 5.0f, 1.0f, 5.0f}, 0.0f);
			}
		},
		TWO_DOT_DASH("2_Dot_Dash") {
			@Override
			public Stroke getStroke() {
				return new BasicStroke(1.0f,
		                BasicStroke.CAP_SQUARE, BasicStroke.JOIN_MITER, 10.0f,
		                new float[] {10.0f, 5.0f, 1.0f, 5.0f, 1.0f, 5.0f}, 0.0f);
			}
		},
		THREE_DOT_DASH("3_Dot_Dash") {
			@Override
			public Stroke getStroke() {
				return new BasicStroke(1.0f,
		                BasicStroke.CAP_SQUARE, BasicStroke.JOIN_MITER, 10.0f,
		                new float[] {10.0f, 5.0f, 1.0f, 5.0f, 1.0f, 5.0f, 1.0f, 5.0f}, 0.0f);
			}
		},
		FOUR_DOT_DASH("4_Dot_Dash") {
			@Override
			public Stroke getStroke() {
				return new BasicStroke(1.0f,
		                BasicStroke.CAP_SQUARE, BasicStroke.JOIN_MITER, 10.0f,
		                new float[] {10.0f, 5.0f, 1.0f, 5.0f, 1.0f, 5.0f, 1.0f, 5.0f, 1.0f, 5.0f}, 0.0f);
			}
		};
		
		private final String styleText;
		private LineStyle(String s) {
			styleText = s;
		}
		
		public abstract Stroke getStroke();
		@Override
		public String toString() {
			return styleText;
		}
	}
	
	public static enum SymbolSize {
		SYMBOL_TINY("Tiny") {
			@Override
			public double getDouble() {
				return 2.0;
			}
		},
		SYMBOL_SMALL("Small") {
			@Override
			public double getDouble() {
				return 4.0;
			}	
		},
		SYMBOL_MEDIEUM("Medieum") {
			@Override
			public double getDouble() {
				return 6.0;
			}	
		},
		SYMBOL_LARGE("Large") {
			@Override
			public double getDouble() {
				return 8.0;
			}
		};
		
		private final String sizeText;
		private SymbolSize(String s) {
			sizeText = s;
		}
		
		public abstract double getDouble();
		
		@Override
		public String toString() {
			return sizeText;
		}
	}
	
	public static enum SymbolStyle {
		SYMBOL_NONE("None"),
		SYMBOL_SQUARE("Square"),
		SYMBOL_CIRCLE("Circle"),
		SYMBOL_STAR("Star") {
			@Override
			public boolean isFilled() {				
				return true;
			}
		} ,
		SYMBOL_XX("XX") {
			@Override
			public boolean isFilled() {				
				return true;
			}
		},
		SYMBOL_TRANGLE("Triangle"),
		SYMBOL_SOLID_SQUARE("Solid_Square") {
			@Override
			public boolean isFilled() {				
				return true;
			}
		},
		SYMBOL_SOLID_CIRCLE("Solid_Circle") {
			@Override
			public boolean isFilled() {				
				return true;
			}
		},
		SYMBOL_THICK_SQUARE("Thick_Square"),
		SYMBOL_THICK_CIRCLE("Thick_Circle");
		
		private final String styleText;
		
		private SymbolStyle(String s) {
			styleText = s;
		}
		
		@Override
		public String toString() {
			return styleText;
		}
		
		/**
		 * Returns if the shape needs filling.
		 * 
		 * @return true if it needs filling, false otherwise.
		 */
		public boolean isFilled() {
			return false;
		}
	}
	
	public static enum GNUPlotLineStyle {
		LINES("lines"),
		POINTS("points"),
		LINE_POINTS("linepoints"),
		IMPULSES("impulses"),
		DOTS("dots"),
		STEPS("steps"),
		FSTEPS("fsteps"),
		HISTEPS("histeps"),
		BOXES("boxes");
		
		private final String styleText;
		
		private GNUPlotLineStyle(String s) {
			styleText = s;
		}
		
		@Override
		public String toString() {
			return styleText;
		}
	}
    public static String[] GUI_FIELD_LABELS_FULL    = new String[] {"Label", "From Units", "Units", "Bias",
                                                                    "Scale", "Max", "Symbol Style",
                                                                    "Symbol Size", "Line Style", "Line Color",
                                                                    "GNUPlot Line Sytle"};

    // all field labels for vars in a Table or X-var
    public static String[] GUI_FIELD_LABELS_TABLE   = new String[] {"Label", "From Units", "Units", "Bias", "Scale", "Max"};
    
    // all field labels for vars in a Program
    public static String[] GUI_FIELD_LABELS_PROGRAM = new String[] {"Units"};
    
    //========================================
    //  Protected data
    //========================================


    //========================================
    //  Private Data
    //========================================
    private static final int LABEL_INDEX                = 0;
    private static final int FROM_UNITS_INDEX           = 1;
    public static final int UNITS_INDEX                 = 2;
    private static final int BIAS_INDEX                 = 3;
    private static final int SCALE_INDEX                = 4;
    private static final int MAX_INDEX                  = 5;
    private static final int SYMBOL_STYLE_INDEX         = 6;
    private static final int SYMBOL_SIZE_INDEX          = 7;
    private static final int LINE_STYLE_INDEX           = 8;
    private static final int LINE_COLOR_INDEX           = 9;
    private static final int GNUPLOT_LINE_STYLE_INDEX   = 10;


    // The name of the variable
    private String name;
    // The label of the variable
    private String label;
    // The <units> element is OPTIONAL. It specifies the units
    // to which the user wants the recorded data to be converted.
    // The default value of units should be null, NOT "--".
    private String units;
    // The units from which needs to be converted.
    // Only needed when the data recording data doesn't provide the units for this variable.
    private String fromUnits;
    // The bias of the variable
    private Double bias;
    // The scale of the variable
    private Double scale;
    // The max value for the variable
    private Double max;
    // The symbol style text for the variable
    private String symbolStyleText;    
    // The symbol size for the variable
    private String symbolSizeText;
    // The line style string for the variable
    private String lineStyleText;
    // The line color string Hex code for the variable
    private String lineColorCode;
    // The line style for the gnu plot
    private String gnuplotLineStyle;
    
    //========================================
    //  Constructors
    //========================================
    /**
     * Constructor with variable name.
     *
     * @param name    The name of the variable.
     *
     */
    public ProductVar(String name) {
        this.name = name;
        // Leave the label blank initially in order for the default label shown.
        // If the label is entered by a user from GUI, that entered text
        // will be displayed and fxplot will not show the default label.
        //this.setLabel( name );
        setUnits(null);
        
    }

    /**
     * Constructor with specified variable and unit.
     *
     * @param name    The name of the variable.
     * @param units    The unit of the variable.
     */
    public ProductVar(String name, String units) {
        this.name = name;
        // Leave the label blank initially in order for the default label shown.
        // If the label is entered by a user from GUI, that entered text
        // will be displayed and fxplot will not show the default label.
        //this.setLabel( name );
        //this.setLabel( name );
        setUnits(units);
    }

    //========================================
    //  Set/Get methods
    //========================================
    /**
     * Gets the data field representation type {@link ProductDataPanel.DataRepresentationType}
     * for the specified label. A desired GUI component will be created based on the type.
     *
     * @param label    The label for the GUI data field.
     *
     * @return The GUI data field type in {@link ProductDataPanel.DataRepresentationType}.
     *
     */
    public static ProductDataPanel.DataRepresentationType getDataFieldType(String label) {
    	ProductDataPanel.DataRepresentationType type = ProductDataPanel.DataRepresentationType.PLAIN_TEXT_FIELD;
    	
    	if (label.equals(GUI_FIELD_LABELS_FULL[UNITS_INDEX])) {
            type = ProductDataPanel.DataRepresentationType.UNITS_FIELD;
        } else if (label.equals(GUI_FIELD_LABELS_FULL[FROM_UNITS_INDEX])) {
        	type = ProductDataPanel.DataRepresentationType.FROM_UNITS_FIELD;
        } else if (label.equals(GUI_FIELD_LABELS_FULL[SYMBOL_STYLE_INDEX])) {
        	type = ProductDataPanel.DataRepresentationType.COMBO_BOX;
        	type.setComboBoxItems(SymbolStyle.values());       	
        } else if (label.equals(GUI_FIELD_LABELS_FULL[SYMBOL_SIZE_INDEX])) {
        	type = ProductDataPanel.DataRepresentationType.COMBO_BOX;
        	type.setComboBoxItems(SymbolSize.values());           
        } else if (label.equals(GUI_FIELD_LABELS_FULL[LINE_STYLE_INDEX])) {
        	type = ProductDataPanel.DataRepresentationType.COMBO_BOX;
        	type.setComboBoxItems(LineStyle.values());       	
        } else if (label.equals(GUI_FIELD_LABELS_FULL[GNUPLOT_LINE_STYLE_INDEX])) {           
        	type = ProductDataPanel.DataRepresentationType.COMBO_BOX;
        	type.setComboBoxItems(GNUPlotLineStyle.values());
        } else if (label.equals(GUI_FIELD_LABELS_FULL[LINE_COLOR_INDEX])) {
        	type = ProductDataPanel.DataRepresentationType.COLOR_LABEL;       	           
        } else if (label.equals(GUI_FIELD_LABELS_FULL[BIAS_INDEX]) ||
                   label.equals(GUI_FIELD_LABELS_FULL[SCALE_INDEX])||
                   label.equals(GUI_FIELD_LABELS_FULL[MAX_INDEX])) {
        	type = ProductDataPanel.DataRepresentationType.NUMBER_FIELD;           
        }
    	
    	return type;
    }
    
    /**
     * Sets data attribute for the specified order number for GUI display.
     *
     * @param index        The order number of the data fields as shown in GUI.
     * @param value        The value of the GUI component represents.
     *
     */
    public void setData(int index, Object value) {
        // can't 100% trust str == "", so using "".equals(str) instead !!!
        String valueStr = null;
        if (value != null && !("".equals(value.toString().trim()))) {
            valueStr = value.toString();
        }

        switch (index) {
            case LABEL_INDEX:
                setLabel(valueStr);
                break;
            case FROM_UNITS_INDEX:
            	setFromUnits(valueStr);
            	break;
            case UNITS_INDEX:
                setUnits(valueStr);
                break;
            case BIAS_INDEX:
                setBias(valueStr);
                break;
            case SCALE_INDEX:
                setScale(valueStr);
                break;
            case MAX_INDEX:
                setMax(valueStr);
                break;
            case SYMBOL_STYLE_INDEX:
                setSymbolStyle(valueStr);
                break;
            case SYMBOL_SIZE_INDEX:
                setSymbolSize(valueStr);
                break;
            case LINE_STYLE_INDEX:
                setLineStyleText(valueStr);
                break;
            case LINE_COLOR_INDEX:
                setLineColorCode(valueStr);
                break;
            case GNUPLOT_LINE_STYLE_INDEX:
                setGnuplotLineStyle(valueStr);
                break;
        }
    }

    /**
     * Gets data for GUI display for the specified order number.
     * @param index index into list
     * @return object pointed to by index
     */
    public Object getData(int index) {
        Object ret = null;
        switch (index) {
            case LABEL_INDEX:
                ret = getLabel();
                break;
            case FROM_UNITS_INDEX:
            	ret = getFromUnits();
            	break;
            case UNITS_INDEX:
                ret = getUnits();
                break;
            case BIAS_INDEX:
                ret = getBias();
                break;
            case SCALE_INDEX:
                ret = getScale();
                break;
            case MAX_INDEX:
                ret = getMax();
                break;
            case SYMBOL_STYLE_INDEX:
                ret = getSymbolStyleText();
                break;
            case SYMBOL_SIZE_INDEX:
                ret = getSymbolSize();
                break;
            case LINE_STYLE_INDEX:
                ret = getLineStyleText();
                break;
            case LINE_COLOR_INDEX:
                ret = getLineColorCode();
                break;
            case GNUPLOT_LINE_STYLE_INDEX:
                ret = getGnuplotLineStyle();
                break;
        }

        return ret;
    }

    /**
     * Sets the var label.
     *
     * @param label        The var label.
     */
    public void setLabel(String label) {
        this.label = label;
    }

    /**
     * Sets the var units.
     *
     * @param units        The var units.
     */
    public void setUnits(String units) {
        this.units = units;
    }
    
    /**
     * Sets the var from units. Only used when recorded data doesn't have units specified.
     *
     * @param fromUnits        The var from units.
     */
    public void setFromUnits(String fromUnits) {
        this.fromUnits = fromUnits;
    }

    /**
     * Sets the var bias.
     *
     * @param bias        The var bias.
     */
    public void setBias(double bias) {
        this.bias = bias;
    }

    /**
     * Sets the var bias.
     *
     * @param biasStr        The var bias in text.
     */
    public void setBias(String biasStr) {
        try {
            bias = Double.valueOf(biasStr);
        } catch (NumberFormatException e) {
            bias = null;
        } catch (NullPointerException e) {
            bias = null;
        }
    }

    /**
     * Sets the var scale.
     *
     * @param scale        The var scale.
     */
    public void setScale(double scale) {
        this.scale = scale;
    }

    /**
     * Sets the var scale.
     *
     * @param scaleStr        The var scale in text.
     */
    public void setScale(String scaleStr) {
        try {
            scale = Double.valueOf(scaleStr);
        } catch (NumberFormatException e) {
            scale = null;
        } catch (NullPointerException e) {
            scale = null;
        }
    }

    /**
     * Sets the var max.
     *
     * @param max        The var max.
     */
    public void setMax(double max) {
        this.max = max;
    }

    /**
     * Sets the var max.
     *
     * @param maxStr        The var max in text.
     */
    public void setMax(String maxStr) {
        try {
            max = Double.valueOf(maxStr);
        } catch (NumberFormatException e) {
            max = null;
        } catch (NullPointerException e) {
            max = null;
        }
    }

    /**
     * Sets the var symbol style.
     *
     * @param symbolStyle    The var symbol style.
     */
    public void setSymbolStyle(String symbolStyle) {
        this.symbolStyleText = symbolStyle;
    }

    /**
     * Sets the var symbol size.
     *
     * @param symbolSize    The var symbol size.
     */
    public void setSymbolSize(String symbolSize) {
        this.symbolSizeText = symbolSize;
    }

    /**
     * Sets the var line style.
     *
     * @param style            The var line style.
     */
    public void setLineStyleText(String style) {
        lineStyleText = style;
    }

    /**
     * Sets the var line color Hex code.
     *
     * @param color            The var line color Hex code.
     */
    public void setLineColorCode(String color) {
        lineColorCode = color;
    }

    /**
     * Sets the var gnuplot line style.
     *
     * @param style    The var gnuplot line style.
     */
    public void setGnuplotLineStyle(String style) {
        gnuplotLineStyle = style;
    }

    /**
     * Gets the name.
     * @return name
     */
    public String getName() {
        return name;
    }

    /**
     * Gets the short name that is the text after the last dot.
     * @return name
     */
    public String getShortName() {
    	String shortName = name;
    	if (name != null) {
    		int lastDot = name.lastIndexOf(".");
    		if (lastDot != -1) {
    			shortName = name.substring(lastDot+1);
    		}
    	}
        return shortName;
    }
    
    /**
     * Gets the var label.
     *
     * @return The var label.
     */
    public String getLabel() {
        return label;
    }

    /**
     * Gets the var units.
     *
     * @return The var units.
     */
    public String getUnits() {
        return units;
    }
    
    /**
     * Gets the var from units. Only used when recorded data doesn't have units specified.
     *
     * @return The var from units.
     */
    public String getFromUnits() {
        return fromUnits;
    }

    /**
     * Gets the var bias.
     *
     * @return The var bias.
     */
    public Double getBias() {
        return bias;
    }

    /**
     * Gets the var scale.
     *
     * @return The var scale.
     */
    public Double getScale() {
        return scale;
    }
    /**
     * Gets the var max.
     *
     * @return The var max.
     */
    public Double getMax() {
        return max;
    }

    /**
     * Gets the var symbol style.
     *
     * @return The var symbol style.
     */
    public String getSymbolStyleText() {
        return symbolStyleText;
    }

    /**
     * Gets the var symbol size.
     *
     * @return The var symbol size.
     */
    public String getSymbolSize() {
        return symbolSizeText;
    }
    
    /**
     * Gets the var symbol shape based on the symbol style and size.
     * If symbol style is not specified, it size is ignored.
     *
     * @param symbolStyle sybol style to use
     * @return The var symbol shape instance {@link Shape}.
     */
    public Shape getSymbolShape(SymbolStyle symbolStyle) {
    	Shape symbolShape = null;
    	if (symbolStyle == null) {
    		return symbolShape;
    	}
    	
    	SymbolSize symbolSize = null;
    	if (symbolSizeText != null && !symbolSizeText.isEmpty()) {
    		for (SymbolSize theSize : SymbolSize.values()) {
    			if (symbolSizeText.equals(theSize.toString())) {
    				symbolSize = theSize;
    			}
    		}
    	}
    	
    	double size = SymbolSize.SYMBOL_MEDIEUM.getDouble();
    	if (symbolSize != null) {
    		size = symbolSize.getDouble();
    	}
    	double delta = size/ 2;
    	Stroke stroke = null;
    	if (symbolStyle != null) {
    		switch (symbolStyle) {
    			case SYMBOL_NONE:
    				break;
    			case SYMBOL_SQUARE:
    			case SYMBOL_SOLID_SQUARE:    			
    				symbolShape = new Rectangle2D.Double(-delta, -delta, size, size);
    				break;
    			case SYMBOL_CIRCLE:
    			case SYMBOL_SOLID_CIRCLE:    			
    				symbolShape = new Ellipse2D.Double(-delta, -delta, size, size);
    				break;
    			case SYMBOL_STAR:
    				symbolShape = createStar((float)delta, 0.1f);
    				break;
    			case SYMBOL_XX:
    				symbolShape = ShapeUtilities.createDiagonalCross((float)delta, 0.1f);
    				break;
    			case SYMBOL_TRANGLE:    				
    				symbolShape = ShapeUtilities.createUpTriangle((float)delta);
    				break;
    			case SYMBOL_THICK_SQUARE:
    				stroke = new BasicStroke(1.0f);
    				symbolShape = stroke.createStrokedShape(new Rectangle2D.Double(-delta, -delta, size, size));
    				break;
    			case SYMBOL_THICK_CIRCLE:
    				stroke = new BasicStroke(1.0f);
    				symbolShape = stroke.createStrokedShape(new Ellipse2D.Double(-delta, -delta, size, size));
    				break;
    		}
    	}
    	
    	return symbolShape;
    }

    /**
     * Gets the var symbol style as a {@link SymbolStyle}.
     *
     * @return The var symbol style.
     */
	public SymbolStyle getSymbolStyle() {
		SymbolStyle symbolStyle = null;
		if (symbolStyleText != null && !symbolStyleText.isEmpty()) {
    		for (SymbolStyle theStyle : SymbolStyle.values()) {
    			if (symbolStyleText.equals(theStyle.toString())) {
    				symbolStyle = theStyle;
    			}
    		}
    	}
		return symbolStyle;
	}

    /**
     * Gets the var line style text.
     *
     * @return The var line style string.
     */
    public String getLineStyleText() {
        return lineStyleText;
    }
    
    /**
     * Gets the var line style.
     *
     * @return The var line style as a {@link LineStyle}.
     */
    public LineStyle getLineStyle() {
    	LineStyle lineStyle = null;    
    	if (lineStyleText != null && !lineStyleText.isEmpty()) {    		
    		for (LineStyle theStyle : LineStyle.values()) {	        			
    			if (lineStyleText.equals(theStyle.toString())) {        				
    				lineStyle = theStyle;
    				break;
    			}
    		}
    	}
    	return lineStyle;
    }

    /**
     * Gets the var line color text.
     *
     * @return The var line color Hex code.
     */
    public String getLineColorCode() {
    	// no need to return the "Default" color text
    	if (lineColorCode != null && lineColorCode.equals(CommonProduct.DEFAULT_COLOR_TEXT)) {
    		return null;
    	}   	
        return lineColorCode;
    }
    
    /**
     * Gets the var line color.
     *
     * @return The var line {@link Color} instance.
     */
    public Color getLineColor() {
    	return UIUtils.getColorFromHTMLHex(lineColorCode);
    }

    /**
     * Gets the var gnuplot line style.
     *
     * @return The var gnuplot line style.
     */
    public String getGnuplotLineStyle() {
        return gnuplotLineStyle;
    }

    //========================================
    //  Methods
    //========================================
    /**
     * Returns the name of this var. This is for GUI displaying.
     */
    @Override
	public String toString() {
        return name;
    }
    
    /**
     * Creates a star that actually combines diagonal & regular cross shapes.
     *
     * @param l  the length of each 'arm'.
     * @param t  the thickness.
     *
     * @return A star (*) shape.
     */
    private Shape createStar(final float l, final float t) {
    	final float SQRT2 = (float) Math.pow(2.0, 0.5);
        final GeneralPath p0 = new GeneralPath();
        p0.moveTo(-l - t, -l + t);
        p0.lineTo(-l + t, -l - t);
        p0.lineTo(0.0f, -t * SQRT2);
        p0.lineTo(l - t, -l - t);
        p0.lineTo(l + t, -l + t);
        p0.lineTo(t * SQRT2, 0.0f);
        p0.lineTo(l + t, l - t);
        p0.lineTo(l - t, l + t);
        p0.lineTo(0.0f, t * SQRT2);
        p0.lineTo(-l + t, l + t);
        p0.lineTo(-l - t, l - t);
        p0.lineTo(-t * SQRT2, 0.0f);
        
        p0.moveTo(-l, t);
        p0.lineTo(-t, t);
        p0.lineTo(-t, l);
        p0.lineTo(t, l);
        p0.lineTo(t, t);
        p0.lineTo(l, t);
        p0.lineTo(l, -t);
        p0.lineTo(t, -t);
        p0.lineTo(t, -l);
        p0.lineTo(-t, -l);
        p0.lineTo(-t, -t);
        p0.lineTo(-l, -t);
        p0.closePath();
        return p0;
    }   
}
