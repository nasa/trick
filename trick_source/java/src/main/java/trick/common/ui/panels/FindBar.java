//========================================
//  Package
//========================================
package trick.common.ui.panels;

//========================================
//Imports
//========================================

import javax.swing.*;
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;
import javax.swing.text.*;

import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import java.util.ArrayList;
import java.util.List;
import java.util.regex.MatchResult;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.util.regex.PatternSyntaxException;

import org.jdesktop.swingx.JXEditorPane;

/**
 * @since Trick 10
 * A {@link JXFindBar} that allows users to input search text.
 * This class extends JXFindBar, a SwingX component providing search bar functionality for a JXEditorPane.
 *
 * Notes for not extending JXFindBar:
 * JXFindBar is a SwingX component extending JXFindPanel.
 * JXFindPanel offers various search options, allowing users to input search text.
 * When JXFindPanel performs a search, it calls the search method in the Searchable object.
 * The Searchable search method is invoked when findNext or findPrevious is clicked.
 * The JXEditorPane's Matcher calls toMatchResult() where the search text is used to find the match.
 * The toMatchResult method is called in the Matcher class and creates a new Matcher, copying the state over.
 * The toMatchResult method returns a {@link MatchResult} object.
 * This works until JDK 9.
 * JDK 9 introduces a new, non-public {@link java.util.regex.Matcher$ImmutableMatchResult} class that is the result of toMatchResult().
 * Therefore, a Matcher can't be cloned and the state can't be copied over. The following exception would be seen:
 * java.lang.ClassCastException: class java.util.regex.Matcher$ImmutableMatchResult cannot be cast to class java.util.regex.Matcher 
 * (java.util.regex.Matcher$ImmutableMatchResult and java.util.regex.Matcher are in module java.base of loader 'bootstrap')
 *
 * @since Trick 19
 * This class no longer extends JXFindBar. It extends {@link JPanel} instead.
 * FindBar is a custom JPanel that provides search functionality for a JXEditorPane.
 * It allows users to search for text within the editor pane and navigate through the matches.
 *
 * <p>Usage example:</p>
 * <pre>
 * {@code
 * JXEditorPane editorPane = new JXEditorPane();
 * FindBar findBar = new FindBar(editorPane);
 * }
 * </pre>
 */
public class FindBar extends JPanel {

    // ========================================
    // Public data
    // ========================================

    // ========================================
    // Protected data
    // ========================================

    // ========================================
    // Private Data
    // =======================================

    /**
     * A text field for entering search queries.
     */
    private JTextField searchField;
    /**
     * A button that, when pressed, triggers the action to find the next occurrence
     * of the search term in the text or document.
     */
    private JButton findNextButton;
    /**
     * JButton used to trigger the action of finding the previous occurrence
     * in a search operation within the UI.
     */
    private JButton findPreviousButton;
    /**
     * The JXEditorPane component used for displaying and editing text content.
     */
    private JXEditorPane editorPane;
    /**
     * A list of integer positions.
     * This list stores the positions of all find matches.
     */
    private List<Integer> matchPosList;
    /**
     * A list of integers representing the lengths of each find match.
     */
    private List<Integer> matchLenList;
    /**
     * The current index used for tracking the position within a list or array.
     * This variable is typically used to keep track of the current item being
     * processed or displayed.
     */
    private int currentIndex;

