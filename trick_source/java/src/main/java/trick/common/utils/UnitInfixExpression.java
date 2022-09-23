
//========================================
//  Package
//========================================
package trick.common.utils;

import java.util.HashMap;
import java.util.Map;
import java.util.Stack;

public class UnitInfixExpression {

	//========================================
    //    Public data
    //========================================

	public static final String OPERATORS="/*+-()123"; //ignore the "(" as an operator
    public static final Map<Character, Integer> OPERATOR_LEVELS = new HashMap<Character, Integer>();
    static {
        OPERATOR_LEVELS.put(')', 0);
        OPERATOR_LEVELS.put('1', 1);
        OPERATOR_LEVELS.put('2', 1);
        OPERATOR_LEVELS.put('3', 1);
        OPERATOR_LEVELS.put('*', 2);
        OPERATOR_LEVELS.put('/', 2);
        OPERATOR_LEVELS.put('+', 3);
        OPERATOR_LEVELS.put('-', 3);
        OPERATOR_LEVELS.put('(', 4);
    }
    
    //========================================
    //    Protected data
    //========================================

    
    
    //========================================
    //    Private Data
    //========================================	
    private String inputExpression = null;
    
		
	//========================================
    //    Constructors
    //========================================
	public UnitInfixExpression(String expression) {
		inputExpression = expression;
	}

	
	//========================================
    //    Methods
    //========================================
	/**
	 * Returns a derived unit based on for the unit expression.
         * @return a derived unit based on for the unit expression
	 */
	public UnitType.Unit getUnit() {
		UnitType.Unit ret = null;
		
		Stack<UnitType.Unit> unitStack = new Stack<UnitType.Unit>();
	    Stack<Character> operationStack = new Stack<Character>();
	    
	    StringBuffer unitBuf = new StringBuffer();
	    
	    for (int i = 0; i < inputExpression.length(); i++ ) {
            char eachChar = inputExpression.charAt(i);

            if (eachChar==' '){
                 //ignore
            }
            else if (isValidOperator(eachChar)){
                if (!isLeftParen(eachChar) && unitBuf.length() > 0) {              	
                    unitStack.push(UnitType.getPrimitiveUnit(unitBuf.toString()));                    
                    unitBuf.setLength(0);                
                }

                if (operationStack.isEmpty() || isLeftParen(eachChar)){ //first item or left parenthesis
                	operationStack.push(eachChar);
                } 
                // if right parenthesis, evaluating the expression within the parenthesizes
                else if (isRightParen(eachChar)){
                    evaluateUntilLeftParenthesis(unitStack, operationStack);

                }
                //if the current operator has higher level than the first item in the stack, insert it
                else if (getHigherPrecedenceOperator(eachChar, operationStack.peek()) == eachChar){
                    operationStack.push(eachChar);
                }
                //if the current operator has lower level, then the previous higher level of operation needs evaluating
                else{
                    evaluateAndPushDerivedUnit(unitStack, operationStack);

                    //after evaluation of the higher pair, insert current operator
                    operationStack.push(eachChar);
                }
            } else if (Character.isLetter(eachChar) || eachChar == '.' || eachChar == '-') { // if the character is a letter, appends to the unit string buffer
                unitBuf.append(eachChar);
                if ( i == inputExpression.length()-1){
                    unitStack.push(UnitType.getPrimitiveUnit(unitBuf.toString())) ; //the last element
                }
            }
        }

	    //System.out.println("After all Unit Stack : "+unitStack);
        //System.out.println("After all Operation Stack : "+operationStack);

        while (!operationStack.empty()){
            evaluateAndPushDerivedUnit(unitStack, operationStack);
        }
        
	    if (unitStack != null && unitStack.size() > 0) {
	    	return unitStack.pop();
	    }
	    return ret;
	}
	
	/**
	 * Helper method to check if the specified character is a right parenthesis.
	 */
    private boolean isRightParen(Character operator) {
        
        return operator == ')' ? true : false;
    }

