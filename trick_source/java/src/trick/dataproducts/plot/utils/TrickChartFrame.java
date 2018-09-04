package trick.dataproducts.plot.utils;

import java.awt.Dimension;
import java.awt.Graphics2D;
import java.awt.GridLayout;
import java.awt.HeadlessException;
import java.awt.Image;
import java.awt.event.ActionEvent;
import java.awt.event.KeyEvent;
import java.awt.geom.Rectangle2D;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.ArrayList;

import javax.swing.AbstractAction;
import javax.swing.ImageIcon;
import javax.swing.JMenuItem;
import javax.swing.JPanel;
import javax.swing.JSeparator;

import org.jfree.chart.ChartPanel;

import org.apache.pdfbox.pdmodel.PDDocument;
import org.apache.pdfbox.pdmodel.PDPage;
import org.apache.pdfbox.pdmodel.PDPageContentStream;
import org.apache.pdfbox.pdmodel.common.PDRectangle;
import org.apache.pdfbox.pdmodel.font.PDFont;
import org.apache.pdfbox.pdmodel.font.PDType1Font;
import org.apache.pdfbox.pdmodel.graphics.image.JPEGFactory;
import org.apache.pdfbox.pdmodel.graphics.image.PDImageXObject;

import org.jfree.chart.JFreeChart;

import trick.common.TrickApplication;
import trick.common.ui.UIUtils;
import trick.dataproducts.trickqp.utils.ProductPage;


public class TrickChartFrame extends TrickFrame {

    //========================================
    //    Public data
    //========================================
    
    

    //========================================
    //    Protected data
    //========================================
    
    
    
    //========================================
    //    Private Data
    //========================================	
	private static final long serialVersionUID = 1388143913508263815L;
	
	private ArrayList<TrickChartControlPanel> allPanels;
	
    private String pageTitle = "Page";
	

	//========================================
    //   Constructors
    //========================================	
	/**
     * Creates a new, initially invisible <code>Frame</code> with the
     * specified title and frame icon if necessary.
     * <p>
     * This constructor sets the component's locale property to the value
     * returned by <code>JComponent.getDefaultLocale</code>.
     *
     * @param title the title for the frame
     * @param iconImage the icon image for the frame and has to set it to null for Mac 
     * @param thePage the {@link ProductPage} instance that contains page spec
     * @param chartList the list of charts that are in the specified the page 
     */
    public TrickChartFrame(String title, Image iconImage, ProductPage thePage, ArrayList<TrickChart> chartList) throws HeadlessException {
        super(title, iconImage);
        setMenuBar();
        if (fileMenu != null) {       
        	fileMenu.add(new SaveAction("Make PDF", new ImageIcon(TrickApplication.class.getResource("resources/filesave.gif")), "Save Page to PDF", KeyEvent.VK_S));
        
        	fileMenu.add(new JSeparator());
        	JMenuItem closeMenuItem = new JMenuItem(new CloseAction("Close", "Close Plot", KeyEvent.VK_L));
        
        	fileMenu.add(closeMenuItem);
        }
        
        if (thePage != null && thePage.getTitle() != null) {
        	pageTitle = thePage.getTitle();
        }
        
        setupChart(thePage, chartList);
    }
    

    //========================================
    //   Methods
    //========================================
    /**
     * Helper method for setting up the plot.
     */
    private void setupChart(ProductPage thePage, ArrayList<TrickChart> chartList) {
    	
		if (chartList == null || chartList.size() < 0) {
    		return;
    	}
    	int chartSize = chartList.size();
    	allPanels = new ArrayList<TrickChartControlPanel>(chartSize);
    	JPanel framePanel = getPlotChartFramePanel(thePage, chartSize);
         
        for (TrickChart eachChart : chartList) {
            TrickChartControlPanel eachPanel = new TrickChartControlPanel(eachChart, true);
            allPanels.add(eachPanel);
            framePanel.add(eachPanel);
        }
        setContentPane(framePanel);
	}
    
    /**
     * Configures a panel so that it is able to show specified number of charts. 
     */
    private JPanel getPlotChartFramePanel(ProductPage thePage, int subChartNumber) {
        JPanel framePanel = new JPanel();
        framePanel.setPreferredSize(new Dimension(ChartPanel.DEFAULT_WIDTH, ChartPanel.DEFAULT_HEIGHT+280));
        int numberVCells = 1;
        int numberHCells = 1;
        
        if (thePage.getHcells() != null) {
            numberHCells = thePage.getHcells().intValue();
        } 
        
        if (thePage.getVcells() != null) {
            numberVCells = thePage.getVcells().intValue();
        }
        
        if (subChartNumber > (numberHCells * numberVCells)) {
            if (subChartNumber < 9) {
                if (subChartNumber == 3) {
                    numberHCells = 1;
                    numberVCells = 3;
                } else if (subChartNumber == 1) {
                    numberVCells = 1;
                    numberHCells = 1;
                } else {
                    numberVCells = 2;
                    numberHCells = (subChartNumber / 2) + (subChartNumber % 2);
                }
            } else {
				while((numberHCells * numberVCells) < subChartNumber ) {
				    if (numberHCells <= numberVCells) {
				        numberHCells++;
				    } else {
				        numberVCells++;
				    }
				}
			}
        }
        
        framePanel.setLayout(new GridLayout(numberVCells, numberHCells));
        return framePanel;
    }
    
    public String getChartTitle() {
    	return pageTitle;
    }
    
