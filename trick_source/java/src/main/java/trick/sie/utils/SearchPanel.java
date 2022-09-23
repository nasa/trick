package trick.sie.utils;

import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.event.ActionEvent;
import java.awt.event.InputEvent;
import java.awt.event.KeyEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

import javax.swing.AbstractAction;
import javax.swing.AbstractListModel;
import javax.swing.Action;
import javax.swing.BoxLayout;
import javax.swing.DefaultListCellRenderer;
import javax.swing.JCheckBox;
import javax.swing.JComponent;
import javax.swing.JList;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPopupMenu;
import javax.swing.JProgressBar;
import javax.swing.JScrollPane;
import javax.swing.KeyStroke;

import org.jdesktop.swingx.JXButton;
import org.jdesktop.swingx.JXLabel;
import org.jdesktop.swingx.JXList;
import org.jdesktop.swingx.JXPanel;
import org.jdesktop.swingx.JXTextField;
import org.jdesktop.swingx.decorator.HighlighterFactory;

import trick.sie.utils.Searcher;
import trick.sie.utils.SearchListener;
import trick.sie.utils.SieTemplate;

/**
 * a panel for searching a collection of <code>SieTemplate</code>s for a target
 * string
 *
 * @author Derek Bankieris
 */
public class SearchPanel extends JXPanel {

    /** the searcher */
    Searcher searcher = new Searcher(null, new SearchListener() {

        @Override
        public void matchFound(Collection<? extends SieTemplate> variables) {
            listModel.add(variables);
        }

        @Override
        public void searchFailed(Exception exception) {
            searchTerminated();
            JOptionPane.showMessageDialog(textField, exception.toString(),
              "Search Failed", JOptionPane.ERROR_MESSAGE);
        }

        @Override
        public void searchCanceled() {
            searchTerminated();
        }

        @Override
        public void searchComplete() {
            searchTerminated();
        }

        /**
         * updates the progress bar's state
         */
        void searchTerminated() {
            cancelButton.setVisible(false);
            if (progressBar.isIndeterminate()) {
                progressBar.setIndeterminate(false);
            }
        }

    }, new PropertyChangeListener() {
        @Override
        public void propertyChange(PropertyChangeEvent propertyChangeEvent) {
            String name = propertyChangeEvent.getPropertyName();
            if (name.equals("progress")) {
                int progress = (Integer)propertyChangeEvent.getNewValue();
                progressBar.setValue(progress);
                progressBar.setString((int)(progress / 100d * searcher.elementCount) +
                  " / " + searcher.elementCount +  " (" + progress + "%)");
            }
            else if (name.equals("count")) {
                progressBar.setIndeterminate(true);
                progressBar.setString(Long.toString(searcher.elementCount));
            }
        }
    });

    /** the text for which to search */
    JXTextField textField = new JXTextField() {{

        setAction(new AbstractAction() {

            {
            putValue(SHORT_DESCRIPTION, "Search for the specified text.");
            }

            @Override
            public void actionPerformed(ActionEvent actionEvent) {
                listModel.clear();
                searcher.search(textField.getText().trim(),
                  caseSensitiveCheckBox.isSelected(),
                  regularExpressionCheckBox.isSelected());
                if (searcher.elementCount == 0) {
                    progressBar.setIndeterminate(true);
                }
                cancelButton.setVisible(true);
            }
        });
    }};

    /** toggles case-insensitive searching */
    JCheckBox caseSensitiveCheckBox = new JCheckBox("Case Sensitive") {{
        setName("caseSensitiveCheckBox");
        setToolTipText("Toggle case sensitive searching.");
    }};

    /** toggles regular expression searching */
    JCheckBox regularExpressionCheckBox = new JCheckBox("Regular Expression") {{
        setName("regularExpressionCheckBox");
        setToolTipText("Toggle regular expression searching.");
    }};

    /** search results list model */
    EfficientListModel listModel = new EfficientListModel();

    /** cancel search button */
    JXButton cancelButton = new JXButton(new AbstractAction() {
        {
        putValue(NAME, "Cancel");
        putValue(SHORT_DESCRIPTION, "Stop searching.");
        }
        @Override
        public void actionPerformed(ActionEvent actionEvent) {
            cancelSearch();
        }
    }) {{
        setVisible(false);
    }};

    /** displays progress */
    JProgressBar progressBar = new JProgressBar(0, 100) {{
        setString("");
        setStringPainted(true);
    }};

    /** the action associated with pressing enter on this list */
    Action action;

    /** popup menu */
    JPopupMenu popupMenu = new JPopupMenu();