    // ========================================
    // Constructors
    // ========================================
    /**
     * Constructs a new FindBar with the specified JXEditorPane.
     *
     * <p>
     * The FindBar consists of a JTextField for entering the search text, and two buttons 
     * for navigating to the next and previous matches. It also highlights the current match 
     * in the editor pane.
     * </p>
     *
     * <p>
     * Features:
     * </p>
     * <ul>
     * <li>Real-time search and highlight as the user types in the search field.</li>
     * <li>Navigation buttons to find the next and previous matches.</li>
     * </ul>
     *
     * @param editorPane The JXEditorPane in which the search will be performed.
     */
    public FindBar(JXEditorPane editorPane) {
        this.editorPane = editorPane;
        setLayout(new FlowLayout(FlowLayout.LEFT));

        searchField = new JTextField(20);
        findNextButton = new JButton("Find Next");
        findPreviousButton = new JButton("Find Previous");

        // Add a DocumentListener to the search field to trigger search and highlight on
        // text change as real-time search.
        // The search field background color is set to red if no matches are found
        // otherwise it is set to white.
        searchField.getDocument().addDocumentListener(new DocumentListener() {
            @Override
            public void insertUpdate(DocumentEvent e) {
                searchAndHighlight();
            }

            @Override
            public void removeUpdate(DocumentEvent e) {
                searchAndHighlight();
            }

            @Override
            public void changedUpdate(DocumentEvent e) {
                searchAndHighlight();
            }

            private void searchAndHighlight() {
                String searchText = searchField.getText();
                findMatches(searchText);
                highlightCurrentMatch();
                if (matchPosList == null || matchPosList.isEmpty()) {
                    searchField.setBackground(Color.RED);
                } else {
                    searchField.setBackground(Color.WHITE);
                }
            }
        });

        // Add an ActionListener to the search field to trigger search and highlight on Enter key press.
        searchField.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                String searchText = searchField.getText();
                findMatches(searchText);
                highlightCurrentMatch();
            }
        });

        // Add ActionListeners to the find next button.
        findNextButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                if (matchPosList != null && !matchPosList.isEmpty()) {
                    currentIndex = (currentIndex + 1) % matchPosList.size();
                    highlightCurrentMatch();
                }
            }
        });

        // Add ActionListeners to the find previous button.
        findPreviousButton.addActionListener(new ActionListener() {
            @Override
            public void actionPerformed(ActionEvent e) {
                if (matchPosList != null && !matchPosList.isEmpty()) {
                    currentIndex = (currentIndex - 1 + matchPosList.size()) % matchPosList.size();
                    highlightCurrentMatch();
                }
            }
        });

        add(new JLabel("Find:"));
        add(searchField);
        add(findNextButton);
        add(findPreviousButton);
    }

    // ========================================
    // Set/Get methods
    // ========================================

    // ========================================
    // Methods
    // ========================================
    /**
     * Finds and highlights all matches of the given search text in the editor pane.
     *
     * @param searchText The text to search for within the editor pane.
     * @throws PatternSyntaxException If the regular expression's syntax is invalid.
     * @throws BadLocationException   If the document's text cannot be retrieved.
     */
    private void findMatches(String searchText) {
        try {
            Highlighter highlighter = editorPane.getHighlighter();
            Highlighter.HighlightPainter painter = new DefaultHighlighter.DefaultHighlightPainter(Color.YELLOW);
            highlighter.removeAllHighlights();

            Document doc = editorPane.getDocument();
            String text = doc.getText(0, doc.getLength());
            matchPosList = new ArrayList<>();
            matchLenList = new ArrayList<>();
            Pattern pattern = Pattern.compile(searchText, Pattern.CASE_INSENSITIVE);
            Matcher matcher = pattern.matcher(text);

            // Find all matches of the search text in the document.
            // Add the start to matchPosList and the length to matchLenList.
            while (matcher.find()) {
                matchPosList.add(matcher.start());
                matchLenList.add(matcher.end() - matcher.start());
            }

            // Reset the current index to 0 and highlight the first match if available.
            currentIndex = 0;
            if (!matchPosList.isEmpty()) {
                // Highlight the current match.
                int pos = matchPosList.get(currentIndex);
                highlighter.addHighlight(pos, pos + matchLenList.get(currentIndex), painter);
                // Set the caret position to the start of the current match.
                editorPane.setCaretPosition(pos);
            }
        } catch (PatternSyntaxException e) {
            JOptionPane.showMessageDialog(this, "Invalid regular expression: " + e.getDescription(), "Error",
                    JOptionPane.ERROR_MESSAGE);
        } catch (BadLocationException e) {
            e.printStackTrace();
        }
    }

    /**
     * Highlights the current match in the editor pane.
     * 
     * This method uses the Highlighter to highlight the text in the editor pane
     * that matches the current search term. It removes any existing highlights
     * before applying the new highlight. If there are positions available in the
     * list, it highlights the text at the current index position and sets the
     * caret position to the start of the highlighted text.
     * 
     * @throws BadLocationException if the position is invalid in the document model
     */
    private void highlightCurrentMatch() {
        try {
            Highlighter highlighter = editorPane.getHighlighter();
            Highlighter.HighlightPainter painter = new DefaultHighlighter.DefaultHighlightPainter(Color.YELLOW);
            highlighter.removeAllHighlights();

            if (matchPosList != null && !matchPosList.isEmpty()) {
                int pos = matchPosList.get(currentIndex);
                // Highlight the current match.
                highlighter.addHighlight(pos, pos + searchField.getText().length(), painter);
                // Set the caret position to the start of the current match.
                editorPane.setCaretPosition(pos);
            }
        } catch (BadLocationException e) {
            e.printStackTrace();
        }
    }

}