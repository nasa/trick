package trick.tv;

import java.awt.Component;
import java.awt.event.ActionEvent;
import java.awt.event.InputEvent;
import java.awt.event.KeyEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import javax.swing.Action;
import javax.swing.JComponent;
import javax.swing.JMenuItem;
import javax.swing.JPopupMenu;
import javax.swing.JTree;
import javax.swing.KeyStroke;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeCellRenderer;
import javax.swing.tree.TreePath;

import trick.common.utils.vs.Variable;
import trick.sie.utils.SieTemplate;
import trick.sie.utils.SieTree;

/**
 * represents a variable containment hierarchy as described by an S_sie.resource file
 *
 * @author Derek Bankieris
 */
public class TVVariableTree extends SieTree {
    
    /** the action associated with pressing enter on the list */
    Action action;
    
    /** popup menu */
    JPopupMenu popupMenu;
    
    /**
     * constructor
     */
    public TVVariableTree() {
        super();
        
        popupMenu = new JPopupMenu();
        setComponentPopupMenu(popupMenu);

        // Add keyboard support.
        getInputMap().put(KeyStroke.getKeyStroke(KeyEvent.VK_ENTER, 0), "TVVariableTree.actionPerformed");

        // Add mouse support.
        addMouseListener(new MouseAdapter() {
            public void mousePressed(MouseEvent event) {
                int button = event.getButton();

                if (event.getClickCount() == 2 && button == MouseEvent.BUTTON1) {
                    TreePath path = getPathForLocation(event.getX(), event.getY());

                    if (path != null) {
                        int modifiers = event.getModifiersEx();

                        // Ctrl-clicks toggle the selection status of a node, but a double
                        // click should always add it to the selection.
                        if ((modifiers & InputEvent.CTRL_DOWN_MASK) == InputEvent.CTRL_DOWN_MASK) {
                            addSelectionPath(path);
                        }

                        // If the user double clicked a leaf node, or any node while holding
                        // down Ctrl or Shift, fire the action.
                        if (treeModel.isLeaf(path.getLastPathComponent()) ||
                          (modifiers & InputEvent.CTRL_DOWN_MASK) ==  InputEvent.CTRL_DOWN_MASK ||
                          (modifiers & InputEvent.SHIFT_DOWN_MASK) == InputEvent.SHIFT_DOWN_MASK) {
                            action.actionPerformed(new ActionEvent(treeModel, 0, ""));
                        }
                    }
                }
            }
        });
    }

    /**
     * sets the action associated with pressing enter on or double clicking a tree node
     *
     * @param action the action to perform
     */
    public void setAction(Action action) {
        this.action = action;

        getActionMap().put("TVVariableTree.actionPerformed", action);

        // Configure the popup menu.
        popupMenu.removeAll();
        popupMenu.add(new JMenuItem(action));
    }    
    
    @Override
    public void setEnabled(boolean enabled) {
        super.setEnabled(enabled);
        setComponentPopupMenu(enabled ? popupMenu : null);
    }
    
    /**
     * creates a new variable named <code>name</code> from <code>template</code>
     *
     * @param name the new variable's name
     * @param template the template from which to create the new variable
     *
     * @return the new variable
     */
    public static Variable<TrickViewFluent> createVariableFromTemplate(String name, SieTemplate template) {
        if (template != null) {
            boolean unsigned = template.typeName.contains("unsigned");

            if (template.typeName.contains("bool")) {
                return new Variable<TrickViewFluent>(name, new TVBoolean(), template.units);
            }
            else if (template.typeName.contains("char")) {
                /**
                 * the variable server will treat character arrays as strings
                 * if the final index is not specified
                 */
                Matcher matcher = Pattern.compile("\\[\\d+\\]").matcher(
                  name.substring(name.lastIndexOf('.') + 1));
                int count = 0;
                while (matcher.find()) {
                    ++count;
                }
                if (template.dimensions.length > count) {
                    return new Variable<TrickViewFluent>(name, new TVString(), template.units);
                }
                return new Variable<TrickViewFluent>(name, new TVByte(unsigned), template.units);
            }
            else if (template.typeName.contains("short")) {
                return new Variable<TrickViewFluent>(name, new TVShort(unsigned), template.units);
            }
            else if (template.typeName.contains("long")) {
                return new Variable<TrickViewFluent>(name, new TVLong(unsigned), template.units);
            }
            else if (template.typeName.contains("string") || template.typeName.contains("_IO_FILE")) {
                return new Variable<TrickViewFluent>(name, new TVString(), template.units);
            }
            else if (template.enumeration != null) {
                return new Variable<TrickViewFluent>(name, new TVEnumeration(template.enumeration), template.units);
            }
            else if (template.typeName.contains("float")) {
                return new Variable<TrickViewFluent>(name, new TVFloat(), template.units);
            }
            else if (template.typeName.contains("double")) {
                return new Variable<TrickViewFluent>(name, new TVDouble(), template.units);
            }
            else {
                return new Variable<TrickViewFluent>(name, new TVInteger(unsigned), template.units);
            }
        }
        return new Variable<TrickViewFluent>(name, new TVString());
    }
}