    /** search results list */
    JXList list = new JXList(listModel) {{
        setComponentPopupMenu(popupMenu);
        setHighlighters(HighlighterFactory.createSimpleStriping());

        // Add tool tips to cells.
        setCellRenderer(new DefaultListCellRenderer() {
            @Override
            public Component getListCellRendererComponent(JList list, Object value, int index,
              boolean isSelected, boolean cellHasFocus) {
                JComponent component = (JComponent)super.getListCellRendererComponent(list,
                  value, index, isSelected, cellHasFocus);
                component.setToolTipText(((SieTemplate)value).getToolTipText());
                return component;
            }
        });

        // Add keyboard support.
        getInputMap().put(KeyStroke.getKeyStroke(KeyEvent.VK_ENTER, 0), "SearchPanel.actionPerformed");

        // Add mouse support.
        addMouseListener(new MouseAdapter() {
            @Override
            public void mousePressed(MouseEvent event) {
                if (event.getClickCount() == 2 && event.getButton() == MouseEvent.BUTTON1) {
                    // Ctrl-clicks toggle the selection status of an element, but a double
                    // click should always add it to the selection.
                    if (action.isEnabled()) {
                        if ((event.getModifiersEx() & InputEvent.CTRL_DOWN_MASK) == InputEvent.CTRL_DOWN_MASK) {
                            int index = locationToIndex(event.getPoint());
                            addSelectionInterval(index, index);
                        }
                        action.actionPerformed(new ActionEvent(list, 0, null));
                    }
                }
            }
        });
    }};

    /**
     * constructor
     */
    public SearchPanel() {
        setLayout(new GridBagLayout());

        GridBagConstraints constraints = new GridBagConstraints() {{
            gridx = 0;
            weightx = 1;
            weighty = 0;
            fill = BOTH;
        }};

        add(new JXPanel(new BorderLayout()) {{
            add(new JXLabel(" Search: "), BorderLayout.WEST);
            add(textField, BorderLayout.CENTER);
        }}, constraints);

        add(new JXPanel() {{
            add(new JXPanel() {{
                setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
                add(caseSensitiveCheckBox);
                add(regularExpressionCheckBox);
            }});
        }}, constraints);

        constraints.weighty = 1;
        add(new JScrollPane(list), constraints);

        constraints.weighty = 0;
        add(cancelButton, constraints);
        add(progressBar, constraints);

    }

    /**
     * sets the action associated with pressing enter on this list
     *
     * @param action the action to perform
     */
    public void setAction(Action action) {
        this.action = action;

        list.getActionMap().put("SearchPanel.actionPerformed", action);

        // Configure the popup menu.
        try {
            popupMenu.remove(0);
        }
        catch (IllegalArgumentException e) {}
        popupMenu.add(new JMenuItem(action), 0);
    }

    /**
     * adds <code>menuItem</code> to this component's popup menu
     *
     * @param menuItem the menu item to add
     */
    public void addPopupMenuItem(JMenuItem menuItem) {
        if (menuItem != null) {
            popupMenu.add(menuItem);
        }
    }

    /**
     * returns the selection
     *
     * @return the current selection
     */
    public List<SieTemplate> getSelectedValues() {
        ArrayList<SieTemplate> result = new ArrayList<SieTemplate>();
        for (Object object : list.getSelectedValues()) {
            result.add((SieTemplate)object);
        }
        return result;
    }

    /**
     * sets the root node from which searches will begin
     *
     * @param rootTemplates the template from which to begin searches
     */
    public void setRootTemplates(Collection<SieTemplate> rootTemplates) {
        searcher.setRootTemplates(rootTemplates);
        progressBar.setString("");
        progressBar.setValue(progressBar.getMinimum());
        progressBar.setIndeterminate(false);
    }

    /**
     * clears this list
     */
    public void clear() {
        listModel.clear();
    }

    @Override
    public void setEnabled(boolean enabled) {
        super.setEnabled(enabled);
        textField.setEnabled(enabled);
        caseSensitiveCheckBox.setEnabled(enabled);
        regularExpressionCheckBox.setEnabled(enabled);
        list.setEnabled(enabled);
        list.setComponentPopupMenu(enabled ? popupMenu : null);
    }

    /**
     * sets the search result list's font size
     *
     * @param fontSize the desired size of the font
     */
    public void setFontSize(float fontSize) {
        list.setFont(list.getFont().deriveFont(fontSize));
    }

    /**
     * halts the executing search, if any
     */
    public void cancelSearch() {
        searcher.cancelSearch();
    }

    /**
     * a list model that allows multiple elements to be added simultaneously,
     * resulting in only one call to <code>fireIntervalAdded</code>
     */
    protected class EfficientListModel extends AbstractListModel {

        /** contents of this list */
        ArrayList<SieTemplate> contents = new ArrayList<SieTemplate>();

        /**
         * adds <code>elements</code> to this list
         *
         * @param elements the elements to be added
         */
        void add(Collection<? extends SieTemplate> elements) {
            int start = contents.size();
            contents.addAll(elements);
            fireIntervalAdded(this, start, contents.size() - 1);
        }

        @Override
        public SieTemplate getElementAt(int index) {
            return contents.get(index);
        }

        @Override
        public int getSize() {
            return contents.size();
        }

        /**
         * removes all elements from this list
         */
        void clear() {
            int end = contents.size() - 1;
            if (end > -1) {
                contents.clear();
                fireIntervalRemoved(this, 0, end);
            }
        }

    }

}
