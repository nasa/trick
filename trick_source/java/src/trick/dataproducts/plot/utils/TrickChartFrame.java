package trick.dataproducts.plot.utils;

import java.awt.Dimension;
import java.awt.Graphics2D;
import java.awt.GridLayout;
import java.awt.HeadlessException;
import java.awt.Image;
import java.awt.event.ActionEvent;
import java.awt.event.KeyEvent;
import java.awt.geom.Rectangle2D;
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

import com.lowagie.text.Document;
import com.lowagie.text.DocumentException;
import com.lowagie.text.PageSize;
import com.lowagie.text.pdf.BaseFont;
import com.lowagie.text.pdf.DefaultFontMapper;
import com.lowagie.text.pdf.FontMapper;
import com.lowagie.text.pdf.PdfContentByte;
import com.lowagie.text.pdf.PdfTemplate;
import com.lowagie.text.pdf.PdfWriter;

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
        if (file == null) {
        	return;
        }
        Document document = new Document(PageSize.A4);
        PdfWriter writer = null;
        PdfContentByte pdfContent = null;
		try {
			writer = PdfWriter.getInstance(document, new FileOutputStream(file));
			document.open();
			pdfContent = writer.getDirectContent();
		} catch (DocumentException e) {								
			e.printStackTrace();
		}
        writePDFPage(pdfContent, new DefaultFontMapper());
        
        if (document != null) {
        	document.close();
        }   
	}
    
    
    /**
     * Writes all plots shown to one page of PDF.
     * 
     * @param pdfContent 	an object containing the user positioned text and graphic contents of a page.
     * @param mapper		mapping between AWT fonts and PDF fonts.
     * @throws IOException IOException
     */
    public void writePDFPage(PdfContentByte pdfContent, FontMapper mapper) throws IOException {
    	if (pdfContent == null) {
    		return;
    	}
    	Document document = pdfContent.getPdfDocument();
    	int chartLocOffset = 30;
        try {       	
            int pageWidth = (int)document.getPageSize().getWidth();
            int pageHeight = (int)document.getPageSize().getHeight();
            document.newPage();
            
            BaseFont pageTitleFont = BaseFont.createFont(BaseFont.HELVETICA_BOLD, "Cp1252", false);
            
            // show page title text
            pdfContent.beginText();
            pdfContent.setFontAndSize(pageTitleFont, 12);            
            pdfContent.showTextAligned(PdfContentByte.ALIGN_CENTER, getChartTitle(), pageWidth / 2 , pageHeight - chartLocOffset / 2, 0);
            pdfContent.endText();
            
            // draw all plots
            PdfTemplate pdfTemplate = pdfContent.createTemplate(pageWidth, pageHeight);
            Graphics2D g2 = pdfTemplate.createGraphics(pageWidth, pageHeight, mapper);
            int rows = 1;
			int columns = 1;
			
			if (getContentPane().getLayout() instanceof GridLayout) {
				rows = ((GridLayout)getContentPane().getLayout()).getRows();
				columns = ((GridLayout)getContentPane().getLayout()).getColumns();
			}
			
			int eachChartWidth = pageWidth;
			int eachChartHeight = pageHeight - chartLocOffset;
			if (columns != 0) {
				eachChartWidth = pageWidth / columns;
			}
			if (rows != 0) {
				eachChartHeight = (pageHeight - chartLocOffset) / rows;
			}
			
			int xLoc = 0;
			int yLoc = chartLocOffset;
			for (int i = 0; i < rows; i++) {
				for (int j = 0; j < columns; j++) {
					int index = i * columns + j;
					if (index >= getChartSize()) {
						break;
					}
					TrickChartControlPanel eachPanel = (TrickChartControlPanel)getContentPane().getComponent(index);
					if (eachPanel.getTheChart() != null) {
						Rectangle2D r2D = new Rectangle2D.Double(xLoc, yLoc, eachChartWidth, eachChartHeight);
						eachPanel.getTheChart().draw(g2, r2D);
					}
					xLoc = xLoc + eachChartWidth;
				}
				xLoc = 0;
				yLoc = yLoc + eachChartHeight;
				
			}			
            g2.dispose();
            pdfContent.addTemplate(pdfTemplate, 0, 0);    
        }
        catch (DocumentException de) {
            de.printStackTrace();
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
