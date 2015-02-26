package trick.tv;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.BoxLayout;
import javax.swing.JComboBox;

import org.jdesktop.swingx.JXLabel;
import org.jdesktop.swingx.JXPanel;

/**
 * two combo boxes separated by a dash on a panel
 *
 * @author Derek Bankieris
 */
// Suppress warning about instatiating a generic type in Java 7.
@SuppressWarnings("unchecked")
public class DoubleComboBox extends JXPanel {

    /** the first combo box */
    JComboBox minimumBox = new JComboBox();

    /** the second combo box */
    JComboBox maximumBox = new JComboBox();

    /**
     * constructor
     *
     * @param count the maximum selectable value
     */
    public DoubleComboBox(int count) {
        setLayout(new BoxLayout(this, BoxLayout.X_AXIS));

        for (int i = 0; i < count; i++) {
            minimumBox.addItem(new Integer(i));
            maximumBox.addItem(new Integer(i));
        }

        minimumBox.setSelectedIndex(0);
        maximumBox.setSelectedIndex(count - 1);

        minimumBox.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent event) {
                int min = ((Integer)minimumBox.getSelectedItem());
                for (int i = ((Integer)maximumBox.getItemAt(0)); i < min; ++i) {
                    maximumBox.removeItemAt(0);
                }
                for (int i = ((Integer)maximumBox.getItemAt(0)); i > min; ) {
                    maximumBox.insertItemAt(new Integer(--i), 0);
                }
            }
        });

        maximumBox.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent event) {
                int max = ((Integer)maximumBox.getSelectedItem());
                for (int i = ((Integer)minimumBox.getItemAt(minimumBox.getItemCount() - 1)); i > max; --i) {
                    minimumBox.removeItemAt(minimumBox.getItemCount() - 1);
                }
                for (int i = ((Integer)minimumBox.getItemAt(minimumBox.getItemCount() - 1)); i < max; ) {
                    minimumBox.addItem(new Integer(++i));
                }
            }
        });

        add(minimumBox);
        add(new JXLabel() {{
            setText("-");
        }});
        add(maximumBox);
    }

    @Override
    public void setEnabled(boolean enabled) {
        super.setEnabled(enabled);
        minimumBox.setEnabled(enabled);
        maximumBox.setEnabled(enabled);
    }

    @Override
    public String toString() {
        return minimumBox.getSelectedItem() + "-" + maximumBox.getSelectedItem();
    }

}