    /**
	 * Helper method to check if the specified character is a left parenthesis.
	 */
    private boolean isLeftParen(Character operator) {
        return operator == '(' ? true : false;
    }
    
    /**
     * Helper method for evaluating units and pushing the intermediate unit to the unit stack.
     */
    private void evaluateAndPushDerivedUnit(Stack<UnitType.Unit> unitStack, Stack<Character> operationStack) {   	
    	UnitType.Unit unit1 = null;
    	UnitType.Unit unit2 = null;
    	String derivedUnitName = "";
    	UnitType.Unit derivedUnit = null;
    	
    	Character operator = operationStack.pop();
    	    
    	switch (operator) {
    	    case '2':    	    	
    	    case '3':
    	    	unit1 = unitStack.pop();
    	    	derivedUnitName = unit1.abbreviation + operator;
    	    	derivedUnit = new UnitType.Unit(derivedUnitName, derivedUnitName, false);
    	    	derivedUnit.factor1 = unit1.factor1;
    	    	if (operator == '2') {
    	    		derivedUnit.factor1 = unit1.factor1 * unit1.factor1;
    	    		derivedUnit.factor2 = unit1.factor2 * unit1.factor2;
    	    	} else {
    	    		derivedUnit.factor1 = unit1.factor1 * unit1.factor1 * unit1.factor1;
    	    		derivedUnit.factor2 = unit1.factor2 * unit1.factor2 * unit1.factor2;
    	    	}  	    	
    	    	break;
    	    case '*':
    	    case '/':   	    	
    	    	unit2 = unitStack.pop();
    	    	unit1 = unitStack.pop();
    	    	derivedUnitName = unit1.abbreviation + operator + unit2.abbreviation;
    	    	derivedUnit = new UnitType.Unit(derivedUnitName, derivedUnitName, false);
    	    	if (operator == '*' ) {
    	    		derivedUnit.factor1 = unit1.factor1 * unit2.factor1;
    	    		derivedUnit.factor2 = unit1.factor2 * unit2.factor2;
    	    	} else {
    	    		if (unit2.factor1 == 0.0) {
    	    			derivedUnit.factor1 = unit1.factor1;
    	    		} else {
    	    			derivedUnit.factor1 = unit1.factor1 / unit2.factor1;
    	    		}
    	    		if (unit2.factor2 == 0.0) {
    	    			derivedUnit.factor2 = unit1.factor2;
    	    		} else {
    	    			derivedUnit.factor2 = unit1.factor2 / unit2.factor2;
    	    		}
    	    	}
    	    	break;
    	}
    	
    	if (derivedUnit != null) {
    		unitStack.push(derivedUnit);
    		//System.out.println("derived unit " + derivedUnit.abbreviation + " factor1=" + derivedUnit.factor1 + "; factor2="+derivedUnit.factor2);
    	}
    }

    /**
     * Helper method for evaluating the expression until a left parenthesis.
     */
    private void evaluateUntilLeftParenthesis(Stack<UnitType.Unit> unitStack, Stack<Character> operationStack) {

        while (!operationStack.empty()){
            if (isLeftParen(operationStack.peek())){ //if the left parenthesis has been reached, then pop it up and exit
                operationStack.pop();
                break;
            }
            evaluateAndPushDerivedUnit(unitStack,operationStack);
        }

    }

    /**
     * Helper method to check if the specified character is a valid operator.
     */
    private boolean isValidOperator(char input) {

        if (OPERATORS.indexOf(input) != -1) {
            return true;
        }
        return false;
        
    }

    /**
     * Helper method for getting a higher level operator based on the predefined operator levels.
     */
    private Character getHigherPrecedenceOperator(Character firstOperator, Character secondOperator){

        return OPERATOR_LEVELS.get(firstOperator)<OPERATOR_LEVELS.get(secondOperator) ? firstOperator : secondOperator;

    }

}