    public int getChartSize() {
    	if (allPanels != null) {
    		return allPanels.size();
    	}
    	return 0;
    }
    
    /**
     * Helper method for saving all charts to a PDF file.
     * @throws IOException IOException
     */
    public void saveChartsAsPDF() throws IOException {
    	File file = UIUtils.chooseSaveFile(null, "plot_", "pdf", this);
        if (file != null) {
            PDDocument document = new PDDocument(); // Default Paper size is US Letter.
            writePDFPage(document);
            document.save(file);
	}
    }
    
    
    /**
     * Writes all plots shown to one page of PDF.
     * 
     * @param  doc         An Apache-PDFBox PDDocument object.
     * @throws IOException IOException
     */
    public void writePDFPage(PDDocument doc) throws IOException {
   
        PDPage pdfPage = new PDPage(); // Default page size is 'US Letter' (8.5x11 inches).
            
        // This is the size of the page in PDF 'points'. One point = 1/72 inch.
        PDRectangle mediaBox = pdfPage.getMediaBox();

        float inches = 72F; // Conversion from inches to points.
            
        float topMargin    = 0.5F  * inches;
        float bottomMargin = 0.75F * inches;
        float leftMargin   = 0.75F * inches;
        float rightMargin  = 0.5F  * inches;
            
        doc.addPage(pdfPage);
            
        try (PDPageContentStream content = new PDPageContentStream(doc, pdfPage)) {
                
            int fontSize = 12;
            PDFont font = PDType1Font.TIMES_ROMAN;
                 
            String chartTitle = getChartTitle();
            float chartTitleWidth  = font.getStringWidth(chartTitle) / 1000F * fontSize;
            float chartTitleHeight = font.getFontDescriptor().getFontBoundingBox().getHeight() / 1000F * fontSize;
            float chartTitleLowerLeftX = (mediaBox.getWidth() + leftMargin - rightMargin - chartTitleWidth) / 2F ;
            float chartTitleLowerLeftY = mediaBox.getHeight() - topMargin - chartTitleHeight;
                
            content.beginText();
            content.setFont(font, fontSize);
            content.newLineAtOffset(chartTitleLowerLeftX, chartTitleLowerLeftY);
            content.showText(chartTitle);
            content.endText();
                
            // Define the box into which the chart(s) will go.
            // The top of the box is 1/2 inch below the top margin.
            // The rest is bounded by the left, right and bottom margins.
            float chartBoxLowerLeftX = leftMargin;
            float chartBoxLowerLeftY = bottomMargin;
            float chartBoxUpperRightX = mediaBox.getUpperRightX() - rightMargin;
            float chartBoxUpperRightY = mediaBox.getUpperRightY() - topMargin - 0.5F * inches;
            float chartBoxWidth  = chartBoxUpperRightX - chartBoxLowerLeftX;
            float chartBoxHeight = chartBoxUpperRightY - chartBoxLowerLeftY;
                
            int rows = 1; int cols = 1; // If we're plotting one plot on this page, otherwise ...
            if ( getContentPane().getLayout() instanceof GridLayout) {
                rows = ((GridLayout)getContentPane().getLayout()).getRows();
                cols = ((GridLayout)getContentPane().getLayout()).getColumns();
            }

            // Calculate the size of the cells, into which each plot will be dawn.
            int cellWidth  = (int)chartBoxWidth / cols;
            int cellHeight = (int)chartBoxHeight/ rows; 
                
            for (int i = 0 ; i < rows ; ++i) {
                int cellLowerLeftY = (int)chartBoxUpperRightY - (i+1) * cellHeight;
                
                for (int j = 0 ; j < cols ; ++j) {
                    int cellLowerLeftX = (int)chartBoxLowerLeftX + j * cellWidth;
                    
                    int whichPanel = i * cols + j; 
                    TrickChartControlPanel panel = (TrickChartControlPanel)getContentPane().getComponent(whichPanel);
                    JFreeChart thechart = panel.getTheChart();
                    BufferedImage bufferedImage = thechart.createBufferedImage((int)chartBoxWidth, (int)chartBoxHeight);
                    PDImageXObject image = JPEGFactory.createFromImage(doc, bufferedImage);
                    content.drawImage(image, cellLowerLeftX, cellLowerLeftY, cellWidth, cellHeight);
                }
            }       
        }   
    }
	   
    /**
     * Sets this frame invisible.
     */
    private void closeChart() {
    	setVisible(false);
    }
    
    //========================================
    //   Inner Classes
    //========================================
	public class SaveAction extends AbstractAction {
      					
		private static final long serialVersionUID = 5844945315420458601L;
		public SaveAction(String text, ImageIcon icon,
                          String desc, Integer mnemonic) {
            super(text, icon);
            putValue(SHORT_DESCRIPTION, desc);
            putValue(MNEMONIC_KEY, mnemonic);
        }
        public void actionPerformed(ActionEvent e) {
            try {
				saveChartsAsPDF();
			} catch (IOException e1) {				
				e1.printStackTrace();
			}
        }
    }
	
	public class CloseAction extends AbstractAction {
				
		private static final long serialVersionUID = 3077821392818469620L;
		public CloseAction(String text, String desc, Integer mnemonic) {
            super(text);
            putValue(SHORT_DESCRIPTION, desc);
            putValue(MNEMONIC_KEY, mnemonic);
        }
        public void actionPerformed(ActionEvent e) {
            closeChart();
        }
    }
	
}
