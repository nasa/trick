
package trick.dataproducts.plot.utils.editor;

import java.awt.event.ActionEvent;
import java.awt.event.FocusEvent;
import java.awt.event.FocusListener;

import javax.swing.BorderFactory;
import javax.swing.JCheckBox;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;

import org.jfree.chart.axis.Axis;
import org.jfree.chart.axis.NumberAxis;
import org.jfree.chart.axis.NumberTickUnit;
import org.jfree.layout.LCBLayout;

/**
 * A panel for editing the properties of a value axis.
 */
public class TrickNumberAxisEditor extends TrickValueAxisEditor
    implements FocusListener {

    
	private static final long serialVersionUID = 651540823473853902L;

	private double manualTickUnitValue;

    private JTextField manualTickUnit;


    /**
     * Standard constructor: builds a property panel for the specified axis.
     *
     * @param axis  the axis, which should be changed.
     */
    public TrickNumberAxisEditor(NumberAxis axis) {

        super(axis);

        this.manualTickUnitValue = axis.getTickUnit().getSize();
        validateTickUnit();
    }

    @Override
	protected JPanel createTickUnitPanel()
    {
        JPanel tickUnitPanel = new JPanel(new LCBLayout(3));
        tickUnitPanel.setBorder(BorderFactory.createEmptyBorder(4, 4, 4, 4));

        tickUnitPanel.add(new JPanel());
        JCheckBox autoTickUnitSelectionCheckBox = new JCheckBox(
                localizationResources.getString("Auto-TickUnit_Selection"),
                isAutoTickUnitSelection());
        autoTickUnitSelectionCheckBox.setActionCommand("AutoTickOnOff");
        autoTickUnitSelectionCheckBox.addActionListener(this);
        setAutoTickUnitSelectionCheckBox(autoTickUnitSelectionCheckBox);
        tickUnitPanel.add(getAutoTickUnitSelectionCheckBox());
        tickUnitPanel.add(new JPanel());

        tickUnitPanel.add(new JLabel(localizationResources.getString(
                "Manual_TickUnit_value")));
        this.manualTickUnit = new JTextField(Double.toString(
                this.manualTickUnitValue));
        this.manualTickUnit.setEnabled(!isAutoTickUnitSelection());
        this.manualTickUnit.setActionCommand("TickUnitValue");
        this.manualTickUnit.addActionListener(this);
        this.manualTickUnit.addFocusListener(this);
        tickUnitPanel.add(this.manualTickUnit);
        tickUnitPanel.add(new JPanel());

        return tickUnitPanel;
    }

    /**
     * Handles actions from within the property panel.
     * @param event an event.
     */
    @Override
	public void actionPerformed(ActionEvent event) {
        String command = event.getActionCommand();
        if (command.equals("TickUnitValue")) {
            validateTickUnit();
        }
        else {
            // pass to the super-class for handling
            super.actionPerformed(event);
        }
    }

    @Override
	public void focusLost(FocusEvent event) {
        super.focusLost(event);
        if (event.getSource() == this.manualTickUnit) {
            validateTickUnit();
        }
    }

    @Override
	public void toggleAutoTick() {
        super.toggleAutoTick();
        if (isAutoTickUnitSelection()) {
            this.manualTickUnit.setText(Double.toString(this.manualTickUnitValue));
            this.manualTickUnit.setEnabled(false);
        }
        else {
            this.manualTickUnit.setEnabled(true);
        }
    }

    public void validateTickUnit() {
        double newTickUnit;
        try {
            newTickUnit = Double.parseDouble(this.manualTickUnit.getText());
        }
        catch (NumberFormatException e) {
            newTickUnit = this.manualTickUnitValue;
        }

        if (newTickUnit > 0.0) {
            this.manualTickUnitValue = newTickUnit;
        }
        this.manualTickUnit.setText(Double.toString(this.manualTickUnitValue));
    }

    /**
     * Sets the properties of the specified axis to match the properties
     * defined on this panel.
     *
     * @param axis  the axis.
     */
    @Override
	public void setAxisProperties(Axis axis) {
        super.setAxisProperties(axis);
        NumberAxis numberAxis = (NumberAxis) axis;
        if (!isAutoTickUnitSelection()) {
            numberAxis.setTickUnit(new NumberTickUnit(manualTickUnitValue));
        }
    }
}
